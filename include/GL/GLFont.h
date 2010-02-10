#ifndef SIMPLE_GL_GL_FONT_H
#define SIMPLE_GL_GL_FONT_H

#include "Device.h"
#include "Font.h"
#include "GLCommon.h"
#include <memory>

namespace sgl {

/* Font class */
class GLFont :
    public ResourceImpl<Font>
{
friend class GLDevice;
protected:
    GLFont(Device* _pDevice);

public:
	// initialize resource
    SGL_HRESULT SGL_DLLCALL Dirty(bool force = false);

    // files
    SGL_HRESULT SGL_DLLCALL LoadFromFile(const char* fileName);

    // work
    SGL_HRESULT SGL_DLLCALL Bind(int width, int height) const;
    void SGL_DLLCALL Unbind() const;
    void SGL_DLLCALL Print(float x, float y, const char* str) const;

    virtual SGL_DLLCALL ~GLFont();

protected:
    // drawing
    ref_ptr<Device>             pDevice;
	ref_ptr<Texture2D>          texture;
    ref_ptr<VertexBuffer>       vbo;
    ref_ptr<VertexLayout>       vertexLayout;

    // States
    ref_ptr<FFPProgram>         ffpProgram;
    ref_ptr<BlendState>         blendState;
    ref_ptr<DepthStencilState>  depthStencilState;
    ref_ptr<SamplerState>       samplerState;
    Uniform4x4F*                projectionMatrixUniform;
    Uniform4x4F*                modelViewMatrixUniform;
    sgl::UniformI*              lightingEnableUniform;
    sgl::Uniform4F*             ambientUniform;
    sgl::Uniform4F*             materialAmbientUniform;    

    // remember states
    mutable math::Matrix4f      projectionMatrix;
    mutable math::Matrix4f      modelViewMatrix;

    // Char info
    float   charWidth;
    float   charHeight;

    // Print info
    mutable int width;
    mutable int height;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_FONT_H
