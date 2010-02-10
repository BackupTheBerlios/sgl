#ifndef SIMPLE_GL_GL_DEPTH_STENCIL_STATE_H
#define SIMPLE_GL_GL_DEPTH_STENCIL_STATE_H

#include "../DepthStencilState.h"
#include "GLDevice.h"

namespace sgl {

class GLDepthStencilState :
    public ReferencedImpl<DepthStencilState>
{
public:
    GLDepthStencilState(sgl::GLDevice* device, const DESC& desc);
    ~GLDepthStencilState();

    // Override State
    void SGL_DLLCALL Bind() const;

    // Override RasterizerState
    const DESC& SGL_DLLCALL Desc() const { return desc; }

private:
    ref_ptr<GLDevice>   device;

    // setup display list
    GLuint              bindDisplayList;

    // properties
    DESC                desc;
};

} // namepsace sgl

#endif // SIMPLE_GL_GL_DEPTH_STENCIL_STATE_H
