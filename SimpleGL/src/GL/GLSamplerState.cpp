#include "GL/GLSamplerState.h"

namespace {

    const GLenum BIND_TEXTURE_FILTER[] =
    {
        GL_NEAREST,
        GL_LINEAR
    };

    const GLenum BIND_TEXTURE_MIN_FILTER[] =
    {
        GL_NEAREST_MIPMAP_NEAREST,
        GL_NEAREST_MIPMAP_LINEAR,
        GL_LINEAR_MIPMAP_NEAREST,
        GL_LINEAR_MIPMAP_LINEAR
    };

    const GLenum BIND_TEXTURE_CLAMP[] =
    {
        GL_REPEAT,
        GL_CLAMP_TO_EDGE
    };

} // anonymous namespace

namespace sgl {

GLSamplerState::GLSamplerState(GLDevice* device_, const DESC& desc_) :
    device(device_),
	desc(desc_)
{
}

} // namespace sgl
