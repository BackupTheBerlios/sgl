#ifndef SIMPLE_GL_GL_RASTERIZER_STATE_H
#define SIMPLE_GL_GL_RASTERIZER_STATE_H

#include "GLForward.h"
#include "../RasterizerState.h"

namespace sgl {

class GLRasterizerState :
	public ReferencedImpl<RasterizerState>
{
public:
	GLRasterizerState(GLDevice* device, const DESC& desc);
	~GLRasterizerState();

	// Override RasterizerState
	const DESC& SGL_DLLCALL Desc() const { return desc; }
	void SGL_DLLCALL Bind() const;

private:
	GLDevice*   device;
	DESC        desc;

	// gl primitives
	unsigned    glCullMode;
	unsigned    glFillMode;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_RASTERIZER_STATE_H
