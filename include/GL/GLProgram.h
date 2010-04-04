#ifndef SIMPLE_GL_GL_PROGRAM_H
#define SIMPLE_GL_GL_PROGRAM_H

#include "GLCommon.h"
#include "GLShader.h"
#include "../Program.h"

namespace sgl {

template<typename T>
class GLUniform;

template<typename T>
class GLSamplerUniform;

/* Wraps gl function for working with shader programs */
template<DEVICE_VERSION DeviceVersion> 
class GLProgram :
    public ResourceImpl<Program>
{
private:
    typedef scoped_ptr<AbstractUniform>         uniform_ptr;

    typedef GLShader<DeviceVersion>             shader_type;
    typedef ref_ptr<shader_type>                shader_ptr;
    typedef std::vector<shader_ptr>             shader_vector;
    
    struct attribute
    {
        unsigned            index;
        unsigned            size;
        std::string         name;
        sgl::SCALAR_TYPE    type;

        // convert to ATTRIBUTE
        ATTRIBUTE to_ATTRIBUTE() const
        {
            ATTRIBUTE attr;
            attr.index = index;
            attr.size  = size;
            attr.name  = name.c_str();
            attr.type  = type;
            return attr;
        }
    };
    typedef std::vector<attribute>              attribute_vector;


private:
    AbstractUniform* CreateUniform( GLProgram*  program,
                                    const char* name,
                                    GLuint      glIndex,
                                    GLuint      glLocation,
                                    GLenum      glUniformType,
                                    size_t      size );

public:
    GLProgram(GLDevice<DeviceVersion>* deviceState);
    ~GLProgram();

    // Override Program
    SGL_HRESULT     SGL_DLLCALL AddShader(Shader* shader);
    bool            SGL_DLLCALL RemoveShader(Shader* shader);

    bool            SGL_DLLCALL IsDirty() const { return dirty; } 
    SGL_HRESULT     SGL_DLLCALL Dirty(bool force = false);
    const char*     SGL_DLLCALL CompilationLog() const;
    void            SGL_DLLCALL Clear();

    SGL_HRESULT     SGL_DLLCALL Bind() const;
    void            SGL_DLLCALL Unbind() const;

    // Attributes
    SGL_HRESULT     SGL_DLLCALL BindAttributeLocation(const char* name, unsigned index);
    int             SGL_DLLCALL AttributeLocation(const char* name) const;
    unsigned        SGL_DLLCALL NumAttributes() const { return attributes.size(); }
    ATTRIBUTE       SGL_DLLCALL Attribute(unsigned index) const;

    // Geometry shaders
    void            SGL_DLLCALL SetGeometryNumVerticesOut(unsigned int maxNumVertices);
    void            SGL_DLLCALL SetGeometryInputType(PRIMITIVE_TYPE inputType);
    void            SGL_DLLCALL SetGeometryOutputType(PRIMITIVE_TYPE outputType);

    unsigned int    SGL_DLLCALL GeometryNumVerticesOut() const { return numVerticesOut; }
    PRIMITIVE_TYPE  SGL_DLLCALL GeometryInputType() const { return inputType; }
    PRIMITIVE_TYPE  SGL_DLLCALL GeometryOutputType() const { return outputType; }

    // Uniforms
    AbstractUniform* SGL_DLLCALL GetUniform(const char* name);
    
    template<typename T>
    GLUniform<T>* GetUniform(const char* name)
    {
        return dynamic_cast< GLUniform<T>* >( GetUniform(name) );
    }

    template<typename T>
    GLSamplerUniform<T>* GetSamplerUniform(const char* name)
    {
        return dynamic_cast< GLSamplerUniform<T>* >( GetUniform(name) );
    }

    /* Create int uniform */
    UniformI*  SGL_DLLCALL GetUniformI(const char* name);
    Uniform2I* SGL_DLLCALL GetUniform2I(const char* name);
    Uniform3I* SGL_DLLCALL GetUniform3I(const char* name);
    Uniform4I* SGL_DLLCALL GetUniform4I(const char* name);

    /* Create float uniform */
    UniformF*  SGL_DLLCALL GetUniformF(const char* name);
    Uniform2F* SGL_DLLCALL GetUniform2F(const char* name);
    Uniform3F* SGL_DLLCALL GetUniform3F(const char* name);
    Uniform4F* SGL_DLLCALL GetUniform4F(const char* name);

    /* Create matrix uniform */
    Uniform2x2F* SGL_DLLCALL GetUniform2x2F(const char* name);
    Uniform3x3F* SGL_DLLCALL GetUniform3x3F(const char* name);
    Uniform4x4F* SGL_DLLCALL GetUniform4x4F(const char* name);

    /* Create texture uniforms */
    SamplerUniform1D*   SGL_DLLCALL GetSamplerUniform1D(const char* name);
    SamplerUniform2D*   SGL_DLLCALL GetSamplerUniform2D(const char* name);
    SamplerUniform3D*   SGL_DLLCALL GetSamplerUniform3D(const char* name);
    SamplerUniformCube* SGL_DLLCALL GetSamplerUniformCube(const char* name);

    /** Get OpenGL program handle */
    GLuint SGL_DLLCALL Handle() const { return glProgram; }

private:
    GLDevice<DeviceVersion>*    device;
    shader_vector               shaders;
    attribute_vector            attributes;
    uniform_ptr*                uniforms;

    // geometry shaders
    unsigned int        numActiveUniforms;
    unsigned int        numVerticesOut;
    PRIMITIVE_TYPE      inputType;
    PRIMITIVE_TYPE      outputType;

    // data
    GLuint              glProgram;
    bool                dirty;

    // log
    std::string         compilationLog;
};

/** Try to make shader program */
template<DEVICE_VERSION DeviceVersion>
sgl::Program* sglCreateProgram(GLDevice<DeviceVersion>* device);

} // namespaec sgl

#endif // SIMPLE_GL_GL_PROGRAM_H
