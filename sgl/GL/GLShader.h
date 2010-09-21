#ifndef SIMPLE_GL_GL_SHADER_H
#define SIMPLE_GL_GL_SHADER_H

#include "GLForward.h"
#include "../Shader.h"
#include <string>

namespace sgl {

/* Wraps GLfunctions to work with shaders */
class GLShader :
    public ResourceImpl<Shader>
{
public:
    GLShader( GLDevice*		device, 
              const DESC&   desc );
    ~GLShader();

    // Override Shader
    TYPE        SGL_DLLCALL Type() const            { return type; }
    const char* SGL_DLLCALL Source() const          { return source.c_str(); }
    const char* SGL_DLLCALL CompilationLog() const  { return compilationLog.c_str(); }

    /** Get OpenGL shader handle */
    unsigned SGL_DLLCALL Handle() const { return shader; }
 
protected:
    GLDevice*    device;

    // data
    TYPE            type;
    unsigned        shader;
    std::string     source;
    std::string     compilationLog;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_SHADER_H
