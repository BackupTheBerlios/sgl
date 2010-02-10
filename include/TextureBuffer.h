#ifndef SIMPLE_GL_TEXTURE_1D_H
#define SIMPLE_GL_TEXTURE_1D_H

#include "Texture.h"

namespace sgl {

// forward
class TextureBuffer;

/** 1 dimensional texture view interface. */
class Texture1D :
	public Texture
{
public:
    /** Get format of the texture */
    virtual Texture::FORMAT SGL_DLLCALL Format() const = 0;

    /** Get width of the texture */
    virtual unsigned int SGL_DLLCALL Width() const = 0;

    /** Get number of mipmap levels in the texture */
    virtual unsigned int SGL_DLLCALL NumMipmaps() const = 0;

    /** Force mipmap regeneration.
     * @return result of the operation. Can be SGLERR_INVALID_CALL, SGLERR_UNSUPPORTED
     */
    virtual SGL_HRESULT SGL_DLLCALL GenerateMipmap() = 0;

    /** Setup sub image of the texture. Will fail for multisample textures.
     * @param mipmap - mipmap level of the texture for access.
     * @param offsetx - x offset of the region.
     * @param width - width of the region.
	 * @param data - texture pixels.
	 * @return status of the operation. Could be SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetSubImage( unsigned int    mipmap,
                                                 unsigned int    offsetx,
                                                 unsigned int    width, 
                                                 const void*     data ) = 0;

    /** Setup mip level of the texture. Mipmap width and height are calculated
     * using mipmap level. 
     * @param mipmap - mipmap index.
	 * @param data - texture pixels. If NULL then create an empty texture.
	 * @return status of the operation. Could be SGLERR_OUT_OF_MEMORY, SGLERR_INVALID_CALL
     */
    virtual SGL_HRESULT SGL_DLLCALL SetImage( unsigned int  mipmap,
                                              const void*   data ) = 0;

    /** Get image data.
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

    virtual ~Texture1D() {}
};

} // namespace sgl

#endif // SIMPLE_GL_TEXTURE_1D_H
