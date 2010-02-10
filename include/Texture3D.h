#ifndef SIMPLE_GL_TEXTURE_3D_H
#define SIMPLE_GL_TEXTURE_3D_H

#include "Texture.h"

namespace sgl {

/** 3 dimensional texture view interface. */
class Texture3D :
	public Texture
{
public:
    /** 3D Texture description. */
    struct DESC
    {
        FORMAT          format;
        unsigned int    width;
        unsigned int    height;
        unsigned int    depth;
        const void*     data;

        DESC() :
            format(UNKNOWN),
            width(0),
            height(0),
            depth(0),
            data(0)
        {}
    };

    /** 3D Multisampled texture description. */
    struct DESC_MS
    {
        FORMAT          format;
        unsigned int    width;
        unsigned int    height;
        unsigned int    depth;
        unsigned int    samples;

        DESC_MS() :
            format(UNKNOWN),
            width(0),
            height(0),
            depth(0),
            samples(0)
        {}
    };

public:
    /** Get format of the texture */
    virtual Texture::FORMAT SGL_DLLCALL Format() const = 0;

    /** Get number of samples per texel in the texture */
    virtual unsigned int SGL_DLLCALL Samples() const = 0;

    /** Get width of the texture */
    virtual unsigned int SGL_DLLCALL Width() const = 0;

    /** Get height of the texture */
    virtual unsigned int SGL_DLLCALL Height() const = 0;

    /** Get depth of the texture */
    virtual unsigned int SGL_DLLCALL Depth() const = 0;

    /** Get number of mipmap levels in the texture */
    virtual unsigned int SGL_DLLCALL NumMipmaps() const = 0;

    /** Force mipmap regeneration.
     * @return result of the operation. Can be SGLERR_INVALID_CALL, SGLERR_UNSUPPORTED
     */
    virtual SGL_HRESULT SGL_DLLCALL GenerateMipmap() = 0;

    /** Setup sub image of the texture. Will fail for multisample textures.
     * @param mipmap - mipmap level of the texture for access.
     * @param offsetx - x offset of the region.
     * @param offsety - y offset of the region.
     * @param offsety - z offset of the region.
     * @param width - width of the region.
     * @param height - height of the region.
     * @param depth - depth of the region.
	 * @param data - texture pixels.
	 * @return status of the operation. Could be SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetSubImage( unsigned int    mipmap,
                                                 unsigned int    offsetx,
                                                 unsigned int    offsety,
                                                 unsigned int    offsetz,
                                                 unsigned int    width, 
                                                 unsigned int    height,
                                                 unsigned int    depth,
                                                 const void*     data ) = 0;

    /** Get image data
     * @param mipmap - mipmap index.
	 * @param data - texture pixels output.
	 * @return status of the operation. Can be SGLERR_INVALID_CALL if mipmap or data is invalid.
     */
    virtual SGL_HRESULT SGL_DLLCALL GetImage( unsigned int  mipmap,
                                              void*         data ) = 0;

    /** Setup texture to the device.
     * @param stage - texture stage.
     * @return result of the operation. Could return SGLERR_INVALID_CALL if
     * texture stage is invalid
     */
    virtual SGL_HRESULT SGL_DLLCALL Bind(unsigned int stage) const = 0;

    /** Unbind texture from the device if it is binded*/
    virtual void SGL_DLLCALL Unbind() const = 0;

    virtual ~Texture3D() {}
};

} // namespace sgl

#endif // SIMPLE_GL_TEXTURE_3D_H
