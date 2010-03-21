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

template<DEVICE_VERSION DeviceVersion>
GLSamplerState<DeviceVersion>::GLSamplerState( GLDevice<DeviceVersion>*    device_, 
                                               const DESC&                 desc_ ) :
    device(device_),
    desc(desc_)
{
}

// explicit template instantiation
template class GLSamplerState<DV_OPENGL_1_3>;
template class GLSamplerState<DV_OPENGL_1_4>;
template class GLSamplerState<DV_OPENGL_1_5>;
template class GLSamplerState<DV_OPENGL_2_0>;
template class GLSamplerState<DV_OPENGL_2_1>;
template class GLSamplerState<DV_OPENGL_3_0>;
template class GLSamplerState<DV_OPENGL_3_1>;
template class GLSamplerState<DV_OPENGL_3_2>;