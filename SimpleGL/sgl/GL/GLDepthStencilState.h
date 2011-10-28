#ifndef SIMPLE_GL_GL_DEPTH_STENCIL_STATE_H
#define SIMPLE_GL_GL_DEPTH_STENCIL_STATE_H

#include "GLForward.h"
#include "../DepthStencilState.h"

namespace sgl {

#ifndef SIMPLE_GL_ES

class GLDepthStencilStateDisplayLists :
	public ReferencedImpl<DepthStencilState>
{
public:
	GLDepthStencilStateDisplayLists(GLDevice* device, const DESC& desc);
	~GLDepthStencilStateDisplayLists();

	// Override DepthStencilState
	void SGL_DLLCALL Bind() const;
	const DESC& SGL_DLLCALL Desc() const { return desc; }

private:
	GLDevice*    device;
	unsigned     bindDisplayList;
	DESC         desc;
};

#endif // SIMPLE_GL_ES

class GLDepthStencilStateSeparate :
	public ReferencedImpl<DepthStencilState>
{
public:
	GLDepthStencilStateSeparate(GLDevice* device, const DESC& desc);
	~GLDepthStencilStateSeparate();

	// Override State
	void SGL_DLLCALL Bind() const;

	// Override DepthStencilState
	const DESC& SGL_DLLCALL Desc() const { return desc; }

private:
	GLDevice*	device;
	DESC        desc;
};

} // namepsace sgl

#endif // SIMPLE_GL_GL_DEPTH_STENCIL_STATE_H
