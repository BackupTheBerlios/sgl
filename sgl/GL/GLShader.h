#ifndef SIMPLE_GL_GL_SHADER_H
#define SIMPLE_GL_GL_SHADER_H

#include "GLForward.h"
#include "../Shader.h"
#include <string>

namespace sgl {

/* Wraps GLfunctions to work with shaders */
template<DEVICE_VERSION DeviceVersion>
class GLShader :
    public ResourceImpl<Shader>
{
public:
    GLShader( GLDevice<DeviceVersion>*  device_, 
              const DESC&               desc );
    ~GLShader();

    // Override Shader
    TYPE        SGL_DLLCALL Type() const            { return type; }
    const char* SGL_DLLCALL Source() const          { return source.c_str(); }
    const char* SGL_DLLCALL CompilationLog() const  { return compilationLog.c_str(); }

    /** Get OpenGL shader handle */
    unsigned SGL_DLLCALL Handle() const { return shader; }
 
protected:
    GLDevice<DeviceVersion>*    device;

    // data
    TYPE            type;
    unsigned        shader;
    std::string     source;
    std::string     compilationLog;
};

/** Make shader */
template<DEVICE_VERSION DeviceVersion>
sgl::Shader* sglCreateShader(GLDevice<DeviceVersion>* device, const Shader::DESC& desc);

} // namespace sgl

#endif // SIMPLE_GL_GL_SHADER_H
