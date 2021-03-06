#ifndef SIMPLE_GL_GL_COMMON_H
#define SIMPLE_GL_GL_COMMON_H

#include "../Config.h"
#ifdef WIN32
#   ifndef NOMINMAX
#   define NOMINMAX
#   endif // disable mingw warning
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif // WIN32
#ifdef __ANDROID__
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#   include <GLES/gl.h>
#   include <GLES/glext.h>
#else
#   include <GL/glew.h>
#endif
#include <stdexcept>
#include "../Device.h"

namespace sgl {

extern const GLenum     BIND_PRIMITIVE_TYPE[__NUMBER_OF_PRIMITIVE_TYPES__];
extern const GLenum     BIND_GL_INDEX_TYPE[IndexBuffer::__NUM_OF_INDEX_TYPES__];
extern const GLsizei    BIND_GL_INDEX_SIZE[IndexBuffer::__NUM_OF_INDEX_TYPES__];

// Misc
SGL_HRESULT CheckGLError(const std::string& preMsg, GLenum error);
SGL_HRESULT CheckGLFramebufferStatus(const std::string& preMsg, GLenum status);

#ifndef SIMPLE_GL_ES
SGL_HRESULT CheckGLUError(const std::string& preMsg, GLenum error);
#endif

class gl_error :
    public std::runtime_error
{
public:
    explicit gl_error(const std::string& msg,
                      SGL_HRESULT        res_ = SGLERR_UNKNOWN) :
        std::runtime_error(msg),
        res(res_)
    {}

    SGL_HRESULT result() const { return res; }

	~gl_error() throw() {}

public:
    SGL_HRESULT res;
};

} // namesapce sgl

#endif // SIMPLE_GL_GL_COMMON_H
