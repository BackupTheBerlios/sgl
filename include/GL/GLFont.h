#ifndef SIMPLE_GL_GL_FONT_H
#define SIMPLE_GL_GL_FONT_H

#include "Device.h"
#include "Font.h"
#include "GLCommon.h"
#include <memory>

namespace sgl {

template<PIPELINE_TYPE pipeline>
struct font_attributes;

template<>
struct font_attributes<PT_FIXED>
{
    // States
    ref_ptr<FFPProgram>         ffpProgram;
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

template<>
struct font_attributes<PT_PROGRAMMABLE>
{
    // States
    ref_ptr<Program>    program;
    SamplerUniform2D*   textureUniform;
    Uniform4F*          colorUniform;
    Uniform2F*          positionUniform;  
    Uniform2F*          scaleUniform;

    // Char info
    mutable float       charWidth;
    mutable float       charHeight;
    mutable float       vpWidth;
    mutable float       vpHeight;
};

/* Font class */
template<PIPELINE_TYPE pipeline>
class GLFont :
    public ResourceImpl<Font>
{
public:
    GLFont(Device* _pDevice);
    ~GLFont();

    SGL_HRESULT     SGL_DLLCALL SetTexture(sgl::Texture2D* texture);
    sgl::Texture2D* SGL_DLLCALL Texture() const;

    SGL_HRESULT SGL_DLLCALL Bind(int width, 
                                 int height,
                                 const math::Vector4f& color) const;
    void        SGL_DLLCALL Unbind() const;
    void        SGL_DLLCALL Print(float x, float y, const char* str) const;



protected:
    // drawing
    ref_ptr<Device>             pDevice;
	ref_ptr<Texture2D>          texture;
    ref_ptr<VertexBuffer>       vbo;
    ref_ptr<VertexLayout>       vertexLayout;
    ref_ptr<BlendState>         blendState;
    ref_ptr<DepthStencilState>  depthStencilState;

    // attributes specified by PIPELINE_TYPE
    font_attributes<pipeline>   myAttr;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_FONT_H
