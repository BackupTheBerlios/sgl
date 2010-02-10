#ifndef SIMPLE_GL_RENDER_TARGET_H
#define SIMPLE_GL_RENDER_TARGET_H

#include "Texture1D.h"
#include "Texture3D.h"
#include "TextureCube.h"

namespace sgl {

/** Render target factory class. Before using it is recommended
 * to call Dirty() function and check consistency of the render target.
 */
class RenderTarget : 
    public Referenced
{
public:
    /** Bind render target to the device. 
     * @return result of the operation. Can be SGLERR_INVALID_CALL if render target is dirty.
     */    
    virtual SGL_HRESULT SGL_DLLCALL Bind() const = 0;

    /** Unbind render target from the device. */
    virtual void SGL_DLLCALL Unbind() const = 0;

    /** Setup active draw buffer. Default value is 0.
     * Sets SGLERR_INVALID_CALL if mrtIndex is invalid
     * @param target - index of the color attachment to be used as draw buffer
     * @return result of the operation
     */
    virtual SGL_HRESULT SGL_DLLCALL SetDrawBuffer(unsigned int target) = 0;

    /** Setup active draw buffers. Default value is 0.
     * Sets SGLERR_INVALID_CALL if any of the draw buffer target is invalid. Checks
     * only that draw buffer target is valid index( e.g. <= max color attachments )
     * @param numTargets - number of render targets
     * @param targets - indices of the color attachments to be used as draw buffers
     * @return result of the operation
     */
    virtual SGL_HRESULT SGL_DLLCALL SetDrawBuffers(unsigned int numTargets, unsigned int* targets) = 0;

    /** Get active draw buffers 
     * @param targets [out] - indices of the color attachments used as draw buffers. Targets must
     *  have enouth space to store draw buffer indices or be NULL(then only number of active draw buffers will be returned).
     * @return number of active draw buffers
     */
    virtual unsigned int SGL_DLLCALL DrawBuffers(unsigned int* targets) const = 0;

    /** Setup read buffer. Default is 0. Could be used when renderTarget is in use.
     * Sets SGLERR_INVALID_CALL if render draw buffer or read buffer are invalid
     * @param target - index of the color attachment used as read buffer
     * @return result of the operation
     */
    virtual SGL_HRESULT SGL_DLLCALL SetReadBuffer(unsigned int target) = 0;

    /** Get active read buffer */
    virtual unsigned int SGL_DLLCALL ReadBuffer() const = 0;

    /** Setup depth stencil renderbuffer format and multisampling. If depth stencil
     * texture attachment exists, then it will replace renderbuffer. Makes render target dirty.
     * @param toggle - toggle using depth stencil buffer.
     * @param format - format of the depth stencil buffer.
     * @param samples - number of samples in the depth stencil buffer.
     * @return result of the operation. Can be SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetDepthStencil( bool             toggle,
                                                     Texture::FORMAT  format   = Texture::D24S8, 
                                                     unsigned int     samples  = 0 ) = 0;

    /** Setup 1d texture as color render target. Texture will be treated as 2d 
     * texture with height of 1. Makes render target dirty.
     * @param mrtIndex - index of the color attachment.
     * @param texture - texture to be used as color buffer.
     * @param level - mip level of the texture attachment.
     * @return result of the operation. Can be SGLERR_INVALID_CALL if mrtIndex or level is invalid.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetColorAttachment( unsigned int    mrtIndex,
                                                        Texture1D*      texture, 
                                                        unsigned int    level ) = 0;

    /** Setup 2d texture as color render target. Makes render target dirty.
     * @param mrtIndex - index of the color attachment.
     * @param texture - texture to be used as color buffer.
     * @param level - mip level of the texture attachment.
     * @return result of the operation. Can be SGLERR_INVALID_CALL if mrtIndex or level is invalid.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetColorAttachment( unsigned int    mrtIndex,
                                                        Texture2D*      texture, 
                                                        unsigned int    level ) = 0;

    /** Setup 3d texture layer as color render target. Makes render target dirty.
     * @param mrtIndex - index of the color attachment.
     * @param texture - texture to be used as color buffer.
     * @param level - mip level of the texture attachment.
     * @param layer - layer of the 3d texture for rendering into.
     * @return result of the operation. Can be SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetColorAttachment( unsigned int    mrtIndex,
                                                        Texture3D*      texture, 
                                                        unsigned int    level,
                                                        unsigned int    layer ) = 0;

    /** Setup 3d texture as color render target for layered framebuffer.
     * Makes render target dirty.
     * @param mrtIndex - index of the color attachment.
     * @param texture - texture to be used as color buffer.
     * @param level - mip level of the texture attachment.
     * @return result of the operation. Can be SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetColorAttachmentLayered( unsigned int    mrtIndex,
                                                               Texture3D*      texture, 
                                                               unsigned int    level ) = 0;

    /** Setup cube map texture as color render target for layered framebuffer.
     * Makes render target dirty.
     * @param mrtIndex - index of the color attachment.
     * @param texture - texture to be used as color buffer.
     * @param level - mip level of the texture attachment.
     * @return result of the operation. Can be SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetColorAttachmentLayered( unsigned int        mrtIndex,
                                                               TextureCube*        texture, 
                                                               unsigned int        level ) = 0;

    /** Get texture used as color render target.
     * @param mrtIndex - index of the color attachment.
     */
    virtual Texture* SGL_DLLCALL ColorAttachment(unsigned int mrtIndex) const = 0;

    /** Set 2d texture as depth stencil surface. Makes render target dirty. 
     * @param texture - texture to be used as depth stencil buffer
     * @param level - mip level of the texture.
     * @return result of the operation. Can be SGLERR_INVALID_CALL if texture format is not depth format.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetDepthStencilAttachment( Texture2D*       depthStencilTexture,
                                                               unsigned int     level ) = 0;

    /** Set 3d texture as depth stencil surface for layered framebuffer.
     * Makes render target dirty.
     * @param texture - texture to be used as depth stencil buffer
     * @param level - mip level of the texture.
     * @return result of the operation. Can be SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetDepthStencilAttachmentLayered( Texture3D*       depthStencilTexture,
                                                                      unsigned int     level ) = 0;

    /** Set cube map texture as depth stencil surface for layered framebuffer.
     * Makes render target dirty.
     * @param texture - texture to be used as depth stencil buffer
     * @param level - mip level of the texture.
     * @return result of the operation. Can be SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetDepthStencilAttachmentLayered( TextureCube*     depthStencilTexture,
                                                                      unsigned int     level ) = 0;

    /** Get texture used as depth stencil surface */
    virtual Texture* SGL_DLLCALL DepthStencilAttachment() const = 0;

    /** Check wether render target have depth stencil texture or render buffer attachment. */
    virtual bool SGL_DLLCALL HaveDepthStencil() const = 0;

    /** Check wether render target is dirty. */
    virtual bool SGL_DLLCALL IsDirty() const = 0;

    /** Create render target view. */
    virtual SGL_HRESULT SGL_DLLCALL Dirty(bool force = false) = 0;

    virtual ~RenderTarget() {}
};

} // namespace sgl

#endif // SIMPLE_GL_RENDER_TARGET_H
