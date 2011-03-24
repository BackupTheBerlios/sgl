#include "GL/GLRasterizerState.h"
#include "GL/GLCommon.h"
#include "GL/GLDevice.h"

namespace {

    using namespace sgl;

    GLenum BIND_CULL_MODE[] =
    {
        GL_NONE,
        GL_BACK,
        GL_FRONT,
	    GL_FRONT_AND_BACK
    };

    GLenum BIND_FILL_MODE[] =
    {
	    GL_FILL,
	    GL_LINE
    };

} // anonymous namespace

namespace sgl {

GLRasterizerState::GLRasterizerState(GLDevice* device_, const DESC& desc_) :
	device(device_),
	desc(desc_)
{
	glCullMode  = BIND_CULL_MODE[desc.cullMode];
	glFillMode  = BIND_FILL_MODE[desc.fillMode];
}

GLRasterizerState::~GLRasterizerState()
{
    assert(device->CurrentRasterizerState() != this);
}

void GLRasterizerState::Bind() const
{
	if (desc.cullMode != NONE)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(glCullMode);
	}
	else {
		glDisable(GL_CULL_FACE);
	}
	glPolygonMode(GL_FRONT_AND_BACK, glFillMode);
	glColorMask(desc.colorMask & RED,
		desc.colorMask & GREEN,
		desc.colorMask & BLUE,
		desc.colorMask & ALPHA);

	device->SetRasterizerState(this);
}

} // namespace sgl
