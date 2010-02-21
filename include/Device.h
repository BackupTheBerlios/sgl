#ifndef SIMPLE_GL_DEVICE_H
#define SIMPLE_GL_DEVICE_H

#include <memory>
#include <cassert>
#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "SamplerState.h"
#include "DeviceTraits.h"
#include "RenderTarget.h"
#include "State.h"
#include "Shader.h"
#include "Program.h"
#include "FFPProgram.h"
#include "Font.h"
#include "Image.h"
#include "Texture1D.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "TextureCube.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"

#ifdef WIN32
#   pragma push_macro("CreateFont")
#   undef CreateFont
#endif

// sgl - simple graphics library
namespace sgl {

/// Device type according to the library used for rendering
enum DEVICE_VERSION
{
    DV_OPENGL_1_4_FFP,
    DV_OPENGL_2_1_MIXED,
    DV_OPENGL_2_1_PROGRAMMABLE,
    DV_OPENGL_3_0_MIXED,
    DV_OPENGL_3_0_PROGRAMMABLE,
    DV_OPENGL_3_2
};

enum PIPELINE_TYPE
{
    PT_FIXED,
    PT_PROGRAMMABLE
};

template <DEVICE_VERSION deviceType>
struct device_traits;

template<>
struct device_traits<DV_OPENGL_2_1_MIXED>
{
    static const PIPELINE_TYPE pipeline = PT_PROGRAMMABLE;
};

template<>
struct device_traits<DV_OPENGL_2_1_PROGRAMMABLE>
{
    static const PIPELINE_TYPE pipeline = PT_PROGRAMMABLE;
};

/** Device class wraps graphics functions */
class Device :
    public Referenced
{
public:
    struct VIDEO_DESC
    {
        unsigned int    width;              /// window width
        unsigned int    height;             /// window height
        unsigned int    samples;            /// number of samples for multisampling 
        bool            fullscreen;         /// initialize in fullscreen mode
        bool            vsync;              /// wait for vsync
        Texture::FORMAT colorBufferFormat;  /// back buffer format
        Texture::FORMAT dsBufferFormat;     /// depth stencil buffer format

        VIDEO_DESC() :
            width(0),
            height(0),
            samples(0),
            fullscreen(false),
            vsync(false),
            colorBufferFormat(Texture::RGBA8),
            dsBufferFormat(Texture::D24S8)
        {}
    };

    enum 
    {
        NUM_FFP_LIGHTS      = 8,
        NUM_TEXTURE_STAGES  = 16
    };

public:


    /** Take screenshot of the device
     * @param image - image to store screenshot. Image is stored using back buffer format.
     */
    //virtual SGL_HRESULT SGL_DLLCALL TakeScreenshot(Image* image) const = 0;

    // ============================ STATE ============================ //
    
    /** Retrieve current device blend state. */
    virtual const BlendState*           SGL_DLLCALL CurrentBlendState() const = 0;

    /** Retrieve current device depth-stencil state. */
    virtual const DepthStencilState*    SGL_DLLCALL CurrentDepthStencilState() const = 0;

    /** Retrieve current device rasterizer state. */
    virtual const RasterizerState*      SGL_DLLCALL CurrentRasterizerState() const = 0;

    /** Retrieve current device sampler state. */
    virtual const SamplerState*         SGL_DLLCALL CurrentSamplerState(unsigned int stage) const = 0;

    /** Retrieve bound texture. */
    virtual const Texture*              SGL_DLLCALL CurrentTexture(unsigned int stage) const = 0;

    /** Retrieve bound vertex buffer. */
    virtual const VertexBuffer*         SGL_DLLCALL CurrentVertexBuffer() const = 0;

    /** Retrieve bound vertex layout.*/
    virtual const VertexLayout*         SGL_DLLCALL CurrentVertexLayout() const = 0;

    /** Retrieve bound index buffer. */
    virtual const IndexBuffer*          SGL_DLLCALL CurrentIndexBuffer() const = 0;

    /** Retrieve index type ofbound index buffer. */
    virtual IndexBuffer::INDEX_TYPE     SGL_DLLCALL CurrentIndexFormat() const = 0;

    /** Retrieve active shader program. */
    virtual const Program*              SGL_DLLCALL CurrentProgram() const = 0;

    /** Retrieve active render target. */
    virtual const RenderTarget*         SGL_DLLCALL CurrentRenderTarget() const = 0;

    // ============================ DRAW ============================ //

    /** Draw vertices. Doesn't perform any checks.
     * @param primType - type of the primitives.
     * @param firstVertex - offset in the vertex buffer to fetch vertices.
     * @param numVertices - number of vertices to draw.
     */
    virtual void            SGL_DLLCALL Draw( PRIMITIVE_TYPE primType, 
                                              unsigned       firstVertex, 
                                              unsigned       numVertices ) const = 0;

    /** Draw vertices indexed. Doesn't perform any checks. Semantic is the following:
     * \code
     * for(int i=0; i<primcount ; i++) {
     *     instanceID = i;
     *     Draw(primType, firstVertex, numVertices);
     * }
     * \uncode
     * @param primType - type of the primitives.
     * @param firstVertex - offset in the vertex buffer to fetch vertices.
     * @param numVertices - number of vertices to draw.
     * @param numInstances - number of instances to draw.
     */
    virtual void            SGL_DLLCALL DrawInstanced( PRIMITIVE_TYPE primType, 
                                                       unsigned       firstVertex, 
                                                       unsigned       numVertices,
                                                       unsigned       numInstances ) const = 0;

    /** Draw vertices indexed. Doesn't perform any checks.
     * @param primType - type of the primitives.
     * @param firstIndex - first index for primitive construction.
     * @param numIndices - number of indices for primitive construction.
     * @param numInstances - number of instances for drawing.
     */
    virtual void            SGL_DLLCALL DrawIndexed( PRIMITIVE_TYPE primType,
                                                     unsigned       firstIndex, 
                                                     unsigned       numIndices ) const = 0;


    /** Draw range of elements. Doesn't perform any checks. Semantic is the following:
     * \code
     * for(int i=0; i<primcount ; i++) {
     *     instanceID = i;
     *     Draw(primType, baseVertex, firstIndex, numIndices);
     * }
     * \uncode
     * @param primType - type of the primitives.
     * @param numIndices - number of indices for primitive construction.
     * @param numInstances - number if instances for drawing.
     */
    virtual void            SGL_DLLCALL DrawIndexedInstanced( PRIMITIVE_TYPE primType, 
                                                              unsigned       firstIndex,
                                                              unsigned       numIndices,
                                                              unsigned       numInstances ) const = 0;

    /** Clear framebuffer | depth buffer | stencil buffer. */
    virtual void            SGL_DLLCALL Clear(bool colorBuffer = true, bool depthBuffer = true, bool stencilBuffer = true) const = 0;

    /** Swap back & front buffers. */
    virtual void            SGL_DLLCALL SwapBuffers() const = 0;

    // ============================ TEXTURES ============================ //

    /** Create generic image */
    virtual Image*              SGL_DLLCALL CreateImage() = 0;

    /** Create texture buffer*/
    //virtual TextureBuffer*      SGL_DLLCALL CreateTextureBuffer(const Texture::TEXTURE_BUFFER_DESC& desc) = 0;
  
    /** Create 1d texture */
    //virtual Texture1D*          SGL_DLLCALL CreateTexture1D(const Texture::TEXTURE_1D_DESC& desc) = 0;

    /** Create 2d texture */
    virtual Texture2D*          SGL_DLLCALL CreateTexture2D(const Texture2D::DESC& desc) = 0;

    /** Create 2d multisampled texture */
    virtual Texture2D*          SGL_DLLCALL CreateTexture2DMS(const Texture2D::DESC_MS& desc) = 0;

    /** Create 3d texture */
    virtual Texture3D*          SGL_DLLCALL CreateTexture3D(const Texture3D::DESC& desc) = 0;

    /** Create 3d multisampled texture */
    virtual Texture3D*          SGL_DLLCALL CreateTexture3DMS(const Texture3D::DESC_MS& desc) = 0;

    /** Create cube map texture */
    virtual TextureCube*        SGL_DLLCALL CreateTextureCube(const TextureCube::DESC& desc) = 0;

    // ============================ BUFFERS ============================ //

    /** Create layout of vertex buffer. */
    virtual VertexLayout*       SGL_DLLCALL CreateVertexLayout( unsigned int                 numElements, 
                                                                const VertexLayout::ELEMENT* elements ) = 0;

    /** Create vertex buffer object. */
    virtual VertexBuffer*       SGL_DLLCALL CreateVertexBuffer() = 0;

    /** Create index buffer object. */
    virtual IndexBuffer*        SGL_DLLCALL CreateIndexBuffer() = 0;

    /** Create uniform buffer object. */
    //virtual UniformBuffer*      SGL_DLLCALL CreateUniformBuffer() = 0;

    // ============================ STATES ============================ //

    /** Create blend state */
    virtual BlendState*         SGL_DLLCALL CreateBlendState(const BlendState::DESC& desc) = 0;

    /** Create depth stencil state */
    virtual DepthStencilState*  SGL_DLLCALL CreateDepthStencilState(const DepthStencilState::DESC& desc) = 0;

    /** Create rasterizer state */
    virtual RasterizerState*    SGL_DLLCALL CreateRasterizerState(const RasterizerState::DESC& desc) = 0;

    /** Create sampler state */
    virtual SamplerState*       SGL_DLLCALL CreateSamplerState(const SamplerState::DESC& desc) = 0;

    /** Save state on the top of the state stack. If you push sampler state
     * then will be saved all 8 sampler states.
     * @param type - type of the state to save
     */
    virtual void                SGL_DLLCALL PushState(State::TYPE type) = 0;

    /** Pop state from the state stack. Could set SGLERR_INVALID_CALL if
     * state stack with states of the specified type is empty
     * @param type - type of the state that needed to be restored
     * @return result of the operation
     */
    virtual SGL_HRESULT         SGL_DLLCALL PopState(State::TYPE type) = 0;

    // ============================ OTHER ============================ //

    /** Set clear color for the framebuffer. */
    virtual void            SGL_DLLCALL SetClearColor(const math::Vector4f& color) = 0;

    /** Set clear depth and stencil. */
    virtual void            SGL_DLLCALL SetClearDepthStencil(double depthClear, int stencilClear) = 0;

    /** Get clear color of the framebuffer. */
    virtual math::Vector4f  SGL_DLLCALL ClearColor() const = 0;

    /** Get clear depth value. */
    virtual double          SGL_DLLCALL ClearDepth() const = 0;

    /** Get clear stencil value. */
    virtual int             SGL_DLLCALL ClearStencil() const = 0;

    /** Get viewport of the device */
    virtual rectangle       SGL_DLLCALL Viewport() const = 0;

    /** Set viewport of the device */
    virtual void            SGL_DLLCALL SetViewport(const rectangle& vp) = 0;

    /** Create font */
    virtual Font*           SGL_DLLCALL CreateFont() = 0;

    /** Create render target. */
    virtual RenderTarget*   SGL_DLLCALL CreateRenderTarget() = 0;

    /** Create shader. */
    virtual Shader*         SGL_DLLCALL CreateShader(const Shader::DESC& desc) = 0;

    /** Create shader program */
    virtual Program*        SGL_DLLCALL CreateProgram() = 0;

    /** Get program that uses(or emulates) fixed function pipeline */
    virtual FFPProgram*     SGL_DLLCALL FixedPipelineProgram() = 0;

    /** Get device traits */
    virtual DeviceTraits*   SGL_DLLCALL Traits() const = 0;

    /** Wait all commands sent to GPU to be executed.
     * @return SGL_INVALID_CALL if graphics mode is not set.
     */
    virtual SGL_HRESULT     SGL_DLLCALL Sync() const = 0;

    // ============================ RETRIEVE ============================ //

    /** Copy content of color attachment or depth stencil attachment ot the texture.
     * Content for copying is determined by the texture format, region for copying is
     * determined by the viewport.
     * @param texture - target texture.
     * @param level - texture mip level where to copy.
     * @param offsetx - x offset in the texture.
     * @param offsety - y offset in the texture.
     * @param width - width ofof the texture region where to copy.
     * @param height - height of the texture region where to copy.
     * @return result of the operation. Can be SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL CopyTexture2D( Texture2D*     texture,
                                                   unsigned       level,
                                                   unsigned       offsetx,
                                                   unsigned       offsety,
                                                   unsigned       width,
                                                   unsigned       height ) const = 0;
    virtual ~Device() {}
};

} // namespace sgl

/** Create device for rendering.
 * @param devType - device type(OPENGL_DEVICE).
 * @param desc - description of the device video mode.
 * @return fresh and hot device.
 */
extern "C" SGL_DLLEXPORT sgl::Device* SGL_DLLCALL sglCreateDevice(sgl::DEVICE_VERSION devType, const sgl::Device::VIDEO_DESC& desc);

/** Create device for rendering and grab context from current window.
 * @param devType - device type(OPENGL_DEVICE).
 * @return fresh and hot device.
 */
extern "C" SGL_DLLEXPORT sgl::Device* SGL_DLLCALL sglCreateDeviceFromCurrent(sgl::DEVICE_VERSION devType);

#endif // SIMPLE_GL_DEVICE_H
