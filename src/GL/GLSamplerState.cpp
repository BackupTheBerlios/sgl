#include "GL/GLSamplerState.h"

using namespace sgl;

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
        GL_CLAMP,
        GL_CLAMP_TO_EDGE
    };

} // anonymous namespace

GLSamplerState::GLSamplerState( sgl::GLDevice*  _device, 
                                const DESC&     _desc ) :
    device(_device),
    desc(_desc)
{
}

GLSamplerState::~GLSamplerState()
{
}

void GLSamplerState::Bind(unsigned int stage) const
{
    device->SetSamplerState(stage, this);
}
