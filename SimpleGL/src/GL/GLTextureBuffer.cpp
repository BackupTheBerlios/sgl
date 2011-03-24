#include "GL/GLTexture1D.h"
#include <string>

using namespace sgl;

GLTexture1D::GLTexture1D(Device* _pDevice) :
    GLTexture<Texture1D>(_pDevice, GL_TEXTURE_1D, GL_TEXTURE_1D, 0),
    size(0),
    format(RGBA8),
    resizeNonPowerOfTwo(true)
{
}

GLTexture1D::~GLTexture1D()
{
    if (glTarget == GL_TEXTURE_1D && glTexture) {
        glDeleteTextures(1, &glTexture);
    }
}

void SGL_DLLCALL GLTexture1D::Clear()
{
    size = 0;

    if (glTexture) {
        glDeleteTextures(1, &glTexture);
    }
    glTexture = 0;
}

// creation
SGL_HRESULT GLTexture1D::SetImage(FORMAT _format, unsigned int _size, const void* data)
{
    // clear error flags
    glGetError();

    if ( !glTexture ) {
        glGenTextures(1, &glTexture);
    }

    // copy glTexture
    size = _size;
    format = _format;

    // image settings
	GLenum glUsage = BIND_GL_FORMAT_USAGE[format];
	GLenum glPixelType = BIND_GL_FORMAT_PIXEL_TYPE[format];
    GLenum glFormat = BIND_GL_FORMAT[format];

    // save previous state & bind texture
    texture_value oldTexValue;
    GuardedBind(oldTexValue);

    // copy image
    glTexImage1D(glTarget, 0, glFormat, size, 0, glUsage, glPixelType, data);

	// check error
    GLenum error = glGetError();
    if ( error != GL_NO_ERROR )
    {
		glDeleteTextures(1, &glTexture);
        glTexture = 0;
        return CheckGLError( "GLTexture1D::SetImage failed: ", error );
    }

    // return binding
    GuardedUnbind(oldTexValue);
    return SGL_OK;
}

void GLTexture1D::SetResizeNonPowerOfTwoHint(bool toggle)
{
    resizeNonPowerOfTwo = toggle;
}

// work
GLenum SGL_DLLCALL GLTexture1D::GLFormat() const
{
    return BIND_GL_FORMAT[format];
}
