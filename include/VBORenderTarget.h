#ifndef SIMPLE_GL_VBO_RENDER_TARGET_H
#define SIMPLE_GL_VBO_RENDER_TARGET_H

#include "RenderTarget.h"
#include "VertexBuffer.h"

namespace sgl {

/** Interface for r2vb render targets. 
 * This acts as traditional render target, but output is also copied or directly transmitted
 * to the vertex buffer. You could set textures as color attachments, they will mapped to
 * corresponding vertex buffer elements. 
 * If the device supports rendering to the vertex buffer using data copy from render target to the VertexBuffer,
 * then copy operation will be performed after unbinding render target.
 */
class VBORenderTarget : 
    public RenderTarget
{
public:
    /** Set output vertex buffer. During Unbind texture content will be copied to the vertex buffer.
	 * @param mrtIndex - index of the color attachment that would be output for the vbo.
     * @param vbo - output vbo. Can be NULL, then output will only rendered to the assigned texture target.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetVBOAttachment(unsigned int mrtIndex, VertexBuffer* vbo) = 0;
    /** Get VertexBuffer attached to the render target at the specified slot. 
	 * @param mrtIndex - index of the color attachment the vbo should be mapped.
	 */
    virtual VertexBuffer* SGL_DLLCALL VBOAttachment(unsigned int mrtIndex) const = 0;

    virtual ~VBORenderTarget() {}
};

} // namespace sgl

#endif // SIMPLE_GL_VBO_RENDER_TARGET_H
