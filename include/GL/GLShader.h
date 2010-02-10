#ifndef SIMPLE_GL_GL_SHADER_H
#define SIMPLE_GL_GL_SHADER_H

#include "GLDevice.h"

namespace sgl {

/* Wraps GLfunctions to work with shaders */
class GLShader :
    public ResourceImpl<Shader>
{
friend class GLDevice;
friend class GLProgram;
public:
    // Override Shader
    TYPE        SGL_DLLCALL Type() const            { return type; }
    const char* SGL_DLLCALL Source() const          { return source.c_str(); }
    const char* SGL_DLLCALL CompilationLog() const  { return compilationLog.c_str(); }
 
    ~GLShader();

protected:
    GLShader( GLDevice*     _device, 
              const DESC&   desc );

protected:
    ref_ptr<GLDevice>    device;

    // data
    TYPE                type;
    GLuint              shader;
    std::string         source;
    std::string         compilationLog;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_SHADER_H
