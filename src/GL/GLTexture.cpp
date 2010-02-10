#include "GL/GLTexture.h"

using namespace sgl;

/* bind glTexture format to gl one */
const GLenum sgl::BIND_GL_FORMAT[] =
{
    0,
    GL_RGBA16,
    GL_RGB16,
    GL_ALPHA16,
    GL_RGBA8,
    GL_RGB8,
    GL_ALPHA8,
    GL_DEPTH_COMPONENT32,
    GL_DEPTH_COMPONENT24,
    GL_DEPTH_COMPONENT16,
    GL_DEPTH24_STENCIL8,
    GL_DEPTH_COMPONENT32F,
    GL_DEPTH32F_STENCIL8,
    GL_RGBA32F_ARB,
    GL_RGB32F_ARB,
#ifdef GL_VERSION_3_0
    GL_RG32F,
    GL_R32F,
#else
    0,
    0,
#endif
    GL_ALPHA32F_ARB,
    GL_RGBA16F_ARB,
    GL_RGB16F_ARB,
#ifdef GL_VERSION_3_0
    GL_RG16F,
    GL_R16F,
#else
    0,
    0,
#endif
    GL_ALPHA16F_ARB,
    GL_RGBA32UI_EXT,
    GL_RGB32UI_EXT,
    GL_ALPHA32UI_EXT,
    GL_RGBA16UI_EXT,
    GL_RGB16UI_EXT,
    GL_ALPHA16UI_EXT,
    GL_RGBA8UI_EXT,
    GL_RGB8UI_EXT,
    GL_ALPHA8UI_EXT,
    GL_RGBA32I_EXT,
    GL_RGB32I_EXT,
    GL_ALPHA32I_EXT,
    GL_RGBA16I_EXT,
    GL_RGB16I_EXT,
    GL_ALPHA16I_EXT,
    GL_RGBA8I_EXT,
    GL_RGB8I_EXT,
    GL_ALPHA8I_EXT,
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
    GL_COMPRESSED_RED_RGTC1_EXT,
    GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,
    GL_COMPRESSED_RED_GREEN_RGTC2_EXT,
    GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT
};

/* Bind sgl format usage gl one */
const GLenum sgl::BIND_GL_FORMAT_USAGE[] =
{
    0,
    GL_RGBA,
    GL_RGB,
    GL_ALPHA,
    GL_RGBA,
    GL_RGB,
    GL_ALPHA,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_RGBA,
    GL_RGB,
#ifdef GL_VERSION_3_0
    GL_RG,
    GL_R,
#else
    0,
    0,
#endif
    GL_ALPHA,
    GL_RGBA,
    GL_RGB,
#ifdef GL_VERSION_3_0
    GL_RG,
    GL_R,
#else
    0,
    0,
#endif
    GL_ALPHA,
    GL_RGBA_INTEGER_EXT,
    GL_RGB_INTEGER_EXT,
    GL_ALPHA_INTEGER_EXT,
    GL_RGBA_INTEGER_EXT,
    GL_RGB_INTEGER_EXT,
    GL_ALPHA_INTEGER_EXT,
    GL_RGBA_INTEGER_EXT,
    GL_RGB_INTEGER_EXT,
    GL_ALPHA_INTEGER_EXT,
    GL_RGBA_INTEGER_EXT,
    GL_RGB_INTEGER_EXT,
    GL_ALPHA_INTEGER_EXT,
    GL_RGBA_INTEGER_EXT,
    GL_RGB_INTEGER_EXT,
    GL_ALPHA_INTEGER_EXT,
    GL_RGBA_INTEGER_EXT,
    GL_RGB_INTEGER_EXT,
    GL_ALPHA_INTEGER_EXT,
    // dummy for compressed
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

/* bind glTexture format to gl one */
const GLenum sgl::BIND_GL_FORMAT_PIXEL_TYPE[] =
{
    0,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_SHORT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_HALF_FLOAT_ARB,
    GL_HALF_FLOAT_ARB,
    GL_HALF_FLOAT_ARB,
    GL_HALF_FLOAT_ARB,
    GL_HALF_FLOAT_ARB,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_INT,
    GL_INT,
    GL_INT,
    GL_SHORT,
    GL_SHORT,
    GL_SHORT,
    GL_BYTE,
    GL_BYTE,
    GL_BYTE,
    // dummy for compressed
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

const GLenum sgl::BIND_TEXTURE_FILTER[] =
{
    GL_NEAREST,
    GL_LINEAR
};

const GLenum sgl::BIND_TEXTURE_MIN_FILTER[] =
{
    GL_NEAREST_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_LINEAR
};

const GLenum sgl::BIND_TEXTURE_CLAMP[] =
{
    GL_REPEAT,
    GL_CLAMP,
    GL_CLAMP_TO_EDGE
};

/* bind glTexture environment mode to gl one */
const GLenum sgl::BIND_GL_TEXTURE_ENV_MODE[] =
{
    GL_DECAL,
    GL_REPLACE,
    GL_MODULATE,
    GL_ADD
};

#ifdef SIMPLE_GL_USE_SDL_IMAGE

Texture::FORMAT sgl::FindTextureFormat(const SDL_PixelFormat& format)
{
    switch( format.BitsPerPixel )
    {
    case 8:
        return Texture::ALPHA8;

    case 16:
        return Texture::ALPHA16;

    case 24:
        return Texture::RGB8;

    case 32:
        return Texture::RGBA8;
    }

    return Texture2D::ALPHA16;
}

#endif // SIMPLE_GL_USE_SDL_IMAGE
