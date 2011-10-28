#include "GL/GLCommon.h"
#include "GL/GLDevice.h"
#include "GL/GLShader.h"
#include <cstring>

namespace {

    using namespace sgl;

    /// bind shader type to sgl value
    GLenum SHADER_TYPE_BINDINGS[] =
    {
        GL_VERTEX_SHADER,
        GL_FRAGMENT_SHADER
    #ifndef SIMPLE_GL_ES
        , GL_GEOMETRY_SHADER
    #endif
    };

} // anonymous namespace

namespace sgl {

GLShader::GLShader( GLDevice*	device_, 
                    const DESC& desc ) :
    device(device_),
    type(desc.type)
{
    // create shader
    shader = glCreateShader( SHADER_TYPE_BINDINGS[desc.type] );

    // set data
    GLsizei size = strlen(desc.source);
    glShaderSource(shader, 1, (const char**)&desc.source, &size);

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

        throw gl_error("Shader compilation error", SGLERR_INVALID_CALL);
    }
}

GLShader::~GLShader()
{
	if ( device->Valid() ) {
		glDeleteShader(shader);
	}
}

} // namespace sgl
