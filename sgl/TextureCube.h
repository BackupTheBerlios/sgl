#ifndef SIMPLE_GL_TEXTURE_CUBE_H
#define SIMPLE_GL_TEXTURE_CUBE_H

#include "Texture2D.h"

namespace sgl {

/** Cube mape texture view interface. */
class TextureCube :
	public Texture
{
public:
    /// Cube map side id
    enum SIDE
    {
        POSITIVE_X,
        NEGATIVE_X,
        POSITIVE_Y,
        NEGATIVE_Y,
        POSITIVE_Z,
        NEGATIVE_Z
    };

    /** Cube map texture description. */
    struct DESC
    {
        Texture2D::DESC sides[6];
    };

public:
    /** Force mipmap regeneration.
     * @return result of the operation. Can be SGLERR_INVALID_CALL, SGLERR_UNSUPPORTED
     */
    virtual SGL_HRESULT SGL_DLLCALL GenerateMipmap() = 0;

    /** Setup texture to the device.
     * @param stage - texture stage.
     * @return result of the operation. Could return SGLERR_INVALID_CALL if
     * texture stage is invalid
     */
    virtual SGL_HRESULT SGL_DLLCALL Bind(unsigned int stage) const = 0;

    /** Unbind texture from the device if it is binded*/
    virtual void SGL_DLLCALL Unbind() const = 0;

    /** Get 2D texture side view. 
     * @param side - side of the cube map.
     */
    virtual Texture2D* SGL_DLLCALL Side(SIDE side) = 0;

    /** Bind sampler state to the texture. */
    virtual SGL_HRESULT SGL_DLLCALL BindSamplerState(SamplerState* samplerState) = 0;

    virtual ~TextureCube() {}
};

} // namespace sgl

#endif // SIMPLE_GL_TEXTURE_CUBE_H
