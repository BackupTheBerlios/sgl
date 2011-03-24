#ifndef SIMPLE_GL_GL_VBO_RENDER_TARGET_H
#define SIMPLE_GL_GL_VBO_RENDER_TARGET_H

#include "GLVertexBuffer.h"

namespace sgl {
/*
class GLVBORenderTarget :
    public GLFBO<VBORenderTarget>
{
friend class GLDevice;
public:
    typedef GLFBO<VBORenderTarget>  base_type;

    typedef ref_ptr<GLVertexBuffer> vbo_ptr;

protected:
    GLVBORenderTarget(Device* _device);

    // Override RenderTarget
    void SGL_DLLCALL Unbind() const;

public:
	// Override VBORenderTarget
    SGL_HRESULT   SGL_DLLCALL SetVBOAttachment(unsigned int mrtIndex, VertexBuffer* vbo);
	VertexBuffer* SGL_DLLCALL VBOAttachment(unsigned int mrtIndex) const;

     ~GLVBORenderTarget() {}

protected:
	// output vbo
	std::vector<vbo_ptr>	vboTargets;
};
*/
} // namespace sgl

#endif // SIMPLE_GL_GL_VBO_RENDER_TARGET_H
