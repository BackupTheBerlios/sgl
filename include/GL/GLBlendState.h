#ifndef SIMPLE_GL_GL_BLEND_STATE_H
#define SIMPLE_GL_GL_BLEND_STATE_H

#include "GLDevice.h"

namespace sgl {

class GLBlendState :
    public ReferencedImpl<BlendState>
{
public:
    GLBlendState(Device* device, const DESC& desc);
    ~GLBlendState();

    // Override State
    void SGL_DLLCALL Bind() const;

    // Override RasterizerState
    const DESC& SGL_DLLCALL Desc() const { return desc; }

private:
    ref_ptr<Device>     device;

    // setup display list
    GLuint              bindDisplayList;

    // properties
    DESC                desc;
};

} // namepsace sgl

#endif // SIMPLE_GL_GL_BLEND_STATE_H
