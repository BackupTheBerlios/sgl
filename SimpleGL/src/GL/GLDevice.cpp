#include "GL/GLBlendState.h"
#include "GL/GLDepthStencilState.h"
#include "GL/GLDeviceTraits.h"
#include "GL/GLRasterizerState.h"
#include "GL/GLRenderTarget.h"
#include "GL/GLSamplerState.h"
#include "GL/GLDevice.h"
#include "GL/GLVertexLayout.h"
#include "GL/GLVertexBuffer.h"
#include "GL/GLIndexBuffer.h"
#include "GL/GLShader.h"
#include "GL/GLProgram.h"
#include "GL/GLFFPProgram.h"
#include "GL/GLTexture1D.h"
#include "GL/GLTexture2D.h"
#include "GL/GLTexture3D.h"
#include "GL/GLTextureCube.h"
#include "GL/GLVBORenderTarget.h"
#include "GL/GLFont.h"
#include "Utility/IlImage.h"
#include "Utility/IfThenElse.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#   include <GL/gl.h>
#   include <GL/wglew.h>

#   pragma push_macro("CreateFont")
#   undef CreateFont
#endif // WIN32

#if defined(__linux__) && !defined(__ANDROID__)
Bool wait_for_notify( Display* /*pDisplay*/,
					  XEvent*  pEvent,
					  XPointer arg )
{
	return (pEvent->type == MapNotify) && (pEvent->xmap.window == (Window) arg);
}
#endif // __linux__

#ifdef SIMPLE_GL_USE_DEVIL
#   include <IL/il.h>
#endif // SIMPLE_GL_USE_DEVIL

using namespace sgl;
using namespace std;
using namespace math;

/*
void GLDevice::CreateWindow( unsigned     width,
						    unsigned     height,
						    unsigned     bpp,
						    bool         fullscreen )
{
    GLuint		PixelFormat;			// Holds The Results After Searching For A Match
    WNDCLASS	wc;						// Windows Class Structure
    DWORD		dwExStyle;				// Window Extended Style
    DWORD		dwStyle;				// Window Style
    RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
    WindowRect.left=(long)0;			// Set Left Value To 0
    WindowRect.right=(long)width;		// Set Right Value To Requested Width
    WindowRect.top=(long)0;				// Set Top Value To 0
    WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

    hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window

    // Create The Window
    hWnd = CreateWindowEx( WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
                           "OpenGL",
                           "SimpleGL window",
                           WS_OVERLAPPEDWINDOW |
                           WS_CLIPSIBLINGS |
                           WS_CLIPCHILDREN,
                           0, 0,
                           width,
                           height,
                           NULL,
                           NULL,
                           hInstance,
                                      this)
    if ( !() )
    {
        Destroy();								// Reset The Display
        MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
        1,											// Version Number
        PFD_DRAW_TO_WINDOW |						// Format Must Support Window
        PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
        PFD_DOUBLEBUFFER,							// Must Support Double Buffering
        PFD_TYPE_RGBA,								// Request An RGBA Format
        bits,										// Select Our Color Depth
        0, 0, 0, 0, 0, 0,							// Color Bits Ignored
        0,											// No Alpha Buffer
        0,											// Shift Bit Ignored
        0,											// No Accumulation Buffer
        0, 0, 0, 0,									// Accumulation Bits Ignored
        16,											// 16Bit Z-Buffer (Depth Buffer)
        0,											// No Stencil Buffer
        0,											// No Auxiliary Buffer
        PFD_MAIN_PLANE,								// Main Drawing Layer
        0,											// Reserved
        0, 0, 0										// Layer Masks Ignored
    };

    if (!(m_hDC=GetDC(m_hWnd)))							// Did We Get A Device Context?
    {
        Destroy();								// Reset The Display
        MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    if (!(PixelFormat=ChoosePixelFormat(m_hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
    {
        Destroy();								// Reset The Display
        MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    if (!SetPixelFormat(m_hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
    {
        Destroy();								// Reset The Display
        MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    HGLRC tempContext = wglCreateContext(m_hDC);
    wglMakeCurrent(m_hDC,tempContext);
    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,//we want a 3.0 context
        WGL_CONTEXT_MINOR_VERSION_ARB, 0,
        //and it shall be forward compatible so that we can only use up to date functionality
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    }; //zero indicates the end of the array

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;	//pointer to the method
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
    if (wglCreateContextAttribsARB == NULL) //OpenGL 3.0 is not supported
    {
        MessageBox(NULL,"Cannot get Proc Adress for CreateContextAttribs", "ERROR",MB_OK);
        Destroy();
        wglDeleteContext(tempContext);
        return false;
    }

    if (!(m_hRC=wglCreateContextAttribsARB(m_hDC,0, attribs)))
    {
        wglDeleteContext(tempContext);
        Destroy();								// Reset The Display
        MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return false;								// Return false
    }
    wglDeleteContext(tempContext);

    if (!wglMakeCurrent(m_hDC,m_hRC))					// Try To Activate The Rendering Context
    {
        Destroy();								// Reset The Display
        MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    ShowWindow(m_hWnd,SW_SHOW);						// Show The Window
    SetForegroundWindow(m_hWnd);						// Slightly Higher Priority
    SetFocus(m_hWnd);									// Sets Keyboard Focus To The Window

    return true;									// Success
}
*/
/*
    // vSync
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, vSync);

    // misc
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// multisampling
	if ( multisample > 0 )
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multisample);
	}

    // create window
    unsigned flags = SDL_HWSURFACE | SDL_OPENGL;
    flags |= fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE;

    if ( !(screen = SDL_SetVideoMode(width, height, bpp, flags)) ) {
        return EInvalidCall( (string("Can't set video mode: ") + SDL_GetError()).c_str() );
    }
*/

GLDevice::GLDevice() :
    makeCleanup(false)
{
#ifdef SIMPLE_GL_USE_DEVIL
    // init image library
    static bool ilInitialized = false;
    if (!ilInitialized)
    {
        ilInit();
        ilInitialized = true;
    }
#endif // SIMPLE_GL_USE_DEVIL
#ifdef WIN32
    hDC = wglGetCurrentDC();
    if (!hDC)
    {
        sglSetError(SGLERR_INVALID_CALL, "Can't get current DC");
        throw gl_error("GLDevice::GLDevice() failed");
    }

    hGLRC = wglGetCurrentContext();
    if (!hGLRC)
    {
        sglSetError(SGLERR_INVALID_CALL, "Can't get current context");
        throw gl_error("GLDevice::GLDevice() failed");
    }

#elif defined(__linux__) && !defined(__ANDROID__)
    display = glXGetCurrentDisplay();
    if (!display)
    {
        sglSetError(SGLERR_INVALID_CALL, "Can't get current display");
        throw gl_error("GLDevice::GLDevice() failed");
    }

    glxContext = glXGetCurrentContext();
    if (!glxContext)
    {
        sglSetError(SGLERR_INVALID_CALL, "Can't get current context");
        throw gl_error("GLDevice::GLDevice() failed");
    }

    glxDrawable = glXGetCurrentDrawable();
    if (!glxDrawable)
    {
        sglSetError(SGLERR_INVALID_CALL, "Can't get current drawable");
        throw gl_error("GLDevice::GLDevice() failed");
    }

#endif

    InitOpenGL();
}

#ifndef __ANDROID__

GLDevice::GLDevice(const Device::VIDEO_DESC& desc)
{
#ifdef SIMPLE_GL_USE_DEVIL
    // init image library
    static bool ilInitialized = false;
    if (!ilInitialized)
    {
        ilInit();
        ilInitialized = true;
    }
#endif // SIMPLE_GL_USE_DEVIL
#ifdef WIN32

#elif defined(__linux__)
    // Open a connection to the X server
    display = XOpenDisplay(0);
    if (!display) {
        throw gl_error("Couldn't open display");
    }

    // Make sure OpenGL's GLX extension supported
    int errorBase;
	int eventBase;
    if( !glXQueryExtension( display, &errorBase, &eventBase ) ) {
        throw gl_error("X server has no OpenGL GLX extension");
    }

    // Find an appropriate visual
    int bufferAttributes32[]  =
    {
        GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,    GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER,   True,
        GLX_RED_SIZE,       8,
        GLX_GREEN_SIZE,     8,
        GLX_BLUE_SIZE,      8,
        GLX_ALPHA_SIZE,     8,
        GLX_DEPTH_SIZE,     24,
        GLX_STENCIL_SIZE,   8,
        None
    };

    int bufferAttributes24[]  =
    {
        GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,    GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER,   True,
        GLX_RED_SIZE,       8,
        GLX_GREEN_SIZE,     8,
        GLX_BLUE_SIZE,      8,
        GLX_DEPTH_SIZE,     24,
        GLX_STENCIL_SIZE,   8,
        None
    };

    int* bufferAttributes = bufferAttributes32;
    if ( Texture::FORMAT_TRAITS[desc.colorBufferFormat].sizeInBits == 24 ) {
        bufferAttributes = bufferAttributes24;
    }

    // Try for the double-bufferd visual first
    GLXFBConfig*    FBConfigs;
    GLXFBConfig     renderFBConfig;
    int             numFBConfigs;
	FBConfigs = glXChooseFBConfig( display,
			    				   DefaultScreen(display),
                                   bufferAttributes,
                                   &numFBConfigs );
    if (!FBConfigs) {
        throw gl_error("Couldn't get FB config with 24 bit depth buffer and 8 bit stencil buffer");
    }

    XVisualInfo* visualInfo = 0;
	for(int i = 0; i < numFBConfigs; i++)
	{
		visualInfo = glXGetVisualFromFBConfig(display, FBConfigs[i]);
        if (!visualInfo) {
			continue;
        }

        visualInfo     = glXGetVisualFromFBConfig(display, FBConfigs[i]);
        renderFBConfig = FBConfigs[i];
        break;
    }

    // Setup window
    XSetWindowAttributes windowAttributes;
    windowAttributes.colormap     = XCreateColormap( display,
                                                     RootWindow(display, visualInfo->screen),
                                                     visualInfo->visual,
                                                     AllocNone );
    windowAttributes.border_pixel = 0;
    windowAttributes.event_mask   = ExposureMask           |
                                    VisibilityChangeMask   |
                                    KeyPressMask           |
                                    KeyReleaseMask         |
                                    ButtonPressMask        |
                                    ButtonReleaseMask      |
                                    PointerMotionMask      |
                                    StructureNotifyMask    |
                                    SubstructureNotifyMask |
                                    FocusChangeMask;

    // Create an X window with the selected visual
    window = XCreateWindow( display,
                            RootWindow(display, visualInfo->screen),
                            0, 0,                                   // x/y position of top-left outside corner of the window
                            desc.width, desc.height,                // Width and height of window
                            0,                                      // Border width
                            visualInfo->depth,
                            InputOutput,
                            visualInfo->visual,
                            CWBorderPixel | CWColormap | CWEventMask,
                            &windowAttributes );

    // Create an OpenGL rendering context
	glxContext = glXCreateNewContext( display,
									  renderFBConfig,
									  GLX_RGBA_TYPE,
									  0,              // No sharing of display lists
									  GL_TRUE );      // Direct rendering if possible
    if (!glxContext) {
        throw gl_error("Couldn't create rendering context");
    }

	// make sure to get a GLX window with the fb-config with render-support
	glxWindow   = glXCreateWindow(display, renderFBConfig, window, 0);
	glxDrawable = glxWindow;

    // Request the X window to be displayed on the screen
    XMapWindow(display, window);

	// ... and wait for it to appear
	XEvent event;
	XIfEvent(display, &event, wait_for_notify, (XPointer) window);

    // Bind the rendering context to the window
    glXMakeContextCurrent(display, glxWindow, glxWindow, glxContext);

    makeCleanup = true;
#endif

    static bool glewInitialized = false;
    if (!glewInitialized)
    {
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            throw gl_error("Can't init glew.");
        }

        std::cout << "OpenGL version is " << glGetString(GL_VERSION) << endl;
        glewInitialized = true;
    }

    InitOpenGL();
}

#endif // !defined(__ANDROID__)

SGL_HRESULT GLDevice::InitOpenGL()
{
    // defaults
    currentRenderTarget         = 0;
    currentProgram              = 0;
    currentIndexBuffer          = 0;
    currentVertexBuffer         = 0;
    currentVertexLayout         = 0;

    std::fill( currentTexture, currentTexture + NUM_TEXTURE_STAGES, ref_ptr<const Texture>() );

    // create unqie objects
    deviceTraits.reset( new GLDeviceTraits(this) );
    assert( GL_NO_ERROR == glGetError() );

    // get viewport
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    viewport = rectangle(vp[0], vp[1], vp[2], vp[3]);

    // default props
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    assert( GL_NO_ERROR == glGetError() );
    return SGL_OK;
}

GLDevice::~GLDevice()
{
    // clean up
    if (makeCleanup)
    {
    #ifdef WIN32
        wglDeleteContext(hGLRC);
        wglMakeCurrent(0, 0);
        ReleaseDC(hWnd, hDC);
    #elif defined(__linux__) && !defined(__ANDROID__)
        // If sgl created window by itself
        glXDestroyContext(display, glxContext);
        XUnmapWindow(display, window);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
	#endif
    }
}

SGL_HRESULT GLDevice::Sync() const
{
    glFinish();
    return SGL_OK;
}

rectangle GLDevice::Viewport() const
{
    return viewport;
}

void GLDevice::SetViewport(const rectangle& vp)
{
    if (viewport != vp)
    {
        glViewport(vp.x, vp.y, vp.width, vp.height);
        viewport = vp;
    }
}

// internal binders
void GLDevice::SetBlendState(const BlendState* blendState)
{
    currentBlendState.reset(blendState);
}

void GLDevice::SetDepthStencilState(const DepthStencilState* depthStencilState)
{
    currentDepthStencilState.reset(depthStencilState);
}

void GLDevice::SetRasterizerState(const RasterizerState* rasterizerState)
{
    currentRasterizerState.reset(rasterizerState);
}

void GLDevice::SetTexture(unsigned int stage, const Texture* texture)
{
    assert(stage < NUM_TEXTURE_STAGES);
    currentTexture[stage] = texture;
}

void GLDevice::SetVertexBuffer(const VertexBuffer* vertexBuffer)
{
    currentVertexBuffer = vertexBuffer;
}

void GLDevice::SetVertexLayout(const VertexLayout* vertexLayout)
{
    currentVertexLayout = vertexLayout;
}

void GLDevice::SetIndexBuffer(const IndexBuffer* indexBuffer, IndexBuffer::INDEX_TYPE type)
{
    currentIndexBuffer = indexBuffer;
    currentIndexFormat = type;
    glIndexType        = BIND_GL_INDEX_TYPE[type];
    glIndexSize        = BIND_GL_INDEX_SIZE[type];
}

void GLDevice::SetProgram(const Program* program)
{
    currentProgram = program;
}

void GLDevice::SetRenderTarget(const RenderTarget* renderTarget)
{
    currentRenderTarget = renderTarget;
}

// ============================ DRAW ============================ //

void GLDevice::Draw( PRIMITIVE_TYPE primType,
                     unsigned       firstVertex,
                     unsigned       numVertices ) const
{
    glDrawArrays(BIND_PRIMITIVE_TYPE[primType], firstVertex, numVertices);
}

void GLDevice::DrawIndexed( PRIMITIVE_TYPE primType,
                            unsigned       firstIndex,
                            unsigned       numIndices ) const
{
    glDrawElements(BIND_PRIMITIVE_TYPE[primType], numIndices, glIndexType, (GLvoid*)(firstIndex * glIndexSize));
}

#ifndef SIMPLE_GL_ES
void GLDevice::DrawInstanced( PRIMITIVE_TYPE primType,
                              unsigned       firstVertex,
                              unsigned       numVertices,
                              unsigned       numInstances ) const
{
    glDrawArraysInstanced(BIND_PRIMITIVE_TYPE[primType], firstVertex, numVertices, numInstances);
}

void GLDevice::DrawIndexedInstanced( PRIMITIVE_TYPE primType,
                                     unsigned       firstIndex,
                                     unsigned       numIndices,
                                     unsigned       numInstances ) const
{
    glDrawElementsInstanced(BIND_PRIMITIVE_TYPE[primType], numIndices, glIndexType, (GLvoid*)(firstIndex * glIndexSize), numInstances);
}
#endif // defined(SIMPLE_GL_ES)

void GLDevice::Clear(bool colorBuffer, bool depthBuffer, bool stencilBuffer) const
{
    GLbitfield mask = (colorBuffer   ? GL_COLOR_BUFFER_BIT   : 0)
                    | (depthBuffer   ? GL_DEPTH_BUFFER_BIT   : 0)
                    | (stencilBuffer ? GL_STENCIL_BUFFER_BIT : 0);
    glClear(mask);
}

#ifndef __ANDROID__
void GLDevice::SwapBuffers() const
{
#ifdef WIN32
    ::SwapBuffers(hDC);
#elif defined(__linux__) && !defined(__ANDROID__)
    glXSwapBuffers(display, glxDrawable);
#endif

#if defined(DEBUG) && !defined(SGL_NO_STATUS_CHECK)
    GLenum error = glGetError();
    switch (error)
    {
    case GL_INVALID_ENUM:
        sglSetError(SGLERR_INVALID_CALL, "GL_INVALID_ENUM error detected in previous frame");
        break;

    case GL_INVALID_VALUE:
        sglSetError(SGLERR_INVALID_CALL, "GL_INVALID_VALUE error detected in previous frame");
        break;

    case GL_INVALID_OPERATION:
        sglSetError(SGLERR_INVALID_CALL, "GL_INVALID_OPERATION error detected in previous frame");
        break;

    case GL_STACK_OVERFLOW:
        sglSetError(SGLERR_INVALID_CALL, "GL_INVALID_ENUM error detected in previous frame");
        break;
    }
#endif 
    //assert( GL_NO_ERROR == glGetError() );
}
#endif // !defined(__ANDROID__)

void SGL_DLLCALL GLDevice::PushState(State::TYPE type)
{
    switch (type)
    {
        case State::BLEND_STATE:
        {
            assert(currentBlendState);
            stateStack[State::BLEND_STATE].push(currentBlendState);
            break;
        }

        case State::DEPTH_STENCIL_STATE:
        {
            assert(currentDepthStencilState);
            stateStack[State::DEPTH_STENCIL_STATE].push(currentDepthStencilState);
            break;
        }

        case State::RASTERIZER_STATE:
        {
            assert(currentRasterizerState);
            stateStack[State::RASTERIZER_STATE].push(currentRasterizerState);
            break;
        }

        default:
            break;
    }
}

SGL_HRESULT SGL_DLLCALL GLDevice::PopState(State::TYPE type)
{
#ifndef SGL_NO_STATUS_CHECK
    if ( stateStack[type].empty() ) {
        return EInvalidCall("State stack of the specified type is empty");
    }
#endif

    switch (type)
    {
        case State::BLEND_STATE:
        {
            static_cast<const BlendState&>( *stateStack[State::BLEND_STATE].top() ).Bind();
            stateStack[State::BLEND_STATE].pop();
            break;
        }

        case State::DEPTH_STENCIL_STATE:
        {
            static_cast<const DepthStencilState&>( *stateStack[State::DEPTH_STENCIL_STATE].top() ).Bind();
            stateStack[State::DEPTH_STENCIL_STATE].pop();
            break;
        }

        case State::RASTERIZER_STATE:
        {
            static_cast<const RasterizerState&>( *stateStack[State::RASTERIZER_STATE].top() ).Bind();
            stateStack[State::RASTERIZER_STATE].pop();
            break;
        }

        case State::SAMPLER_STATE:
        {
            return EInvalidCall("Can't bind SamplerState to device, it can be binded to the texture.");
            break;
        }

        default:
        {
            assert(!"Can't get here");
            break;
        }
    }

    return SGL_OK;
}

// ============================ OTHER ============================ //

void GLDevice::SetClearColor(const math::Vector4f& color)
{
    glClearColor(color.x, color.y, color.z, color.w);
}

void GLDevice::SetClearDepthStencil(double depthClear, int stencilClear)
{
    glClearDepthf(depthClear);
    glClearStencil(stencilClear);
}

math::Vector4f GLDevice::ClearColor() const
{
    math::Vector4f color;
    glGetFloatv(GL_COLOR_CLEAR_VALUE, &color[0]);
    return color;
}

double GLDevice::ClearDepth() const
{
    double depth;
#ifdef SIMPLE_GL_ES
    float depthF;
    glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depthF);
    depth = depthF;
#else
    glGetDoublev(GL_DEPTH_CLEAR_VALUE, &depth);
#endif
    return depth;
}

int GLDevice::ClearStencil() const
{
    int stencil;
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &stencil);
    return stencil;
}

SGL_HRESULT GLDevice::CopyTexture2D( Texture2D*     texture,
                                     unsigned       level,
                                     unsigned       offsetx,
                                     unsigned       offsety,
                                     unsigned       width,
                                     unsigned       height ) const
{
#ifndef SGL_NO_STATUS_CHECK
    if (!texture) {
        return EInvalidCall("GLDevice::CopyTexture2D failed. Texture is NULL.");
    }
#endif

    typedef GLTexture2D::guarded_binding     guarded_binding;
    typedef GLTexture2D::guarded_binding_ptr guarded_binding_ptr;

    GLTexture2D*		glTexture = static_cast<GLTexture2D*>(texture);
    guarded_binding_ptr texBinding( new guarded_binding(this, glTexture, 0) );
    {
        glCopyTexSubImage2D(glTexture->Target(), level, offsetx, offsety, 0, 0, width, height);

    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if ( error != GL_NO_ERROR ) {
            return CheckGLError("GLDevice::CopyTexture2D failed: ", error);
        }
    #endif
    }

    return SGL_OK;
}

/*
VBORenderTarget* SGL_DLLCALL GLDevice::CreateVBORenderTarget() const
{
    if ( deviceTraits->SupportsRenderToTexture() ) {
        return new GLVBORenderTarget( const_cast<GLDevice*>(this) );
    }
    sglSetError(SGLERR_UNSUPPORTED, "FBO is not supported by the device");
    return 0;
}
*/
namespace {

    template<bool toggle> struct support_fixed_pipeline         { static const bool value = toggle; };
    template<bool toggle> struct support_separate_blending      { static const bool value = toggle; };
    template<bool toggle> struct support_programmable_pipeline  { static const bool value = toggle; };
    template<bool toggle> struct support_generic_attributes     { static const bool value = toggle; };
    template<bool toggle> struct support_fixed_attributes       { static const bool value = toggle; };
    template<bool toggle> struct support_display_lists          { static const bool value = toggle; };
    template<bool toggle> struct support_render_target          { static const bool value = toggle; };
    template<bool toggle> struct support_buffer_copies          { static const bool value = toggle; };

    #define SUPPORT(Feature, DeviceVersion) support_##Feature<device_traits<DeviceVersion>::support_##Feature>()
	
    Shader* CreateShader(GLDevice*           /*device*/,
                         const Shader::DESC& /*desc*/,
                         support_programmable_pipeline<false>)
	{
        return 0;
    }

    Shader* CreateShader(GLDevice*           device,
                         const Shader::DESC& desc,
                         support_programmable_pipeline<true>)
    {
		return new GLShader(device, desc);
	}

    Program* CreateProgram(GLDevice* /*device*/,
                           support_programmable_pipeline<false>)
    {
        return 0;
    }

    Program* CreateProgram(GLDevice* device,
                           support_programmable_pipeline<true>)
	{
		return new GLProgram(device);
	}

	FFPProgram* CreateFFPProgram(GLDevice* device, support_fixed_pipeline<true>)
	{
		return new GLFFPProgram(device);
	}

    FFPProgram* CreateFFPProgram(GLDevice* /*device*/, support_fixed_pipeline<false>)
    {
		return 0;
	}

	sgl::Font* CreateFont(GLDevice* device,
						  support_programmable_pipeline<true>)
	{
		return new GLFontProgrammable(device);
	}

	sgl::Font* CreateFont(GLDevice* device,
						  support_programmable_pipeline<false>)
	{
		return new GLFontFixed(device);
	}

	Image* CreateImage(GLDevice* device)
	{
#ifdef SIMPLE_GL_USE_DEVIL
		return new IlImage(device);
#endif
		sglSetError(SGLERR_UNSUPPORTED, "SimpleGL compiled without DevIL. Can't create image.");
		return 0;
	}


	RenderTarget* CreateRenderTarget(GLDevice* device, support_render_target<true>)
	{
		return new GLRenderTarget(device);
	}

    RenderTarget* CreateRenderTarget(GLDevice* /*device*/, support_render_target<false>)
	{
		sglSetError(SGLERR_UNSUPPORTED, "Render targets are not supported");
		return 0;
	}

	/*
	Texture1D* SGL_DLLCALL GLDevice::CreateTexture1D() const
	{
		return new GLTexture1D( const_cast<GLDevice*>(this) );
	}
	*/

	Texture2D* CreateTexture2D(GLDevice* device, const Texture2D::DESC& desc)
	{
		return new GLTexture2D(device, desc);
	}

	Texture2D* CreateTexture2DMS(GLDevice* device, const Texture2D::DESC_MS& desc)
	{
    #ifdef SIMPLE_GL_ES
        return 0;
    #else
		return new GLTexture2D(device, desc);
    #endif
	}

	Texture3D* CreateTexture3D(GLDevice* device, const Texture3D::DESC& desc)
	{
    #ifdef SIMPLE_GL_ES
        return 0;
    #else
		return new GLTexture3D(device, desc);
    #endif
	}

	Texture3D* CreateTexture3DMS(GLDevice* device, const Texture3D::DESC_MS& desc)
	{
    #ifdef SIMPLE_GL_ES
        return 0;
    #else
		return new GLTexture3D(device, desc);
    #endif
	}

	TextureCube* CreateTextureCube(GLDevice* device, const TextureCube::DESC& desc)
	{
		return new GLTextureCube(device, desc);
	}

	// ============================ BUFFERS ============================ //

	VertexLayout* CreateVertexLayout(GLDevice*					  device, 
									 unsigned int                 numElements,
									 const VertexLayout::ELEMENT* elements,
									 support_fixed_attributes<true>)
	{
		return new GLVertexLayoutMixed(device, numElements, elements);
	}

	VertexLayout* CreateVertexLayout(GLDevice*						device, 
									 unsigned int					numElements,
									 const VertexLayout::ELEMENT*	elements,
									 support_fixed_attributes<false>)
	{
		return new GLVertexLayoutAttribute(device, numElements, elements);
	}

#ifdef SIMPLE_GL_ES
    VertexBuffer* CreateVertexBuffer(GLDevice* device,
                                     support_buffer_copies<false>)
    {
        return new GLVertexBuffer<GLBufferDefault<VertexBuffer> >(device);
    }

    IndexBuffer* CreateIndexBuffer(GLDevice* device,
                                   support_buffer_copies<false>)
    {
        return new GLIndexBuffer<GLBufferDefault<IndexBuffer> >(device);
    }
#else // !defined(SIMPLE_GL_ES)
    template<bool BufferCopies>
	VertexBuffer* CreateVertexBuffer(GLDevice* device,
                                     support_buffer_copies<BufferCopies>)
	{
        typedef typename if_then_else< BufferCopies,
                                       GLBufferModern<VertexBuffer>,
                                       GLBufferDefault<VertexBuffer> >::type buffer_impl;

		return new GLVertexBuffer<buffer_impl>(device);
	}

    template<bool BufferCopies>
    IndexBuffer* CreateIndexBuffer(GLDevice* device,
                                   support_buffer_copies<BufferCopies>)
	{
        typedef typename if_then_else< BufferCopies,
                                       GLBufferModern<IndexBuffer>,
                                       GLBufferDefault<IndexBuffer> >::type buffer_impl;

		return new GLIndexBuffer<buffer_impl>(device);
	}
#endif // !defined(SIMPLE_GL_ES)
	/*
	UniformBufferView* SGL_DLLCALL GLDevice::CreateUniformBuffer()
	{
		return new GLUniformBufferView( this, new GLBuffer(this) );
	}
	*/
	// ============================ STATES ============================ //

#ifndef SIMPLE_GL_ES
	BlendState* SGL_DLLCALL CreateBlendState(GLDevice*					device, 
											 const BlendState::DESC&	desc,
                                             support_display_lists<true>,
                                             support_separate_blending<false>)
	{
        if (    desc.destBlend != desc.destBlendAlpha
			 || desc.srcBlend != desc.srcBlendAlpha
			 || desc.blendOp != desc.blendOpAlpha )
		{
            sglSetError(SGLERR_UNSUPPORTED, "Separate blending is not supported");
            return 0;
		}

		return new GLBlendStateDisplayLists(device, desc);
	}

    BlendState* SGL_DLLCALL CreateBlendState(GLDevice*					device,
                                             const BlendState::DESC&	desc,
                                             support_display_lists<true>,
                                             support_separate_blending<true>)
    {
        return new GLBlendStateDisplayLists(device, desc);
    }
#endif // !defined(SIMPLE_GL_ES)

	BlendState* SGL_DLLCALL CreateBlendState(GLDevice*					device, 
											 const BlendState::DESC&	desc,
                                             support_display_lists<false>,
                                             support_separate_blending<false>)
	{
        if ( desc.destBlend  != desc.destBlendAlpha
			|| desc.srcBlend != desc.srcBlendAlpha
            || desc.blendOp  != desc.blendOpAlpha )
		{
            sglSetError(SGLERR_UNSUPPORTED, "Separate blending is not supported");
            return 0;
		}

		return new GLBlendStateDefault(device, desc);
	}

    BlendState* SGL_DLLCALL CreateBlendState(GLDevice*					device,
                                             const BlendState::DESC&	desc,
                                             support_display_lists<false>,
                                             support_separate_blending<true>)
    {
        if (   desc.destBlend != desc.destBlendAlpha
            || desc.srcBlend  != desc.srcBlendAlpha
            || desc.blendOp   != desc.blendOpAlpha )
        {
            return new GLBlendStateSeparate(device, desc);
        }

        return new GLBlendStateDefault(device, desc);
    }

#ifndef SIMPLE_GL_ES
	DepthStencilState* CreateDepthStencilState(GLDevice* device, 
											   const DepthStencilState::DESC& desc,
											   support_display_lists<true>)
	{
		return new GLDepthStencilStateDisplayLists(device, desc);
	}
#endif

	DepthStencilState* CreateDepthStencilState(GLDevice* device, 
											   const DepthStencilState::DESC& desc,
											   support_display_lists<false>)
	{
		return new GLDepthStencilStateSeparate(device, desc);
	}

	RasterizerState* SGL_DLLCALL CreateRasterizerState(GLDevice* device, const RasterizerState::DESC& desc)
	{
		return new GLRasterizerState(device, desc);
	}

	SamplerState* SGL_DLLCALL CreateSamplerState(GLDevice* device, const SamplerState::DESC& desc)
	{
		return new GLSamplerState(device, desc);
	}

} // anonymous namespace

namespace sgl {
		
template<DEVICE_VERSION DeviceVersion>
GLDeviceConcrete<DeviceVersion>::GLDeviceConcrete()
{
	// create blend state
	{
		BlendState::DESC_SIMPLE desc;
		desc.blendEnable = false;

		BlendState* blendState = this->CreateBlendState(Extend(desc));
		blendState->Bind();
	}
	assert( GL_NO_ERROR == glGetError() );

	// create depth stencil state
	{
		DepthStencilState::DESC desc;
		desc.depthEnable    = true;
		desc.depthFunc      = DepthStencilState::LEQUAL;
		desc.depthWriteMask = true;
		desc.stencilEnable  = false;

		DepthStencilState* depthStencilState = this->CreateDepthStencilState(desc);
		depthStencilState->Bind();
	}
	assert( GL_NO_ERROR == glGetError() );

	// create rasterizer state
	{
		RasterizerState::DESC desc;
		desc.cullMode      = RasterizerState::BACK;
		desc.fillMode      = RasterizerState::SOLID;

		RasterizerState* rasterizerState = this->CreateRasterizerState(desc);
		rasterizerState->Bind();
	}
	assert( GL_NO_ERROR == glGetError() );

    ffpProgram.reset( CreateFFPProgram( this, SUPPORT(fixed_pipeline, DeviceVersion) ) );
    assert( GL_NO_ERROR == glGetError() );
}

#ifndef __ANDROID__
template<DEVICE_VERSION DeviceVersion>
GLDeviceConcrete<DeviceVersion>::GLDeviceConcrete(const Device::VIDEO_DESC& desc)
:	GLDevice(desc)
{
	// create blend state
	{
		BlendState::DESC_SIMPLE desc;
		desc.blendEnable = false;

		BlendState* blendState = this->CreateBlendState(Extend(desc));
		blendState->Bind();
	}
	assert( GL_NO_ERROR == glGetError() );

	// create depth stencil state
	{
		DepthStencilState::DESC desc;
		desc.depthEnable    = true;
		desc.depthFunc      = DepthStencilState::LEQUAL;
		desc.depthWriteMask = true;
		desc.stencilEnable  = false;

		DepthStencilState* depthStencilState = this->CreateDepthStencilState(desc);
		depthStencilState->Bind();
	}
	assert( GL_NO_ERROR == glGetError() );

	// create rasterizer state
	{
		RasterizerState::DESC desc;
		desc.cullMode      = RasterizerState::BACK;
		desc.fillMode      = RasterizerState::SOLID;

		RasterizerState* rasterizerState = this->CreateRasterizerState(desc);
		rasterizerState->Bind();
	}
	assert( GL_NO_ERROR == glGetError() );

	ffpProgram.reset( CreateFFPProgram( this, SUPPORT(fixed_pipeline, DeviceVersion) ) );
	assert( GL_NO_ERROR == glGetError() );
}
#endif // !defined(__ANDROID__)

// ============================ TEXTURES ============================ //

template<DEVICE_VERSION DeviceVersion>
Image* GLDeviceConcrete<DeviceVersion>::CreateImage()
{
	return ::CreateImage(this);
}

//Texture1D*          SGL_DLLCALL CreateTexture1D(const Texture::TEXTURE_1D_DESC& desc);
//TextureBuffer*      SGL_DLLCALL CreateTextureBuffer(const Texture::TEXTURE_BUFFER_DESC& desc);
template<DEVICE_VERSION DeviceVersion>
Texture2D* GLDeviceConcrete<DeviceVersion>::CreateTexture2D(const Texture2D::DESC& desc)
{
	return ::CreateTexture2D(this, desc);
}

template<DEVICE_VERSION DeviceVersion>
Texture2D* GLDeviceConcrete<DeviceVersion>::CreateTexture2DMS(const Texture2D::DESC_MS& desc)
{
	return ::CreateTexture2DMS(this, desc);
}

template<DEVICE_VERSION DeviceVersion>
Texture3D* GLDeviceConcrete<DeviceVersion>::CreateTexture3D(const Texture3D::DESC& desc)
{
	return ::CreateTexture3D(this, desc);
}

template<DEVICE_VERSION DeviceVersion>
Texture3D* GLDeviceConcrete<DeviceVersion>::CreateTexture3DMS(const Texture3D::DESC_MS& desc)
{
	return ::CreateTexture3DMS(this, desc);
}

template<DEVICE_VERSION DeviceVersion>
TextureCube* GLDeviceConcrete<DeviceVersion>::CreateTextureCube(const TextureCube::DESC& desc)
{
	return ::CreateTextureCube(this, desc);
}

// ============================ BUFFERS ============================ //

template<DEVICE_VERSION DeviceVersion>
VertexLayout* GLDeviceConcrete<DeviceVersion>::CreateVertexLayout(unsigned int                 numElements, 
																  const VertexLayout::ELEMENT* elements)
{
	return ::CreateVertexLayout( this, numElements, elements, SUPPORT(fixed_attributes, DeviceVersion) );
}

template<DEVICE_VERSION DeviceVersion>
VertexBuffer* GLDeviceConcrete<DeviceVersion>::CreateVertexBuffer()
{
	return ::CreateVertexBuffer( this, SUPPORT(buffer_copies, DeviceVersion) );
}

template<DEVICE_VERSION DeviceVersion>
IndexBuffer* GLDeviceConcrete<DeviceVersion>::CreateIndexBuffer()
{
	return ::CreateIndexBuffer( this, SUPPORT(buffer_copies, DeviceVersion) );
}

// ============================ STATES ============================ //

template<DEVICE_VERSION DeviceVersion>
BlendState* GLDeviceConcrete<DeviceVersion>::CreateBlendState(const BlendState::DESC& desc)
{
    return ::CreateBlendState( this,
                               desc,
                               SUPPORT(display_lists, DeviceVersion),
                               SUPPORT(separate_blending, DeviceVersion) );
}

template<DEVICE_VERSION DeviceVersion>
DepthStencilState* GLDeviceConcrete<DeviceVersion>::CreateDepthStencilState(const DepthStencilState::DESC& desc)
{
	return ::CreateDepthStencilState( this, desc, SUPPORT(display_lists, DeviceVersion) );
}

template<DEVICE_VERSION DeviceVersion>
RasterizerState* GLDeviceConcrete<DeviceVersion>::CreateRasterizerState(const RasterizerState::DESC& desc)
{
	return ::CreateRasterizerState(this, desc);
}

template<DEVICE_VERSION DeviceVersion>
SamplerState* GLDeviceConcrete<DeviceVersion>::CreateSamplerState(const SamplerState::DESC& desc)
{
	return ::CreateSamplerState(this, desc);
}

// ============================ OTHER ============================ //

template<DEVICE_VERSION DeviceVersion>
Shader* GLDeviceConcrete<DeviceVersion>::CreateShader(const Shader::DESC& desc)
{
    return ::CreateShader( this, desc, SUPPORT(programmable_pipeline, DeviceVersion) );
}

template<DEVICE_VERSION DeviceVersion>
Program* GLDeviceConcrete<DeviceVersion>::CreateProgram()
{
    return ::CreateProgram( this, SUPPORT(programmable_pipeline, DeviceVersion) );
}

template<DEVICE_VERSION DeviceVersion>
Font* GLDeviceConcrete<DeviceVersion>::CreateFont()
{
	return ::CreateFont( this, SUPPORT(programmable_pipeline, DeviceVersion) );
}

template<DEVICE_VERSION DeviceVersion>
RenderTarget* GLDeviceConcrete<DeviceVersion>::CreateRenderTarget()
{
	return ::CreateRenderTarget( this, SUPPORT(render_target, DeviceVersion) );
}

#undef SUPPORT

// explicit template instantiation
#ifndef SIMPLE_GL_ES
    template class GLDeviceConcrete<DV_OPENGL_1_3>;
    template class GLDeviceConcrete<DV_OPENGL_1_4>;
    template class GLDeviceConcrete<DV_OPENGL_1_5>;
    template class GLDeviceConcrete<DV_OPENGL_2_0>;
    template class GLDeviceConcrete<DV_OPENGL_2_1>;
    template class GLDeviceConcrete<DV_OPENGL_3_0>;
    template class GLDeviceConcrete<DV_OPENGL_3_1>;
    template class GLDeviceConcrete<DV_OPENGL_3_2>;
#else
    template class GLDeviceConcrete<DV_OPENGL_ES_1_0>;
    template class GLDeviceConcrete<DV_OPENGL_ES_1_1>;
    template class GLDeviceConcrete<DV_OPENGL_ES_2_0>;
#endif

} // namespace sgl

#ifdef _WIN32
#pragma pop_macro("CreateFont")
#endif
