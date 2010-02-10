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
#include <iostream>
#include <string>

#ifdef _WIN32
#   include <GL/gl.h>
#   include <GL/wglew.h>

#   pragma push_macro("CreateFont")
#   pragma push_macro("DEVICE_TYPE")
#   undef CreateFont
#   undef DEVICE_TYPE
#endif // WIN32

#ifdef __linux__
Bool wait_for_notify( Display* pDisplay,
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

#elif __linux__
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

#elif __linux__
    // Open a connection to the X server
    display = XOpenDisplay(0);
    if (!display) {
        return EUnknown("Couldn't open display");
    }

    // Make sure OpenGL's GLX extension supported
    int errorBase;
	int eventBase;
    if( !glXQueryExtension( display, &errorBase, &eventBase ) ) {
        return EUnsupported("X server has no OpenGL GLX extension");
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
    if ( bpp == 24 ) {
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
        return EUnsupported("Couldn't get FB config with 24 bit depth buffer and 8 bit stencil buffer");
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
                            width, height,                          // Width and height of window
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
        return EUnsupported("Couldn't create rendering context");
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

    InitOpenGL();
}

SGL_HRESULT GLDevice::InitOpenGL()
{
    // init extensions
    static bool glewInitialized = false;
    if (!glewInitialized)
    {
        GLenum err = glewInit();
        if (GLEW_OK != err) 
        {
            sglSetError(SGLERR_UNKNOWN, "Can't init glew.");
            throw gl_error("GLDevice::InitOpenGL() failed");
        }

        glewInitialized = true;
    }

    cerr << "OpenGL version is " << glGetString(GL_VERSION) << endl;

    // default settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    // create blend state
    {
        BlendState::DESC_SIMPLE desc;
        desc.blendEnable = false;

        BlendState* blendState = new GLBlendState(this, Extend(desc));
        blendState->Bind();
    }

    // create depth stencil state
    {
        DepthStencilState::DESC desc;
        desc.depthEnable    = true;
        desc.depthFunc      = DepthStencilState::LEQUAL;
        desc.depthWriteMask = true;
        desc.stencilEnable  = false;

        DepthStencilState* depthStencilState = new GLDepthStencilState(this, desc);
        depthStencilState->Bind();
    }

    // create rasterizer state
    {
        RasterizerState::DESC desc;
        desc.cullMode      = RasterizerState::BACK;
        desc.fillMode      = RasterizerState::SOLID;
        desc.scissorEnable = false;

        RasterizerState* rasterizerState = new GLRasterizerState(this, desc);
        rasterizerState->Bind();
    }

    // create sampler states
    {
        SamplerState::DESC desc;
        desc.filter[0] = SamplerState::NEAREST;
        desc.filter[1] = SamplerState::NEAREST;
        desc.filter[2] = SamplerState::NEAREST;

        desc.wrapping[0] = SamplerState::CLAMP;
        desc.wrapping[1] = SamplerState::CLAMP;
        desc.wrapping[2] = SamplerState::CLAMP;

        GLSamplerState* samplerState = new GLSamplerState(this, desc);
        for(int stage = 0; stage<NUM_TEXTURE_STAGES; ++stage) {
            samplerState->Bind(stage);
        }
    }

    // create unqie objects
    deviceTraits.reset( new GLDeviceTraits(this) );
    ffpProgram.reset( new GLFFPProgram(this) );

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
    #elif __linux__
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

// misc
SGL_HRESULT GLDevice::TakeScreenshot(Image* image) const
{
#ifndef SGL_NO_STATUS_CHECK
    if (!image) {
        return EInvalidCall("GLDevice::TakeScreenshot failed. Image may not be NULL.");
    }
#endif

    // viewport
    rectangle vp = Viewport();
/*
    // get data
    SGL_HRESULT result = image->SetImage(Texture::RGB8, vp.width, vp.height, 1, 0);
    if ( result != SGL_OK ) {
        return result;
    }
    glReadBuffer(GL_BACK);
    glReadPixels( vp.x, vp.y, vp.width, vp.height, GL_RGB, GL_UNSIGNED_BYTE, image->Data() );
*/
    return SGL_OK;
}

// create
Shader* SGL_DLLCALL GLDevice::CreateShader(const Shader::DESC& desc)
{
    try 
    {
        return new GLShader( const_cast<GLDevice*>(this), desc );
    }
    catch(gl_error& err) 
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

Program* SGL_DLLCALL GLDevice::CreateProgram()
{
    try 
    {
        return new GLProgram( const_cast<GLDevice*>(this) );
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

sgl::Font* SGL_DLLCALL GLDevice::CreateFont()
{
    return new GLFont( const_cast<GLDevice*>(this) );
}

Image* GLDevice::CreateImage()
{
#ifdef SIMPLE_GL_USE_DEVIL
    return new IlImage( const_cast<GLDevice*>(this) );
#endif
    sglSetError(SGLERR_UNSUPPORTED, "SimpleGL compiled without DevIL. Can't create image."); 
    return 0;
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

void GLDevice::SetSamplerState(unsigned int stage, const SamplerState* samplerState)
{
    assert(stage < NUM_TEXTURE_STAGES);
    if (currentSamplerState[stage] != samplerState && currentTexture[stage]) 
    {
        // rebind state to the texture
        currentSamplerState[stage].reset(samplerState);
        currentTexture[stage]->Bind(stage);
    }
    else {
        currentSamplerState[stage].reset(samplerState);
    }
}

void GLDevice::SetTexture(unsigned int stage, const Texture* texture)
{
    assert(stage < NUM_TEXTURE_STAGES);
    currentTexture[stage].reset(texture);
}

void GLDevice::SetVertexBuffer(const VertexBuffer* vertexBuffer)
{
    currentVertexBuffer.reset(vertexBuffer);
}

void GLDevice::SetVertexLayout(const VertexLayout* vertexLayout)
{
    currentVertexLayout.reset(vertexLayout);
}

void GLDevice::SetIndexBuffer(const IndexBuffer* indexBuffer, IndexBuffer::INDEX_TYPE type)
{
    currentIndexBuffer.reset(indexBuffer);
    currentIndexFormat = type;
    glIndexType        = BIND_GL_INDEX_TYPE[type];
    glIndexSize        = BIND_GL_INDEX_SIZE[type];
}

void GLDevice::SetProgram(const Program* program)
{
    currentProgram.reset(program);
}

void GLDevice::SetRenderTarget(const RenderTarget* renderTarget)
{
    currentRenderTarget.reset(renderTarget);
}

// ============================ DRAW ============================ //

void GLDevice::Draw( PRIMITIVE_TYPE primType, 
                     unsigned       firstVertex, 
                     unsigned       numVertices ) const
{
    glDrawArrays(BIND_PRIMITIVE_TYPE[primType], firstVertex, numVertices);
}

void GLDevice::DrawInstanced( PRIMITIVE_TYPE primType, 
                              unsigned       firstVertex, 
                              unsigned       numVertices,
                              unsigned       numInstances ) const
{
    glDrawArraysInstancedEXT(BIND_PRIMITIVE_TYPE[primType], firstVertex, numVertices, numInstances);
}

void GLDevice::DrawIndexed( PRIMITIVE_TYPE primType,
                            unsigned       firstIndex,
                            unsigned       numIndices ) const
{
    glDrawElements(BIND_PRIMITIVE_TYPE[primType], numIndices, glIndexType, (GLvoid*)(firstIndex * glIndexSize));
}

void GLDevice::DrawIndexedInstanced( PRIMITIVE_TYPE primType, 
                                     unsigned       firstIndex,
                                     unsigned       numIndices,
                                     unsigned       numInstances ) const
{
    glDrawElementsInstanced(BIND_PRIMITIVE_TYPE[primType], numIndices, glIndexType, (GLvoid*)(firstIndex * glIndexSize), numInstances);
}

void GLDevice::Clear(bool colorBuffer, bool depthBuffer, bool stencilBuffer) const
{
    GLbitfield mask = (colorBuffer   ? GL_COLOR_BUFFER_BIT   : 0)
                    | (depthBuffer   ? GL_DEPTH_BUFFER_BIT   : 0)
                    | (stencilBuffer ? GL_STENCIL_BUFFER_BIT : 0);
    glClear(mask);;
}

void GLDevice::SwapBuffers() const
{
#ifdef WIN32
    ::SwapBuffers(hDC);
#elif __linux__
    glXSwapBuffers(display, glxDrawable);
#endif

    //assert( GL_NO_ERROR == glGetError() );
}

// ============================ TEXTURES ============================ //
/*
Texture1D* SGL_DLLCALL GLDevice::CreateTexture1D() const
{
    return new GLTexture1D( const_cast<GLDevice*>(this) );
}
*/
Texture2D* SGL_DLLCALL GLDevice::CreateTexture2D(const Texture2D::DESC& desc)
{
    try
    {
        return new GLTexture2D(this, desc);
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

Texture2D* SGL_DLLCALL GLDevice::CreateTexture2DMS(const Texture2D::DESC_MS& desc)
{
    try
    {
        return new GLTexture2D(this, desc);
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

Texture3D* SGL_DLLCALL GLDevice::CreateTexture3D(const Texture3D::DESC& desc)
{
    try
    {
        return new GLTexture3D(this, desc);
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

Texture3D* SGL_DLLCALL GLDevice::CreateTexture3DMS(const Texture3D::DESC_MS& desc)
{
    try
    {
        return new GLTexture3D(this, desc);
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

TextureCube* SGL_DLLCALL GLDevice::CreateTextureCube(const TextureCube::DESC& desc)
{
    try
    {
        return new GLTextureCube(this, desc);
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

// ============================ BUFFERS ============================ //

VertexLayout* SGL_DLLCALL GLDevice::CreateVertexLayout(unsigned int                 numElements, 
                                                       const VertexLayout::ELEMENT* elements)
{
	return new GLVertexLayout(this, numElements, elements);
}

VertexBuffer* SGL_DLLCALL GLDevice::CreateVertexBuffer()
{
    return new GLVertexBuffer(this);
}

IndexBuffer* SGL_DLLCALL GLDevice::CreateIndexBuffer()
{
    return new GLIndexBuffer(this);
}
/*
UniformBufferView* SGL_DLLCALL GLDevice::CreateUniformBuffer()
{
    return new GLUniformBufferView( this, new GLBuffer(this) );
}
*/
// ============================ STATES ============================ //

BlendState* SGL_DLLCALL GLDevice::CreateBlendState(const BlendState::DESC& desc)
{
    return new GLBlendState(this, desc);
}

DepthStencilState* SGL_DLLCALL GLDevice::CreateDepthStencilState(const DepthStencilState::DESC& desc)
{
    return new GLDepthStencilState(this, desc);
}

RasterizerState* SGL_DLLCALL GLDevice::CreateRasterizerState(const RasterizerState::DESC& desc)
{
    return new GLRasterizerState(this, desc);
}

SamplerState* SGL_DLLCALL GLDevice::CreateSamplerState(const SamplerState::DESC& desc)
{
    return new GLSamplerState(this, desc);
}

void SGL_DLLCALL GLDevice::PushState(State::TYPE type)
{
    switch (type)
    {
        case State::BLEND_STATE:
        {
            BlendState* state = new GLBlendState( this, static_cast<const GLBlendState&>(*currentBlendState).Desc() );
            stateStack[State::BLEND_STATE].push( ref_ptr<BlendState>(state) );
            break;
        }

        case State::DEPTH_STENCIL_STATE:
        {
            DepthStencilState* state = new GLDepthStencilState( this, static_cast<const GLDepthStencilState&>(*currentDepthStencilState).Desc() );
            stateStack[State::DEPTH_STENCIL_STATE].push( ref_ptr<DepthStencilState>(state) );
            break;
        }

        case State::RASTERIZER_STATE:
        {
            RasterizerState* state = new GLRasterizerState( this, static_cast<const GLRasterizerState&>(*currentRasterizerState).Desc() );
            stateStack[State::RASTERIZER_STATE].push( ref_ptr<RasterizerState>(state) );
            break;
        }

        case State::SAMPLER_STATE:
        {
            for(size_t i = 0; i<8; ++i) 
            {
                SamplerState* state = new GLSamplerState( this, currentSamplerState[i]->Desc() );
                stateStack[State::SAMPLER_STATE + i].push( ref_ptr<SamplerState>(state) );
            }
            break;
        }

        default:
        {
            assert(!"Can't get here");
            break;
        }
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
            static_cast<GLBlendState&>( *stateStack[State::BLEND_STATE].top() ).Bind();
            stateStack[State::BLEND_STATE].pop();
            break;
        }

        case State::DEPTH_STENCIL_STATE:
        {
            static_cast<GLDepthStencilState&>( *stateStack[State::DEPTH_STENCIL_STATE].top() ).Bind();
            stateStack[State::DEPTH_STENCIL_STATE].pop();
            break;
        }

        case State::RASTERIZER_STATE:
        {
            static_cast<GLRasterizerState&>( *stateStack[State::RASTERIZER_STATE].top() ).Bind();
            stateStack[State::RASTERIZER_STATE].pop();
            break;
        }

        case State::SAMPLER_STATE:
        {
            for(size_t i = 0; i<8; ++i) 
            {
                static_cast<GLSamplerState&>( *stateStack[State::SAMPLER_STATE + i].top() ).Bind(i);
                stateStack[State::SAMPLER_STATE + i].pop();
            }
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
    glClearDepth(depthClear);
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
    glGetDoublev(GL_DEPTH_CLEAR_VALUE, &depth);
    return depth;
}

int GLDevice::ClearStencil() const
{
    int stencil;
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &stencil);
    return stencil;
}


RenderTarget* SGL_DLLCALL GLDevice::CreateRenderTarget()
{
    return new GLRenderTarget(this);
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

#ifdef _WIN32
#pragma pop_macro("CreateFont")
#pragma pop_macro("DEVICE_TYPE")
#endif
