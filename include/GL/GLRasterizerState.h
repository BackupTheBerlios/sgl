#ifndef SIMPLE_GL_GL_RASTERIZER_STATE_H
#define SIMPLE_GL_GL_RASTERIZER_STATE_H

#include "GLDevice.h"

namespace sgl {

class GLRasterizerState :
    public ReferencedImpl<RasterizerState>
{
public:
    GLRasterizerState(sgl::GLDevice* device, const DESC& desc);
    ~GLRasterizerState();

    // Override RasterizerState
    const math::Vector4i&   SGL_DLLCALL ScissorRectangle() const { return scissorRect; }
    const DESC&             SGL_DLLCALL Desc() const             { return desc; }

    void SGL_DLLCALL Bind() const;
    void SGL_DLLCALL SetScissorRectangle(const math::Vector4i& rect);

private:
    ref_ptr<GLDevice>   device;

    // setup display list
    GLuint              bindDisplayList;

    // properties
    DESC                desc;
    math::Vector4i      scissorRect;
};

} // namepsace sgl

#endif // SIMPLE_GL_GL_RASTERIZER_STATE_H
