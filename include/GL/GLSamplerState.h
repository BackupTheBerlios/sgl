#ifndef SIMPLE_GL_GL_SAMPLER_STATE_H
#define SIMPLE_GL_GL_SAMPLER_STATE_H

#include "GLDevice.h"

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
class GLSamplerState :
    public ReferencedImpl<SamplerState>
{
public:
    GLSamplerState(sgl::GLDevice<DeviceVersion>* device, const DESC& desc);

    // Override SamplerState
    const DESC& SGL_DLLCALL Desc() const { return desc; }

private:
    GLDevice<DeviceVersion>* device;

    // properties
    DESC desc;
};

} // namepsace sgl

#endif // SIMPLE_GL_GL_SAMPLER_STATE_H
