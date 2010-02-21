#ifndef SIMPLE_GL_GL_SAMPLER_STATE_H
#define SIMPLE_GL_GL_SAMPLER_STATE_H

#include "GLDevice.h"

namespace sgl {

class GLSamplerState :
    public ReferencedImpl<SamplerState>
{
public:
    GLSamplerState(sgl::Device* device, const DESC& desc);
    ~GLSamplerState();

    // Override SamplerState
    const DESC& SGL_DLLCALL Desc() const { return desc; }

private:
    mutable ref_ptr<Device> device;

    // properties
    DESC desc;
};

} // namepsace sgl

#endif // SIMPLE_GL_GL_SAMPLER_STATE_H
