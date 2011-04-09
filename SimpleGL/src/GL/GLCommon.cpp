#include "GL/GLCommon.h"
#include <string>

namespace sgl {

SGL_HRESULT CheckGLError(const std::string& preMsg, GLenum error)
{
    switch(error)
    {
    case GL_NO_ERROR:
        return SGL_OK;

    case GL_INVALID_ENUM:
    case GL_INVALID_VALUE:
        return EInvalidCall( (preMsg + "Invalid value").c_str() );

    case GL_INVALID_OPERATION:
        return EInvalidCall( (preMsg + "Invalid opertation").c_str() );

    case GL_OUT_OF_MEMORY:
        return EOutOfMemory( (preMsg + "Out of memory").c_str() );

    default:
        return EUnknown( (preMsg + "Unknown error").c_str() );
    }
}

SGL_HRESULT CheckGLUError(const std::string& preMsg, GLenum error)
{
    switch(error)
    {
    case GL_NO_ERROR:
        return SGL_OK;

    case GLU_INVALID_ENUM:
    case GLU_INVALID_VALUE:
        return EInvalidCall( (preMsg + "Invalid call").c_str() );

    case GLU_OUT_OF_MEMORY:
        return EOutOfMemory( (preMsg + "Out of memory").c_str() );

    default:
        return EUnknown( (preMsg + "Unknown error").c_str() );
    }
}

SGL_HRESULT CheckGLFramebufferStatus(const std::string& preMsg, GLenum status)
{
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        return SGL_OK;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        return EInvalidCall( (preMsg + "Incomplete attachment").c_str() );

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        return EInvalidCall( (preMsg + "Missing attachment").c_str() );

    case GL_FRAMEBUFFER_UNSUPPORTED:
        return EInvalidCall( (preMsg + "Unsupported").c_str() );

    default:
        return EUnknown( (preMsg + "unknown error").c_str() );
    }
}

// bind sgl primitive types to gl ones
const GLenum BIND_PRIMITIVE_TYPE[__NUMBER_OF_PRIMITIVE_TYPES__] =
{
    GL_POINTS,
    GL_LINES,
    GL_LINE_STRIP,
    GL_LINE_LOOP,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN,
    GL_QUADS,
    GL_QUAD_STRIP,
	GL_POLYGON
};

const GLenum BIND_GL_INDEX_TYPE[] = 
{
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_INT
};

const GLsizei BIND_GL_INDEX_SIZE[] = 
{
    1,
    2,
    4
};

} // namesapce sgl