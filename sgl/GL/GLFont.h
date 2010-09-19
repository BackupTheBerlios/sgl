#ifndef SIMPLE_GL_GL_FONT_H
#define SIMPLE_GL_GL_FONT_H

#include "GLForward.h"
#include "../Font.h"

namespace sgl {

/* Font class */
template<DEVICE_VERSION DeviceVersion>
class GLFont :
    public ResourceImpl<Font>
{
public:
    GLFont(GLDevice<DeviceVersion>* _pDevice);

    // Override Font
    SGL_HRESULT     SGL_DLLCALL SetTexture(sgl::Texture2D* texture);
    sgl::Texture2D* SGL_DLLCALL Texture() const;

protected:
    // drawing
    GLDevice<DeviceVersion>*    device;
    ref_ptr<Texture2D>          texture;
    ref_ptr<VertexBuffer>       vbo;
    ref_ptr<VertexLayout>       vertexLayout;
    ref_ptr<BlendState>         blendState;
    ref_ptr<DepthStencilState>  depthStencilState;
};

/** Make suitable font */
template<DEVICE_VERSION DeviceVersion>
sgl::Font* sglCreateFont(GLDevice<DeviceVersion>* device);

} // namespace sgl

#endif // SIMPLE_GL_GL_FONT_H
