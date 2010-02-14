#include "GL/GLProgram.h"
#include "GL/GLDevice.h"
#include "GL/GLUniform.h"
#include "GL/GLUtility.h"
#include "GL/GLVertexBuffer.h"
#include <algorithm>
#include <functional>

using namespace sgl;
using namespace math;

namespace {

// get number of uniforms and summary size
struct uniform_desc
{
    GLenum      type;
    GLsizei     size;
    GLuint      index;
    GLuint      location;
    std::string name;
};

}

// program
GLProgram::GLProgram(Device* _device) :
    device( static_cast<GLDevice*>(_device) ),
    uniforms(0),
    numVerticesOut(3),
    inputType(TRIANGLES),
    outputType(TRIANGLE_STRIP),
    glProgram(0),
    dirty(true)
{
    glProgram = glCreateProgram();
}

GLProgram::~GLProgram()
{
    glDeleteProgram(glProgram);
    if (uniforms) {
        delete[] uniforms;
    }
}

// shaders
SGL_HRESULT GLProgram::AddShader(Shader* shader)
{
#ifndef SGL_NO_STATUS_CHECK
    if (!shader) {
		return EInvalidCall("GLProgram::AddShader failed. Shader is NULL");
    }
#endif
    shaders.push_back( ref_ptr<GLShader>( static_cast<GLShader*>(shader) ) );
    dirty = true;

    return SGL_OK;
}

bool GLProgram::RemoveShader(Shader* shader)
{
    shader_vector::iterator iter = std::find( shaders.begin(), shaders.end(), ref_ptr<Shader>(shader) );
    if ( iter != shaders.end() ) 
    {
        std::swap(*iter, shaders.back() );
        shaders.pop_back();
        dirty = true;
        return true;
    }

    return false;
}

AbstractUniform* GLProgram::CreateUniform( GLProgram*   program,
                                           const char*  name,
                                           GLuint       glIndex,
                                           GLuint       glLocation,
                                           GLenum       glUniformType,
                                           size_t       size )
{
    typedef GLUniform<float>      uniform1f;
    typedef GLUniform<Vector2f>   uniform2f;
    typedef GLUniform<Vector3f>   uniform3f;
    typedef GLUniform<Vector4f>   uniform4f;

    typedef GLUniform<int>        uniform1i;
    typedef GLUniform<Vector2i>   uniform2i;
    typedef GLUniform<Vector3i>   uniform3i;
    typedef GLUniform<Vector4i>   uniform4i;

    typedef GLUniform<Matrix2f>   uniform2x2f;
    typedef GLUniform<Matrix3f>   uniform3x3f;
    typedef GLUniform<Matrix4f>   uniform4x4f;

    switch(glUniformType)
    {
    case GL_FLOAT:
        return new uniform1f( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_FLOAT_VEC2:
        return new uniform2f( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_FLOAT_VEC3:
        return new uniform3f( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_FLOAT_VEC4:
        return new uniform4f( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_INT:
        return new uniform1i( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_INT_VEC2:
        return new uniform2i( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_INT_VEC3:
        return new uniform3i( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_INT_VEC4:
        return new uniform4i( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_BOOL:
        return new uniform1i( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_BOOL_VEC2:
        return new uniform2i( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_BOOL_VEC3:
        return new uniform3i( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_BOOL_VEC4:
        return new uniform4i( program,
                              name,
                              glIndex,
                              glLocation,
                              size );

    case GL_FLOAT_MAT2:
        return new uniform2x2f( program,
                                name,
                                glIndex,
                                glLocation,
                                size );

    case GL_FLOAT_MAT3:
        return new uniform3x3f( program,
                                name,
                                glIndex,
                                glLocation,
                                size );

    case GL_FLOAT_MAT4:
        return new uniform4x4f( program,
                                name,
                                glIndex,
                                glLocation,
                                size );

    case GL_SAMPLER_1D_SHADOW:
    case GL_INT_SAMPLER_1D_EXT:
    case GL_SAMPLER_1D:
    {
        GLSamplerUniform<Texture1D>* uniform;
        uniform = new GLSamplerUniform<Texture1D>( program,
                                                   name,
                                                   glIndex,
                                                   glLocation );
        return uniform;
    }

    case GL_SAMPLER_2D_SHADOW:
    case GL_INT_SAMPLER_2D_EXT:
    case GL_SAMPLER_2D:
    {
        GLSamplerUniform<Texture2D>* uniform;
        uniform = new GLSamplerUniform<Texture2D>( program,
                                                   name,
                                                   glIndex,
                                                   glLocation );
        return uniform;
    }

    case GL_INT_SAMPLER_3D_EXT:
    case GL_SAMPLER_3D:
    {
        GLSamplerUniform<Texture3D>* uniform;
        uniform = new GLSamplerUniform<Texture3D>( program,
                                                   name,
                                                   glIndex,
                                                   glLocation );
        return uniform;
    }

    case GL_SAMPLER_CUBE:
    {
        GLSamplerUniform<TextureCube>* uniform;
        uniform = new GLSamplerUniform<TextureCube>( program,
                                                     name,
                                                     glIndex,
                                                     glLocation );
        return uniform;
    }

    default:
        return 0;
    }

    return 0;
}

// Work
SGL_HRESULT GLProgram::Dirty(bool force)
{
    if ( !force && !dirty ) {
        return SGL_OK;
    }

#ifndef SGL_NO_STATUS_CHECK
    if ( device->CurrentProgram() == this ) {
        return EInvalidCall("GLProgram::Dirty failed. Can't dirty program while it is binded.");
    }
#endif

    // attach/detach shaders
    {
        GLint numShaders;
        glGetProgramiv(	glProgram, 
 	                    GL_ATTACHED_SHADERS, 
 	                    &numShaders );

        std::vector<GLuint> attachedShaders(numShaders);
        if (numShaders > 0)
        {
            glGetAttachedShaders( glProgram, 
 	                              numShaders, 
 	                              &numShaders, 
 	                              &attachedShaders[0] );
        }
        std::sort( attachedShaders.begin(), attachedShaders.end() );

        std::vector<GLuint> programShaders( shaders.size() );
        for (size_t i = 0; i<shaders.size(); ++i) {
            programShaders[i] = shaders[i]->shader;
        }
        std::sort( programShaders.begin(), programShaders.end() );

        std::vector<GLuint> forDetaching;
        std::set_difference( attachedShaders.begin(), 
                             attachedShaders.end(), 
                             programShaders.begin(), 
                             programShaders.end(), 
                             std::back_inserter(forDetaching) );

        std::vector<GLuint> forAttaching;
        std::set_difference( programShaders.begin(), 
                             programShaders.end(), 
                             attachedShaders.begin(), 
                             attachedShaders.end(), 
                             std::back_inserter(forAttaching) );

        for (size_t i = 0; i<forDetaching.size(); ++i) {
            glDetachShader(glProgram, forDetaching[i]);
        }

        for (size_t i = 0; i<forAttaching.size(); ++i) {
            glAttachShader(glProgram, forAttaching[i]);
        }
    }

    // link
    {
        if (glProgramParameteriEXT)
        {
            glProgramParameteriEXT(glProgram, GL_GEOMETRY_INPUT_TYPE_EXT,   BIND_PRIMITIVE_TYPE[inputType]);
            glProgramParameteriEXT(glProgram, GL_GEOMETRY_OUTPUT_TYPE_EXT,  BIND_PRIMITIVE_TYPE[outputType]);
            glProgramParameteriEXT(glProgram, GL_GEOMETRY_VERTICES_OUT_EXT, numVerticesOut);
        }
        glLinkProgram(glProgram);

        // error
        GLsizei errLength;
        glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &errLength);
        compilationLog.resize(errLength);
        glGetProgramInfoLog(glProgram, errLength, &errLength, &compilationLog[0]);
        compilationLog.resize(errLength);

        // check link status
        GLint linkStatus;
        glGetProgramiv(glProgram, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            return EInvalidCall( compilationLog.c_str() );
        }
    }

    // get uniform longest name and allocate space for it
    GLint uniformMaxNameLength;
    glGetProgramiv(glProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxNameLength);
    std::string uniformName;
    uniformName.resize(uniformMaxNameLength + 1);

    // get active uniforms
    size_t                    dataSize = 0;
    std::vector<uniform_desc> uniformDescriptions;
    {
        glGetProgramiv(glProgram, GL_ACTIVE_UNIFORMS, (GLint*)&numActiveUniforms);
        uniformDescriptions.resize(numActiveUniforms);

        for(unsigned int i = 0; i<numActiveUniforms; ++i)
        {
            GLenum  uniformType;
            GLsizei nameLength;
            GLsizei uniformSize;

            glGetActiveUniform( glProgram,
                                i,
                                uniformMaxNameLength,
                                &nameLength,
                                &uniformSize,
                                &uniformType,
                                &uniformName[0] );

            uniformDescriptions[i].type     = uniformType;
            uniformDescriptions[i].size     = uniformSize;
            uniformDescriptions[i].index    = i;
            uniformDescriptions[i].location = glGetUniformLocation( glProgram, uniformName.c_str() );
            uniformDescriptions[i].name     = uniformName;
        }
    }

    // create uniforms
    {
        if (uniforms) {
            delete[] uniforms;
        }

        uniforms = new uniform_ptr[ uniformDescriptions.size() ];
        for(size_t i = 0; i<uniformDescriptions.size(); ++i)
        {
            // create uniform
            uniforms[i].reset( CreateUniform( this,
                                              uniformDescriptions[i].name.c_str(),
                                              uniformDescriptions[i].index,
                                              uniformDescriptions[i].location,
                                              uniformDescriptions[i].type,
                                              uniformDescriptions[i].size ) );
        }
    }

    // enumerate attributes
    {
        int numAttributes;
        glGetProgramiv(glProgram, GL_ACTIVE_ATTRIBUTES, &numAttributes);

        GLsizei maxNameLength;
        glGetProgramiv(glProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);

        for (int i = 0; i<numAttributes; ++i)
        {
            attribute attr;
            attr.name.resize(maxNameLength);

            GLsizei nameLength;
            GLenum  type;
            GLint   size;
            glGetActiveAttrib( glProgram,
 	                           i, 
 	                           maxNameLength, 
 	                           &nameLength, 
 	                           &size, 
 	                           &type, 
 	                           &attr.name[0] );

            if (attr.name.substr(0, 3) != "gl_") 
            {
                // bind type
                attr.name.resize(nameLength);
                attr.index = glGetAttribLocation( glProgram, attr.name.c_str() );
                attr.size  = size;
                attr.type  = GLTypeTraits::Type(type);
                attributes.push_back(attr);
            }
        }
	
    }

    dirty = false;
	return SGL_OK;
}

void GLProgram::Clear()
{
    dirty = true;
    shaders.clear();
    if (uniforms) {
        delete[] uniforms;
    }
    compilationLog.clear();
}

SGL_HRESULT GLProgram::Bind() const
{
#ifndef SGL_NO_STATUS_CHECK
    if (dirty) {
		return EInvalidCall("GLProgram::Bind failed. Program is dirty.");
    }
#endif

    if (device->CurrentProgram() != this) 
    {
        glUseProgram(glProgram);
        device->SetProgram(this);
    }

	return SGL_OK;
}

void GLProgram::Unbind() const
{
    if ( device->CurrentProgram() == this )
    {
        glUseProgram(0);
        device->SetProgram(0);
    }
}

const char* SGL_DLLCALL GLProgram::CompilationLog() const
{
    if (dirty) {
		return 0;
    }

    return compilationLog.c_str();
}

void SGL_DLLCALL GLProgram::SetGeometryNumVerticesOut(unsigned int _numVerticesOut)
{
    numVerticesOut = _numVerticesOut;
    dirty          = true;
}

void SGL_DLLCALL GLProgram::SetGeometryInputType(PRIMITIVE_TYPE _inputType)
{
    inputType = _inputType;
    dirty     = true;
}

void SGL_DLLCALL GLProgram::SetGeometryOutputType(PRIMITIVE_TYPE _outputType)
{
    outputType = _outputType;
    dirty      = true;
}

// Attributes
SGL_HRESULT SGL_DLLCALL GLProgram::BindAttributeLocation(const char* name, unsigned index)
{
    glBindAttribLocation(glProgram, index, name);
#ifndef SGL_NO_STATUS_CHECK
    GLuint err = glGetError();
    if (GL_NO_ERROR != err) {
        return EInvalidCall( (std::string("Can't bind attribute: ") + name).c_str() );
    }
#endif
    dirty = true;

    return SGL_OK;
}

int SGL_DLLCALL GLProgram::AttributeLocation(const char* name) const
{
    int location = glGetAttribLocation(glProgram, name);
    return location;
}

Program::ATTRIBUTE SGL_DLLCALL GLProgram::Attribute(unsigned index) const
{
#ifndef SGL_NO_STATUS_CHECK
    if (index >= attributes.size() ) 
    {
        sglSetError(SGLERR_INVALID_CALL, "FFP program doesn't have generic attributes");
        return ATTRIBUTE();   
    }
#endif

    return attributes[index].to_ATTRIBUTE();
}

// Uniforms
UniformI* SGL_DLLCALL GLProgram::GetUniformI(const char* name)
{
    return GetUniform<int>(name);
}

Uniform2I* SGL_DLLCALL GLProgram::GetUniform2I(const char* name)
{
    return GetUniform<Vector2i>(name);
}

Uniform3I* SGL_DLLCALL GLProgram::GetUniform3I(const char* name)
{
    return GetUniform<Vector3i>(name);
}

Uniform4I* SGL_DLLCALL GLProgram::GetUniform4I(const char* name)
{
    return GetUniform<Vector4i>(name);
}

UniformF* SGL_DLLCALL GLProgram::GetUniformF(const char* name)
{
    return GetUniform<float>(name);
}

Uniform2F* SGL_DLLCALL GLProgram::GetUniform2F(const char* name)
{
    return GetUniform<Vector2f>(name);
}

Uniform3F* SGL_DLLCALL GLProgram::GetUniform3F(const char* name)
{
    return GetUniform<Vector3f>(name);
}

Uniform4F* SGL_DLLCALL GLProgram::GetUniform4F(const char* name)
{
    return GetUniform<Vector4f>(name);
}

Uniform2x2F* SGL_DLLCALL GLProgram::GetUniform2x2F(const char* name)
{
    return GetUniform<Matrix2f>(name);
}

Uniform3x3F* SGL_DLLCALL GLProgram::GetUniform3x3F(const char* name)
{
    return GetUniform<Matrix3f>(name);
}

Uniform4x4F* SGL_DLLCALL GLProgram::GetUniform4x4F(const char* name)
{
    return GetUniform<Matrix4f>(name);
}

SamplerUniform1D* SGL_DLLCALL GLProgram::GetSamplerUniform1D(const char* name)
{
    return GetSamplerUniform<Texture1D>(name);
}

SamplerUniform2D* SGL_DLLCALL GLProgram::GetSamplerUniform2D(const char* name)
{
    return GetSamplerUniform<Texture2D>(name);
}

SamplerUniform3D* SGL_DLLCALL GLProgram::GetSamplerUniform3D(const char* name)
{
    return GetSamplerUniform<Texture3D>(name);
}

SamplerUniformCube* SGL_DLLCALL GLProgram::GetSamplerUniformCube(const char* name)
{
    return GetSamplerUniform<TextureCube>(name);
}