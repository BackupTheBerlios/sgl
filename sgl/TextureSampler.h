#ifndef SIMPLE_GL_TEXTURE_SAMPLER_H
#define SIMPLE_GL_TEXTURE_SAMPLER_H

#include "Texture.h"
#include "SamplerState.h"

namespace sgl {

/** 2 dimensional texture interface. */
class TextureSamplerView :
    public TextureView
{
public:
    /** Setup texture to the device
     * @param slot - texture stage
     * @return result of the operation. Could return SGLERR_INVALID_CALL if
     * texture or slot is invalid
     */
    virtual SGL_HRESULT SGL_DLLCALL Bind(unsigned int slot) const = 0;

    /** Unbind texture from the device if it is binded*/
    virtual void SGL_DLLCALL Unbind() const = 0;

    /** Bind sampler state to the texture. It will setup with the texture setup. 
     * You can set this to NULL, then texture will receive sampler state from the device.
     */
    virtual void SGL_DLLCALL BindSamplerState(const SamplerState* _samplerState) = 0;

    virtual ~TextureSamplerView() {}
};

} // namespace sgl

#endif // SIMPLE_GL_TEXTURE_SAMPLER_H
