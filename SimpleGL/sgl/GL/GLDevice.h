#ifndef SIMPLE_GL_GL_DEVICE_H
#define SIMPLE_GL_GL_DEVICE_H

#include "GL/GLCommon.h"

#ifdef WIN32
#   pragma push_macro("CreateFont")
#   pragma push_macro("DEVICE_TYPE")
#   undef CreateFont
#   undef DEVICE_TYPE
#endif

#include <stack>
#include "Device.h"
#include "Utility/Referenced.h"

namespace sgl {

/* GLDevice class wraps gl functions */
class GLDevice :
	public ReferencedImpl<Device>
{
protected:
    typedef std::stack< ref_ptr<const State> > state_stack;

public:
    GLDevice(const Device::VIDEO_DESC& desc);
    GLDevice();

    // setup
    virtual rectangle           SGL_DLLCALL Viewport() const;
    virtual void                SGL_DLLCALL SetViewport(const rectangle& vp);

    // internal binders
    const BlendState*           SGL_DLLCALL CurrentBlendState() const                       { return currentBlendState; }
    const DepthStencilState*    SGL_DLLCALL CurrentDepthStencilState() const                { return currentDepthStencilState; }
    const RasterizerState*      SGL_DLLCALL CurrentRasterizerState() const                  { return currentRasterizerState; }
    const Texture*              SGL_DLLCALL CurrentTexture(unsigned int stage) const        { return currentTexture[stage]; }
    const VertexBuffer*         SGL_DLLCALL CurrentVertexBuffer() const                     { return currentVertexBuffer; }
    const VertexLayout*         SGL_DLLCALL CurrentVertexLayout() const                     { return currentVertexLayout; }
    const IndexBuffer*          SGL_DLLCALL CurrentIndexBuffer() const                      { return currentIndexBuffer; }
    IndexBuffer::INDEX_TYPE     SGL_DLLCALL CurrentIndexFormat() const                      { return currentIndexFormat; }
    const Program*              SGL_DLLCALL CurrentProgram() const                          { return currentProgram; }
    const RenderTarget*         SGL_DLLCALL CurrentRenderTarget() const                     { return currentRenderTarget; }

    void                        SGL_DLLCALL SetBlendState(const BlendState* blendState);
    void                        SGL_DLLCALL SetDepthStencilState(const DepthStencilState* depthStencilState);
    void                        SGL_DLLCALL SetRasterizerState(const RasterizerState* rasterizerState);
    void                        SGL_DLLCALL SetTexture(unsigned int stage, const Texture* texture); 
    void                        SGL_DLLCALL SetVertexBuffer(const VertexBuffer* vertexBuffer);
    void                        SGL_DLLCALL SetVertexLayout(const VertexLayout* vertexLayout);
    void                        SGL_DLLCALL SetIndexBuffer(const IndexBuffer* indexBuffer, IndexBuffer::INDEX_TYPE type);
    void                        SGL_DLLCALL SetProgram(const Program* program);
    void                        SGL_DLLCALL SetRenderTarget(const RenderTarget* renderTarget);

    // ============================ DRAW ============================ //
    void                SGL_DLLCALL Draw( PRIMITIVE_TYPE primType, 
                                          unsigned       firstVertex, 
                                          unsigned       numVertices ) const;

    void                SGL_DLLCALL DrawInstanced( PRIMITIVE_TYPE primType, 
                                                   unsigned       firstIndex, 
                                                   unsigned       numIndices,
                                                   unsigned       numInstances ) const;

    void                SGL_DLLCALL DrawIndexed( PRIMITIVE_TYPE primType,
                                                 unsigned       firstIndex,
                                                 unsigned       numIndices ) const;


    void                SGL_DLLCALL DrawIndexedInstanced( PRIMITIVE_TYPE primType, 
                                                          unsigned       firstIndex,
                                                          unsigned       numIndices,
                                                          unsigned       numInstances ) const;

    void                SGL_DLLCALL Clear(bool colorBuffer = true, bool depthBuffer = true, bool stencilBuffer = true) const;

    void                SGL_DLLCALL SwapBuffers() const;

    SGL_HRESULT         SGL_DLLCALL Sync() const;

    // ============================ STATES ============================ //

	void                SGL_DLLCALL PushState(State::TYPE type);
    SGL_HRESULT         SGL_DLLCALL PopState(State::TYPE type);

    // ============================ OTHER ============================ //

    void                SGL_DLLCALL SetClearColor(const math::Vector4f& color);
    void                SGL_DLLCALL SetClearDepthStencil(double depthClear, int stencilClear);
    math::Vector4f      SGL_DLLCALL ClearColor() const;
    double              SGL_DLLCALL ClearDepth() const;
    int                 SGL_DLLCALL ClearStencil() const;

    FFPProgram*         SGL_DLLCALL FixedPipelineProgram()          { return ffpProgram.get(); }
    DeviceTraits*       SGL_DLLCALL Traits() const                  { return deviceTraits.get(); }

    // ============================ RETRIEVE ============================ //

    SGL_HRESULT         SGL_DLLCALL CopyTexture2D( Texture2D*     texture,
                                                   unsigned       level,
                                                   unsigned       offsetx,
                                                   unsigned       offsety,
                                                   unsigned       width,
                                                   unsigned       height ) const;
protected:
	virtual ~GLDevice();

private:
    virtual SGL_HRESULT InitOpenGL();

protected:
    // current state
    const RenderTarget*     currentRenderTarget;
    const Program*          currentProgram;
    const IndexBuffer*      currentIndexBuffer;
    const VertexBuffer*     currentVertexBuffer;
    const VertexLayout*     currentVertexLayout;
    const Texture*          currentTexture[NUM_TEXTURE_STAGES];

    ref_ptr<const BlendState>          currentBlendState;
    ref_ptr<const DepthStencilState>   currentDepthStencilState;
    ref_ptr<const RasterizerState>     currentRasterizerState;

    IndexBuffer::INDEX_TYPE     currentIndexFormat;
    GLuint                      glIndexType;
    GLsizei                     glIndexSize;
    sgl::rectangle              viewport;

    // states
    state_stack					stateStack[State::__NUMBER_OF_STATES_WITH_SAMPLERS__];

    // unique device objects
    ref_ptr<FFPProgram>			ffpProgram;
    ref_ptr<DeviceTraits>		deviceTraits;

#ifdef WIN32
    HDC			hDC;
    HGLRC		hGLRC;
    HWND		hWnd;
#elif defined(__linux__)
    Display*    display;
    Window      window;
    GLXDrawable glxDrawable;
    GLXWindow   glxWindow;
    GLXContext  glxContext;
#endif

    // settings
    bool    makeCleanup;
};

template<DEVICE_VERSION DeviceVersion>
class GLDeviceConcrete :
	public GLDevice
{
public:
	GLDeviceConcrete();
	GLDeviceConcrete(const Device::VIDEO_DESC& desc);

	// ============================ TEXTURES ============================ //

	Image*              SGL_DLLCALL CreateImage();
	//Texture1D*          SGL_DLLCALL CreateTexture1D(const Texture::TEXTURE_1D_DESC& desc);
	//TextureBuffer*      SGL_DLLCALL CreateTextureBuffer(const Texture::TEXTURE_BUFFER_DESC& desc);
	Texture2D*          SGL_DLLCALL CreateTexture2D(const Texture2D::DESC& desc);
	Texture2D*          SGL_DLLCALL CreateTexture2DMS(const Texture2D::DESC_MS& desc);
	Texture3D*          SGL_DLLCALL CreateTexture3D(const Texture3D::DESC& desc);
	Texture3D*          SGL_DLLCALL CreateTexture3DMS(const Texture3D::DESC_MS& desc);
	TextureCube*        SGL_DLLCALL CreateTextureCube(const TextureCube::DESC& desc);

	// ============================ BUFFERS ============================ //

	VertexLayout*       SGL_DLLCALL CreateVertexLayout(unsigned int                 numElements, 
													   const VertexLayout::ELEMENT* elements);

	VertexBuffer*       SGL_DLLCALL CreateVertexBuffer();
	IndexBuffer*        SGL_DLLCALL CreateIndexBuffer();

	// ============================ STATES ============================ //

	BlendState*         SGL_DLLCALL CreateBlendState(const BlendState::DESC& desc);
	DepthStencilState*  SGL_DLLCALL CreateDepthStencilState(const DepthStencilState::DESC& desc);
	RasterizerState*    SGL_DLLCALL CreateRasterizerState(const RasterizerState::DESC& desc);
	SamplerState*       SGL_DLLCALL CreateSamplerState(const SamplerState::DESC& desc);

	// ============================ OTHER ============================ //

	Shader*             SGL_DLLCALL CreateShader(const Shader::DESC& desc);
	Program*            SGL_DLLCALL CreateProgram();
	Font*               SGL_DLLCALL CreateFont();
	RenderTarget*       SGL_DLLCALL CreateRenderTarget();
};

} // namesapce sgl

#ifdef _WIN32
#   pragma pop_macro("CreateFont")
#   pragma pop_macro("DEVICE_TYPE")
#endif

#endif // SIMPLE_GL_GL_DEVICE_H
