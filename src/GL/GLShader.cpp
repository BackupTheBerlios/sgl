#include "GL/GLCommon.h"
#include "GL/GLDevice.h"
#include "GL/GLShader.h"

namespace {

    using namespace sgl;

    /// bind shader type to sgl value
    GLenum SHADER_TYPE_BINDINGS[] =
    {
        GL_VERTEX_SHADER,
        GL_FRAGMENT_SHADER,
        GL_GEOMETRY_SHADER_EXT
    };

} // anonymous namespace

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
GLShader<DeviceVersion>::GLShader( GLDevice<DeviceVersion>*  device_, 
                                   const DESC&               desc ) :
    device(device_),
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

        throw gl_error("Shader compilation error", SGLERR_INVALID_CALL);
    }
}

template<DEVICE_VERSION DeviceVersion>
GLShader<DeviceVersion>::~GLShader()
{
    glDeleteShader(shader);
}

/** Make shader */
template<DEVICE_VERSION DeviceVersion>
sgl::Shader* sglCreateShader(GLDevice<DeviceVersion>* device, const Shader::DESC& desc)
{
    if (!GLEW_VERSION_2_0) {
        throw gl_error("Shaders are not supported", SGLERR_UNSUPPORTED);
    }

    if (desc.type == Shader::GEOMETRY && !GL_ARB_geometry_shader4) {
        throw gl_error("Geometry shaders are not supported", SGLERR_UNSUPPORTED);
    }

    return new GLShader<DeviceVersion>(device, desc);
}

// explicit template instantiation
template class GLShader<DV_OPENGL_2_0>;
template class GLShader<DV_OPENGL_2_1>;
template class GLShader<DV_OPENGL_3_0>;
template class GLShader<DV_OPENGL_3_1>;
template class GLShader<DV_OPENGL_3_2>;

template sgl::Shader* sglCreateShader<DV_OPENGL_1_3>(GLDevice<DV_OPENGL_1_3>*, const Shader::DESC&);
template sgl::Shader* sglCreateShader<DV_OPENGL_1_4>(GLDevice<DV_OPENGL_1_4>*, const Shader::DESC&);
template sgl::Shader* sglCreateShader<DV_OPENGL_1_5>(GLDevice<DV_OPENGL_1_5>*, const Shader::DESC&);
template sgl::Shader* sglCreateShader<DV_OPENGL_2_0>(GLDevice<DV_OPENGL_2_0>*, const Shader::DESC&);
template sgl::Shader* sglCreateShader<DV_OPENGL_2_1>(GLDevice<DV_OPENGL_2_1>*, const Shader::DESC&);
template sgl::Shader* sglCreateShader<DV_OPENGL_3_0>(GLDevice<DV_OPENGL_3_0>*, const Shader::DESC&);
template sgl::Shader* sglCreateShader<DV_OPENGL_3_1>(GLDevice<DV_OPENGL_3_1>*, const Shader::DESC&);
template sgl::Shader* sglCreateShader<DV_OPENGL_3_2>(GLDevice<DV_OPENGL_3_2>*, const Shader::DESC&);

} // namespace sgl
