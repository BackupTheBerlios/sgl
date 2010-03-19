#ifndef SIMPLE_GL_GL_COMMON_H
#define SIMPLE_GL_GL_COMMON_H

#ifdef WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/glew.h>
#include <stdexcept>
#include "../Device.h"

namespace sgl {

extern const GLenum     BIND_PRIMITIVE_TYPE[__NUMBER_OF_PRIMITIVE_TYPES__];
extern const GLenum     BIND_GL_INDEX_TYPE[IndexBuffer::__NUM_OF_INDEX_TYPES__];
extern const GLsizei    BIND_GL_INDEX_SIZE[IndexBuffer::__NUM_OF_INDEX_TYPES__];

// Misc
SGL_HRESULT CheckGLError(const std::string& preMsg, GLenum error);
SGL_HRESULT CheckGLUError(const std::string& preMsg, GLenum error);
SGL_HRESULT CheckGLFramebufferStatus(const std::string& preMsg, GLenum status);

class gl_error :
    public std::runtime_error
{
public:
    explicit gl_error(const std::string& msg) :
        std::runtime_error(msg)
    {}

	~gl_error() throw() {}
};

} // namesapce sgl

#endif // SIMPLE_GL_GL_COMMON_H
