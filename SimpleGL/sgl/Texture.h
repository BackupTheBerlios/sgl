#ifndef SIMPLE_GL_TEXTURE_H
#define SIMPLE_GL_TEXTURE_H

#include "Resource.h"
#include "SamplerState.h"

namespace sgl {

/** Texture interface. User can store different views of the texture
 * which it supports. If all views of the texture become deleted texture source will be deleted.
 */
class Texture :
    public Resource
{
public:
    /** texture type */
    enum TYPE
    {
        TEXTURE_1D,
        TEXTURE_2D,
        TEXTURE_3D,
        TEXTURE_CUBE_MAP
    };

    /** texture format */
    enum FORMAT
    {
        UNKNOWN,
        RGBA16,
        RGB16,
        ALPHA16,
        RGBA8,
        RGB8,
        ALPHA8,
        D32,
        D24,
        D16,
        D24S8,
        D32F,
        D32FS8,
        RGBA32F,
        RGB32F,
        RG32F,
        R32F,
        ALPHA32F,
        RGBA16F,
        RGB16F,
        RG16F,
        R16F,
        ALPHA16F,
        RGBA32UI,
        RGB32UI,
        ALPHA32UI,
        RGBA16UI,
        RGB16UI,
        ALPHA16UI,
        RGBA8UI,
        RGB8UI,
        ALPHA8UI,
        RGBA32I,
        RGB32I,
        ALPHA32I,
        RGBA16I,
        RGB16I,
        ALPHA16I,
        RGBA8I,
        RGB8I,
        ALPHA8I,
        COMPRESSED_RGB_S3TC_DXT1,
        COMPRESSED_RGBA_S3TC_DXT1,
        COMPRESSED_RGBA_S3TC_DXT3,
        COMPRESSED_RGBA_S3TC_DXT5,
        COMPRESSED_RED_RGTC1,
        COMPRESSED_SIGNED_RED_RGTC1,
        COMPRESSED_RG_RGTC2,
        COMPRESSED_SIGNED_RG_RGTC2,
        __NUMBER_OF_FORMATS__ /// Number of formats. Reserved for internal purposes
    };

    /** 1D Texture description. */
    struct TEXTURE_1D_DESC
    {
        FORMAT          format;
        unsigned int    width;
        const void*     data;

        TEXTURE_1D_DESC() :
            format(UNKNOWN),
            width(0),
            data(0)
        {}
    };

    /** Texture buffer description. */
    struct TEXTURE_BUFFER_DESC
    {
        FORMAT format;

        TEXTURE_BUFFER_DESC() :
            format(UNKNOWN)
        {}
    };

    /** 3D texture description. */
    struct TEXTURE_3D_DESC
    {
        FORMAT          format;
        unsigned int    width;
        unsigned int    height;
        unsigned int    depth;
        const void*     data;

        TEXTURE_3D_DESC() :
            format(UNKNOWN),
            width(0),
            height(0),
            depth(0),
            data(0)
        {}
    };

    /** 3D multisampled texture description. */
    struct TEXTURE_3D_MS_DESC
    {
        FORMAT          format;
        unsigned int    width;
        unsigned int    height;
        unsigned int    depth;
        unsigned int    samples;

        TEXTURE_3D_MS_DESC() :
            format(UNKNOWN),
            width(0),
            height(0),
            depth(0),
            samples(0)
        {}
    };

public:
    /** Information about texture format */
    struct format_desc
    {
        unsigned int    sizeInBits;
        unsigned int    numComponents;
        bool            depth;
        bool            compressed;
    };

    /** Traits containing format information for runtime checks. Implementation in Device.cpp */
    SGL_DLLEXPORT static const format_desc FORMAT_TRAITS[];

    /** Bind texture to the device.
     * @param stage - texture stage.
     */
    virtual SGL_HRESULT SGL_DLLCALL Bind(unsigned int stage) const = 0;

    /** Unbind texture from the device if it is binded*/
    virtual void SGL_DLLCALL Unbind() const = 0;

public:
    /** Get texture type. */
    virtual TYPE SGL_DLLCALL Type() const = 0;

    virtual ~Texture() {}
};

} // namespace sgl

#endif // SIMPLE_GL_TEXTURE_H
