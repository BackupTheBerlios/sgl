#include "GL/GLCommon.h"
#include "GL/GLShader.h"
#include <string>

using namespace sgl;
using namespace std;

/// bind shader type to sgl value
GLenum SHADER_TYPE_BINDINGS[] =
{
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
    GL_GEOMETRY_SHADER_EXT
};

GLShader::GLShader( GLDevice*     _device, 
                    const DESC&   desc ) :
    device(_device),
    type(desc.type)
{
    // create shader
    shader = glCreateShader( SHADER_TYPE_BINDINGS[desc.type] );

    // set data
    GLsizei size = strlen(desc.source);
    glShaderSource(shader, 1, (const GLchar**)&desc.source, &size);

    // compile
    GLint compileStatus;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    // error
    if (compileStatus != GL_TRUE)
    {
        GLsizei     errLength;
        std::string log;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errLength);
        log.resize(errLength);
        glGetShaderInfoLog(shader, errLength, &errLength, &log[0]);
        glDeleteShader(shader);
        sglSetError( SGLERR_INVALID_CALL, ("Shader compilation log: " + log).c_str() );

        throw gl_error("Shader compilation error");
    }
}

GLShader::~GLShader()
{
    glDeleteShader(shader);
}
