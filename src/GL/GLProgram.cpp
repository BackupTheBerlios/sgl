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

} // anonymous namespace

namespace sgl {

// program
template<DEVICE_VERSION DeviceVersion> 
GLProgram<DeviceVersion>::GLProgram(GLDevice<DeviceVersion>* device_) :
    device(device_),
    uniforms(0),
    numVerticesOut(3),
    inputType(TRIANGLES),
    outputType(TRIANGLE_STRIP),
    glProgram(0),
    dirty(true)
{
    glProgram = glCreateProgram();
}

template<DEVICE_VERSION DeviceVersion> 
GLProgram<DeviceVersion>::~GLProgram()
{
    glDeleteProgram(glProgram);
    if (uniforms) {
        delete[] uniforms;
    }
}

// shaders
template<DEVICE_VERSION DeviceVersion> 
SGL_HRESULT GLProgram<DeviceVersion>::AddShader(Shader* shader)
{
#ifndef SGL_NO_STATUS_CHECK
    if (!shader) {
		return EInvalidCall("GLProgram::AddShader failed. Shader is NULL");
    }
#endif
    shaders.push_back( ref_ptr<shader_type>( static_cast<shader_type*>(shader) ) );
    dirty = true;

    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion> 
bool GLProgram<DeviceVersion>::RemoveShader(Shader* shader)
{
    typename shader_vector::iterator iter = std::find( shaders.begin(), shaders.end(), ref_ptr<Shader>(shader) );
    if ( iter != shaders.end() ) 
    {
        std::swap(*iter, shaders.back() );
        shaders.pop_back();
        dirty = true;
        return true;
    }

    return false;
}

template<DEVICE_VERSION DeviceVersion> 
AbstractUniform* GLProgram<DeviceVersion>::CreateUniform( GLProgram*   program,
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

    typedef GLSamplerUniform<Texture1D>     sampler_uniform_1d;
    typedef GLSamplerUniform<Texture2D>     sampler_uniform_2d;
    typedef GLSamplerUniform<Texture3D>     sampler_uniform_3d;
    typedef GLSamplerUniform<TextureCube>   sampler_uniform_cube;

    switch(glUniformType)
    {
    case GL_FLOAT:
        return new uniform1f( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_FLOAT_VEC2:
        return new uniform2f( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_FLOAT_VEC3:
        return new uniform3f( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_FLOAT_VEC4:
        return new uniform4f( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_INT:
        return new uniform1i( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_INT_VEC2:
        return new uniform2i( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_INT_VEC3:
        return new uniform3i( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_INT_VEC4:
        return new uniform4i( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_BOOL:
        return new uniform1i( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_BOOL_VEC2:
        return new uniform2i( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_BOOL_VEC3:
        return new uniform3i( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_BOOL_VEC4:
        return new uniform4i( device,
                              program,
                              name,
                              glProgram,
                              glIndex,
                              glLocation,
                              size );

    case GL_FLOAT_MAT2:
        return new uniform2x2f( device,
                                program,
                                name,
                                glProgram,
                                glIndex,
                                glLocation,
                                size );

    case GL_FLOAT_MAT3:
        return new uniform3x3f( device,
                                program,
                                name,
                                glProgram,
                                glIndex,
                                glLocation,
                                size );

    case GL_FLOAT_MAT4:
        return new uniform4x4f( device,
                                program,
                                name,
                                glProgram,
                                glIndex,
                                glLocation,
                                size );

    case GL_SAMPLER_1D_SHADOW:
    case GL_INT_SAMPLER_1D_EXT:
    case GL_SAMPLER_1D:
    {
        return new sampler_uniform_1d( device,
                                       program,
                                       name,
                                       glProgram,
                                       glIndex,
                                       glLocation );
    }

    case GL_SAMPLER_2D_SHADOW:
    case GL_INT_SAMPLER_2D_EXT:
    case GL_SAMPLER_2D:
    {
        return new sampler_uniform_2d( device,
                                       program,
                                       name,
                                       glProgram,
                                       glIndex,
                                       glLocation );
    }

    case GL_INT_SAMPLER_3D_EXT:
    case GL_SAMPLER_3D:
    {
        return new sampler_uniform_3d( device,
                                       program,
                                       name,
                                       glProgram,
                                       glIndex,
                                       glLocation );
    }

    case GL_SAMPLER_CUBE:
    {
        return new sampler_uniform_cube( device,
                                         program,
                                         name,
                                         glProgram,
                                         glIndex,
                                         glLocation );
    }

    default:
        return 0;
    }

    return 0;
}

// Work
template<DEVICE_VERSION DeviceVersion> 
SGL_HRESULT GLProgram<DeviceVersion>::Dirty(bool force)
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
            programShaders[i] = shaders[i]->Handle();
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

template<DEVICE_VERSION DeviceVersion> 
void GLProgram<DeviceVersion>::Clear()
{
    dirty = true;
    shaders.clear();
    if (uniforms) {
        delete[] uniforms;
    }
    compilationLog.clear();
}

template<DEVICE_VERSION DeviceVersion> 
SGL_HRESULT GLProgram<DeviceVersion>::Bind() const
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

template<DEVICE_VERSION DeviceVersion> 
void GLProgram<DeviceVersion>::Unbind() const
{
    if (device->CurrentProgram() == this) 
    {
        glUseProgram(0);
        device->SetProgram(0);
    }
}

template<DEVICE_VERSION DeviceVersion> 
const char* GLProgram<DeviceVersion>::CompilationLog() const
{
    if (dirty) {
		return 0;
    }

    return compilationLog.c_str();
}

template<DEVICE_VERSION DeviceVersion> 
void GLProgram<DeviceVersion>::SetGeometryNumVerticesOut(unsigned int _numVerticesOut)
{
    numVerticesOut = _numVerticesOut;
    dirty          = true;
}

template<DEVICE_VERSION DeviceVersion> 
void GLProgram<DeviceVersion>::SetGeometryInputType(PRIMITIVE_TYPE _inputType)
{
    inputType = _inputType;
    dirty     = true;
}

template<DEVICE_VERSION DeviceVersion> 
void GLProgram<DeviceVersion>::SetGeometryOutputType(PRIMITIVE_TYPE _outputType)
{
    outputType = _outputType;
    dirty      = true;
}

// Attributes
template<DEVICE_VERSION DeviceVersion> 
SGL_HRESULT GLProgram<DeviceVersion>::BindAttributeLocation(const char* name, unsigned index)
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

template<DEVICE_VERSION DeviceVersion> 
int GLProgram<DeviceVersion>::AttributeLocation(const char* name) const
{
    int location = glGetAttribLocation(glProgram, name);
    return location;
}

template<DEVICE_VERSION DeviceVersion> 
Program::ATTRIBUTE GLProgram<DeviceVersion>::Attribute(unsigned index) const
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
template<DEVICE_VERSION DeviceVersion>
AbstractUniform* GLProgram<DeviceVersion>::GetUniform(const char* name) const
{
    if (dirty)
    {
        sglSetError(SGLERR_INVALID_CALL, "GLProgram::GetUniform failed. Program is Dirty.");
        return 0;
    }

    GLint uniform = glGetUniformLocation(glProgram, name);
    if (uniform == -1) {
        return 0;
    }

    // search for uniform in the uniform array
    for(size_t i = 0; i<numActiveUniforms; ++i)
    {
        // hack
        GLint location = -1;
        switch ( uniforms[i]->Type() )
        {
        case AbstractUniform::INT:
            location = static_cast< GLUniform<int>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::VEC2I:
            location = static_cast< GLUniform<math::Vector2i>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::VEC3I:
            location = static_cast< GLUniform<math::Vector3i>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::VEC4I:
            location = static_cast< GLUniform<math::Vector4i>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::FLOAT:
            location = static_cast< GLUniform<float>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::VEC2F:
            location = static_cast< GLUniform<math::Vector2f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::VEC3F:
            location = static_cast< GLUniform<math::Vector3f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::VEC4F:
            location = static_cast< GLUniform<math::Vector4f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::MAT2x2F:
            location = static_cast< GLUniform<math::Matrix2x2f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::MAT2x3F:
            location = static_cast< GLUniform<math::Matrix2x3f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::MAT2x4F:
            location = static_cast< GLUniform<math::Matrix2x4f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::MAT3x2F:
            location = static_cast< GLUniform<math::Matrix3x2f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::MAT3x3F:
            location = static_cast< GLUniform<math::Matrix3x3f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::MAT3x4F:
            location = static_cast< GLUniform<math::Matrix3x4f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::MAT4x2F:
            location = static_cast< GLUniform<math::Matrix4x2f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::MAT4x3F:
            location = static_cast< GLUniform<math::Matrix4x3f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::MAT4x4F:
            location = static_cast< GLUniform<math::Matrix4x4f>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::SAMPLER_1D:
            location = static_cast< GLSamplerUniform<Texture1D>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::SAMPLER_2D:
            location = static_cast< GLSamplerUniform<Texture2D>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::SAMPLER_3D:
            location = static_cast< GLSamplerUniform<Texture3D>& >(*uniforms[i]).Location();
            break;
        case AbstractUniform::SAMPLER_CUBE:
            location = static_cast< GLSamplerUniform<TextureCube>& >(*uniforms[i]).Location();
            break;
        default:
            assert(!"Can't get here");
            break;
        }

        if (uniform == location) {
            return uniforms[i].get();
        }
    }

    return 0;
}

template<DEVICE_VERSION DeviceVersion> 
UniformI* GLProgram<DeviceVersion>::GetUniformI(const char* name) const
{
    return GetUniform<int>(name);
}

template<DEVICE_VERSION DeviceVersion> 
Uniform2I* SGL_DLLCALL GLProgram<DeviceVersion>::GetUniform2I(const char* name) const
{
    return GetUniform<Vector2i>(name);
}

template<DEVICE_VERSION DeviceVersion> 
Uniform3I* SGL_DLLCALL GLProgram<DeviceVersion>::GetUniform3I(const char* name) const
{
    return GetUniform<Vector3i>(name);
}

template<DEVICE_VERSION DeviceVersion> 
Uniform4I* SGL_DLLCALL GLProgram<DeviceVersion>::GetUniform4I(const char* name) const
{
    return GetUniform<Vector4i>(name);
}

template<DEVICE_VERSION DeviceVersion> 
UniformF* SGL_DLLCALL GLProgram<DeviceVersion>::GetUniformF(const char* name) const
{
    return GetUniform<float>(name);
}

template<DEVICE_VERSION DeviceVersion> 
Uniform2F* SGL_DLLCALL GLProgram<DeviceVersion>::GetUniform2F(const char* name) const
{
    return GetUniform<Vector2f>(name);
}

template<DEVICE_VERSION DeviceVersion> 
Uniform3F* SGL_DLLCALL GLProgram<DeviceVersion>::GetUniform3F(const char* name) const
{
    return GetUniform<Vector3f>(name);
}

template<DEVICE_VERSION DeviceVersion> 
Uniform4F* SGL_DLLCALL GLProgram<DeviceVersion>::GetUniform4F(const char* name) const
{
    return GetUniform<Vector4f>(name);
}

template<DEVICE_VERSION DeviceVersion> 
Uniform2x2F* SGL_DLLCALL GLProgram<DeviceVersion>::GetUniform2x2F(const char* name) const
{
    return GetUniform<Matrix2f>(name);
}

template<DEVICE_VERSION DeviceVersion> 
Uniform3x3F* SGL_DLLCALL GLProgram<DeviceVersion>::GetUniform3x3F(const char* name) const
{
    return GetUniform<Matrix3f>(name);
}

template<DEVICE_VERSION DeviceVersion> 
Uniform4x4F* SGL_DLLCALL GLProgram<DeviceVersion>::GetUniform4x4F(const char* name) const
{
    return GetUniform<Matrix4f>(name);
}

template<DEVICE_VERSION DeviceVersion> 
SamplerUniform1D* SGL_DLLCALL GLProgram<DeviceVersion>::GetSamplerUniform1D(const char* name) const
{
    return GetSamplerUniform<Texture1D>(name);
}

template<DEVICE_VERSION DeviceVersion> 
SamplerUniform2D* SGL_DLLCALL GLProgram<DeviceVersion>::GetSamplerUniform2D(const char* name) const
{
    return GetSamplerUniform<Texture2D>(name);
}

template<DEVICE_VERSION DeviceVersion> 
SamplerUniform3D* SGL_DLLCALL GLProgram<DeviceVersion>::GetSamplerUniform3D(const char* name) const
{
    return GetSamplerUniform<Texture3D>(name);
}

template<DEVICE_VERSION DeviceVersion> 
SamplerUniformCube* SGL_DLLCALL GLProgram<DeviceVersion>::GetSamplerUniformCube(const char* name) const
{
    return GetSamplerUniform<TextureCube>(name);
}

template<DEVICE_VERSION DeviceVersion>
sgl::Program* sglCreateProgram(GLDevice<DeviceVersion>* device)
{
    if ( device_traits<DeviceVersion>::support_programmable_pipeline() ) {
        return new GLProgram<DeviceVersion>(device);
    }

    throw gl_error("Device profile doesn't programmable pipeline", SGLERR_UNSUPPORTED);
}

// explicit template instantiation
template class GLProgram<DV_OPENGL_2_0>;
template class GLProgram<DV_OPENGL_2_1>;
template class GLProgram<DV_OPENGL_3_0>;
template class GLProgram<DV_OPENGL_3_1>;
template class GLProgram<DV_OPENGL_3_2>;

template sgl::Program* sglCreateProgram<DV_OPENGL_1_3>(GLDevice<DV_OPENGL_1_3>*);
template sgl::Program* sglCreateProgram<DV_OPENGL_1_4>(GLDevice<DV_OPENGL_1_4>*);
template sgl::Program* sglCreateProgram<DV_OPENGL_1_5>(GLDevice<DV_OPENGL_1_5>*);
template sgl::Program* sglCreateProgram<DV_OPENGL_2_0>(GLDevice<DV_OPENGL_2_0>*);
template sgl::Program* sglCreateProgram<DV_OPENGL_2_1>(GLDevice<DV_OPENGL_2_1>*);
template sgl::Program* sglCreateProgram<DV_OPENGL_3_0>(GLDevice<DV_OPENGL_3_0>*);
template sgl::Program* sglCreateProgram<DV_OPENGL_3_1>(GLDevice<DV_OPENGL_3_1>*);
template sgl::Program* sglCreateProgram<DV_OPENGL_3_2>(GLDevice<DV_OPENGL_3_2>*);

} // namespace sgl
