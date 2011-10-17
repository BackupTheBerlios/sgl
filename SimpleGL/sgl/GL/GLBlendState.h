#ifndef SIMPLE_GL_GL_BLEND_STATE_H
#define SIMPLE_GL_GL_BLEND_STATE_H

#include "GLForward.h"
#include "../BlendState.h"

namespace sgl {

#ifndef __ANDROID__

class GLBlendStateDisplayLists :
	public ReferencedImpl<BlendState>
{
public:
	GLBlendStateDisplayLists(GLDevice*					device, 
							 const BlendState::DESC&    desc);
	~GLBlendStateDisplayLists();

	// Override BlendState
	void SGL_DLLCALL Bind() const;
	const BlendState::DESC& SGL_DLLCALL Desc() const { return desc; }

private:
	GLDevice*        device;
	unsigned         bindDisplayList;
	BlendState::DESC desc;
};

#endif // !defined(__ANDROID__)

class GLBlendStateSeparate :
	public ReferencedImpl<BlendState>
{
public:
	GLBlendStateSeparate(GLDevice*					device, 
						 const BlendState::DESC&    desc);
	~GLBlendStateSeparate();

	// Override BlendState
	void SGL_DLLCALL Bind() const;
	const BlendState::DESC& SGL_DLLCALL Desc() const { return desc; }

private:
	GLDevice*        device;
	BlendState::DESC desc;
};

class GLBlendStateDefault :
	public ReferencedImpl<BlendState>
{
public:
	GLBlendStateDefault(GLDevice*				device, 
						const BlendState::DESC& desc);
	~GLBlendStateDefault();

	// Override BlendState
	void SGL_DLLCALL Bind() const;
	const BlendState::DESC& SGL_DLLCALL Desc() const { return desc; }

private:
	GLDevice*			device;
	BlendState::DESC    desc;

	// gl bindings
	unsigned	glSrcBlend;
	unsigned	glDestBlend;
	unsigned	glBlendOp;
};

} // namepsace sgl

#endif // SIMPLE_GL_GL_BLEND_STATE_H
