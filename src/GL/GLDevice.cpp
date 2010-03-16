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
#   undef CreateFont
#endif // WIN32

#ifdef __linux__
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
template<DEVICE_VERSION deviceVersion>
GLDevice<deviceVersion>::GLDevice() :
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

template<DEVICE_VERSION deviceVersion>
GLDevice<deviceVersion>::GLDevice(const Device::VIDEO_DESC& desc)
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

    InitOpenGL();
}

template<DEVICE_VERSION deviceVersion>
SGL_HRESULT GLDevice<deviceVersion>::InitOpenGL()
{
    // defaults
    currentRenderTarget         = 0;
    currentProgram              = 0;
    currentIndexBuffer          = 0;
    currentVertexBuffer         = 0;
    currentVertexLayout         = 0;
    currentBlendState           = 0;
    currentDepthStencilState    = 0;
    currentRasterizerState      = 0;

    std::fill(currentTexture,      currentTexture      + NUM_TEXTURE_STAGES, (Texture*)0);
    std::fill(currentSamplerState, currentSamplerState + NUM_TEXTURE_STAGES, (SamplerState*)0);

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

template<DEVICE_VERSION deviceVersion>
GLDevice<deviceVersion>::~GLDevice()
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

template<DEVICE_VERSION deviceVersion>
SGL_HRESULT GLDevice<deviceVersion>::Sync() const
{
    glFinish();
    return SGL_OK;
}

template<DEVICE_VERSION deviceVersion>
rectangle GLDevice<deviceVersion>::Viewport() const
{
    return viewport;
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetViewport(const rectangle& vp)
{
    if (viewport != vp)
    {
        glViewport(vp.x, vp.y, vp.width, vp.height);
        viewport = vp;
    }
}

// misc
template<DEVICE_VERSION deviceVersion>
SGL_HRESULT GLDevice<deviceVersion>::TakeScreenshot(Image* image) const
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
template<DEVICE_VERSION deviceVersion>
Shader* GLDevice<deviceVersion>::CreateShader(const Shader::DESC& desc)
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

template<DEVICE_VERSION deviceVersion>
Program* GLDevice<deviceVersion>::CreateProgram()
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

template<DEVICE_VERSION deviceVersion>
sgl::Font* GLDevice<deviceVersion>::CreateFont()
{
    return new GLFont< device_traits<deviceVersion>::pipeline >( const_cast<GLDevice*>(this) );
}

template<DEVICE_VERSION deviceVersion>
Image* GLDevice<deviceVersion>::CreateImage()
{
#ifdef SIMPLE_GL_USE_DEVIL
    return new IlImage( const_cast<GLDevice*>(this) );
#endif
    sglSetError(SGLERR_UNSUPPORTED, "SimpleGL compiled without DevIL. Can't create image.");
    return 0;
}

// internal binders
template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetBlendState(const BlendState* blendState)
{
    currentBlendState = blendState;
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetDepthStencilState(const DepthStencilState* depthStencilState)
{
    currentDepthStencilState = depthStencilState;
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetRasterizerState(const RasterizerState* rasterizerState)
{
    currentRasterizerState = rasterizerState;
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetSamplerState(unsigned int stage, const SamplerState* samplerState)
{
    assert(stage < NUM_TEXTURE_STAGES);
    if (currentSamplerState[stage] != samplerState && currentTexture[stage])
    {
        // rebind state to the texture
        currentSamplerState[stage] = samplerState;
        currentTexture[stage]->Bind(stage);
    }
    else {
        currentSamplerState[stage] = samplerState;
    }
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetTexture(unsigned int stage, const Texture* texture)
{
    assert(stage < NUM_TEXTURE_STAGES);
    currentTexture[stage] = texture;
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetVertexBuffer(const VertexBuffer* vertexBuffer)
{
    currentVertexBuffer = vertexBuffer;
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetVertexLayout(const VertexLayout* vertexLayout)
{
    currentVertexLayout = vertexLayout;
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetIndexBuffer(const IndexBuffer* indexBuffer, IndexBuffer::INDEX_TYPE type)
{
    currentIndexBuffer = indexBuffer;
    currentIndexFormat = type;
    glIndexType        = BIND_GL_INDEX_TYPE[type];
    glIndexSize        = BIND_GL_INDEX_SIZE[type];
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetProgram(const Program* program)
{
    currentProgram = program;
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetRenderTarget(const RenderTarget* renderTarget)
{
    currentRenderTarget = renderTarget;
}

// ============================ DRAW ============================ //

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::Draw( PRIMITIVE_TYPE primType,
                                    unsigned       firstVertex,
                                    unsigned       numVertices ) const
{
    glDrawArrays(BIND_PRIMITIVE_TYPE[primType], firstVertex, numVertices);
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::DrawInstanced( PRIMITIVE_TYPE primType,
                                             unsigned       firstVertex,
                                             unsigned       numVertices,
                                             unsigned       numInstances ) const
{
    glDrawArraysInstancedEXT(BIND_PRIMITIVE_TYPE[primType], firstVertex, numVertices, numInstances);
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::DrawIndexed( PRIMITIVE_TYPE primType,
                                           unsigned       firstIndex,
                                           unsigned       numIndices ) const
{
    glDrawElements(BIND_PRIMITIVE_TYPE[primType], numIndices, glIndexType, (GLvoid*)(firstIndex * glIndexSize));
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::DrawIndexedInstanced( PRIMITIVE_TYPE primType,
                                                    unsigned       firstIndex,
                                                    unsigned       numIndices,
                                                    unsigned       numInstances ) const
{
    glDrawElementsInstanced(BIND_PRIMITIVE_TYPE[primType], numIndices, glIndexType, (GLvoid*)(firstIndex * glIndexSize), numInstances);
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::Clear(bool colorBuffer, bool depthBuffer, bool stencilBuffer) const
{
    GLbitfield mask = (colorBuffer   ? GL_COLOR_BUFFER_BIT   : 0)
                    | (depthBuffer   ? GL_DEPTH_BUFFER_BIT   : 0)
                    | (stencilBuffer ? GL_STENCIL_BUFFER_BIT : 0);
    glClear(mask);;
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SwapBuffers() const
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
template<DEVICE_VERSION deviceVersion>
Texture2D* GLDevice<deviceVersion>::CreateTexture2D(const Texture2D::DESC& desc)
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

template<DEVICE_VERSION deviceVersion>
Texture2D* SGL_DLLCALL GLDevice<deviceVersion>::CreateTexture2DMS(const Texture2D::DESC_MS& desc)
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

template<DEVICE_VERSION deviceVersion>
Texture3D* SGL_DLLCALL GLDevice<deviceVersion>::CreateTexture3D(const Texture3D::DESC& desc)
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

template<DEVICE_VERSION deviceVersion>
Texture3D* SGL_DLLCALL GLDevice<deviceVersion>::CreateTexture3DMS(const Texture3D::DESC_MS& desc)
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

template<DEVICE_VERSION deviceVersion>
TextureCube* SGL_DLLCALL GLDevice<deviceVersion>::CreateTextureCube(const TextureCube::DESC& desc)
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

template<DEVICE_VERSION deviceVersion>
VertexLayout* SGL_DLLCALL GLDevice<deviceVersion>::CreateVertexLayout(unsigned int                 numElements,
                                                                      const VertexLayout::ELEMENT* elements)
{
	return new GLVertexLayout(this, numElements, elements);
}

template<DEVICE_VERSION deviceVersion>
VertexBuffer* SGL_DLLCALL GLDevice<deviceVersion>::CreateVertexBuffer()
{
    return new GLVertexBuffer(this);
}

template<DEVICE_VERSION deviceVersion>
IndexBuffer* SGL_DLLCALL GLDevice<deviceVersion>::CreateIndexBuffer()
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

template<DEVICE_VERSION deviceVersion>
BlendState* SGL_DLLCALL GLDevice<deviceVersion>::CreateBlendState(const BlendState::DESC& desc)
{
    return new GLBlendState(this, desc);
}

template<DEVICE_VERSION deviceVersion>
DepthStencilState* SGL_DLLCALL GLDevice<deviceVersion>::CreateDepthStencilState(const DepthStencilState::DESC& desc)
{
    return new GLDepthStencilState(this, desc);
}

template<DEVICE_VERSION deviceVersion>
RasterizerState* SGL_DLLCALL GLDevice<deviceVersion>::CreateRasterizerState(const RasterizerState::DESC& desc)
{
    return new GLRasterizerState(this, desc);
}

template<DEVICE_VERSION deviceVersion>
SamplerState* SGL_DLLCALL GLDevice<deviceVersion>::CreateSamplerState(const SamplerState::DESC& desc)
{
    return new GLSamplerState(this, desc);
}

template<DEVICE_VERSION deviceVersion>
void SGL_DLLCALL GLDevice<deviceVersion>::PushState(State::TYPE type)
{
    switch (type)
    {
        case State::BLEND_STATE:
        {
            stateStack[State::BLEND_STATE].push(currentBlendState);
            break;
        }

        case State::DEPTH_STENCIL_STATE:
        {
            stateStack[State::DEPTH_STENCIL_STATE].push(currentDepthStencilState);
            break;
        }

        case State::RASTERIZER_STATE:
        {
            stateStack[State::RASTERIZER_STATE].push(currentRasterizerState);
            break;
        }

        case State::SAMPLER_STATE:
        {
            for(size_t i = 0; i<8; ++i) {
                stateStack[State::SAMPLER_STATE + i].push(currentSamplerState[i]);
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

template<DEVICE_VERSION deviceVersion>
SGL_HRESULT SGL_DLLCALL GLDevice<deviceVersion>::PopState(State::TYPE type)
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
            static_cast<const GLBlendState&>( *stateStack[State::BLEND_STATE].top() ).Bind();
            stateStack[State::BLEND_STATE].pop();
            break;
        }

        case State::DEPTH_STENCIL_STATE:
        {
            static_cast<const GLDepthStencilState&>( *stateStack[State::DEPTH_STENCIL_STATE].top() ).Bind();
            stateStack[State::DEPTH_STENCIL_STATE].pop();
            break;
        }

        case State::RASTERIZER_STATE:
        {
            static_cast<const GLRasterizerState&>( *stateStack[State::RASTERIZER_STATE].top() ).Bind();
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

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetClearColor(const math::Vector4f& color)
{
    glClearColor(color.x, color.y, color.z, color.w);
}

template<DEVICE_VERSION deviceVersion>
void GLDevice<deviceVersion>::SetClearDepthStencil(double depthClear, int stencilClear)
{
    glClearDepth(depthClear);
    glClearStencil(stencilClear);
}

template<DEVICE_VERSION deviceVersion>
math::Vector4f GLDevice<deviceVersion>::ClearColor() const
{
    math::Vector4f color;
    glGetFloatv(GL_COLOR_CLEAR_VALUE, &color[0]);
    return color;
}

template<DEVICE_VERSION deviceVersion>
double GLDevice<deviceVersion>::ClearDepth() const
{
    double depth;
    glGetDoublev(GL_DEPTH_CLEAR_VALUE, &depth);
    return depth;
}

template<DEVICE_VERSION deviceVersion>
int GLDevice<deviceVersion>::ClearStencil() const
{
    int stencil;
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &stencil);
    return stencil;
}


template<DEVICE_VERSION deviceVersion>
RenderTarget* GLDevice<deviceVersion>::CreateRenderTarget()
{
    return new GLRenderTarget(this);
}

template<DEVICE_VERSION deviceVersion>
SGL_HRESULT GLDevice<deviceVersion>::CopyTexture2D( Texture2D*     texture,
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

    GLTexture2D*                     glTexture = static_cast<GLTexture2D*>(texture);
    GLTexture2D::guarded_binding_ptr texBinding( new GLTexture2D::guarded_binding(this, glTexture, 0) );
    {
        glCopyTexSubImage2D(glTexture->GLTarget(), level, offsetx, offsety, 0, 0, width, height);

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

// explicit template instantiation
template class GLDevice<DV_OPENGL_2_1_MIXED>;
template class GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>;

#ifdef _WIN32
#pragma pop_macro("CreateFont")
#endif
