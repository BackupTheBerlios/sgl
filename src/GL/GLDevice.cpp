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
template<DEVICE_VERSION DeviceVersion>
GLDevice<DeviceVersion>::GLDevice() :
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

template<DEVICE_VERSION DeviceVersion>
GLDevice<DeviceVersion>::GLDevice(const Device::VIDEO_DESC& desc)
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

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLDevice<DeviceVersion>::InitOpenGL()
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

    std::fill(currentTexture, currentTexture + NUM_TEXTURE_STAGES, (Texture*)0);

    // default settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    // create blend state
    {
        BlendState::DESC_SIMPLE desc;
        desc.blendEnable = false;

        BlendState* blendState = sglCreateBlendState(this, Extend(desc));
        blendState->Bind();
    }

    // create depth stencil state
    {
        DepthStencilState::DESC desc;
        desc.depthEnable    = true;
        desc.depthFunc      = DepthStencilState::LEQUAL;
        desc.depthWriteMask = true;
        desc.stencilEnable  = false;

        DepthStencilState* depthStencilState = sglCreateDepthStencilState(this, desc);
        depthStencilState->Bind();
    }

    // create rasterizer state
    {
        RasterizerState::DESC desc;
        desc.cullMode      = RasterizerState::BACK;
        desc.fillMode      = RasterizerState::SOLID;

        RasterizerState* rasterizerState = sglCreateRasterizerState(this, desc);
        rasterizerState->Bind();
    }

    // create unqie objects
    deviceTraits.reset( new GLDeviceTraits(this) );
    ffpProgram.reset( sglCreateFFPProgram(this) );

    // get viewport
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    viewport = rectangle(vp[0], vp[1], vp[2], vp[3]);

    // default props
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    assert( GL_NO_ERROR == glGetError() );
    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion>
GLDevice<DeviceVersion>::~GLDevice()
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

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLDevice<DeviceVersion>::Sync() const
{
    glFinish();
    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion>
rectangle GLDevice<DeviceVersion>::Viewport() const
{
    return viewport;
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetViewport(const rectangle& vp)
{
    if (viewport != vp)
    {
        glViewport(vp.x, vp.y, vp.width, vp.height);
        viewport = vp;
    }
}

// misc
template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLDevice<DeviceVersion>::TakeScreenshot(Image* image) const
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
template<DEVICE_VERSION DeviceVersion>
Shader* GLDevice<DeviceVersion>::CreateShader(const Shader::DESC& desc)
{
    try
    {
        Shader* shader = sglCreateShader(this, desc);
        return shader;
    }
    catch(gl_error& err)
    {
        sglSetError( err.result(), err.what() );
        return 0;
    }
}

template<DEVICE_VERSION DeviceVersion>
Program* GLDevice<DeviceVersion>::CreateProgram()
{
    try
    {
        Program* program = sglCreateProgram(this);
        return program;
    }
    catch(gl_error& err)
    {
        sglSetError( err.result(), err.what() );
        return 0;
    }
}

template<DEVICE_VERSION DeviceVersion>
sgl::Font* GLDevice<DeviceVersion>::CreateFont()
{
    return sglCreateFont<DeviceVersion>(this);
}

template<DEVICE_VERSION DeviceVersion>
Image* GLDevice<DeviceVersion>::CreateImage()
{
#ifdef SIMPLE_GL_USE_DEVIL
    return new IlImage( const_cast<GLDevice*>(this) );
#endif
    sglSetError(SGLERR_UNSUPPORTED, "SimpleGL compiled without DevIL. Can't create image.");
    return 0;
}

// internal binders

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetBlendState(const BlendState* blendState)
{
    currentBlendState = blendState;
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetDepthStencilState(const DepthStencilState* depthStencilState)
{
    currentDepthStencilState = depthStencilState;
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetRasterizerState(const RasterizerState* rasterizerState)
{
    currentRasterizerState = rasterizerState;
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetTexture(unsigned int stage, const Texture* texture)
{
    assert(stage < NUM_TEXTURE_STAGES);
    currentTexture[stage] = texture;
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetVertexBuffer(const VertexBuffer* vertexBuffer)
{
    currentVertexBuffer = vertexBuffer;
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetVertexLayout(const VertexLayout* vertexLayout)
{
    currentVertexLayout = vertexLayout;
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetIndexBuffer(const IndexBuffer* indexBuffer, IndexBuffer::INDEX_TYPE type)
{
    currentIndexBuffer = indexBuffer;
    currentIndexFormat = type;
    glIndexType        = BIND_GL_INDEX_TYPE[type];
    glIndexSize        = BIND_GL_INDEX_SIZE[type];
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetProgram(const Program* program)
{
    currentProgram = program;
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetRenderTarget(const RenderTarget* renderTarget)
{
    currentRenderTarget = renderTarget;
}

// ============================ DRAW ============================ //

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::Draw( PRIMITIVE_TYPE primType,
                                    unsigned       firstVertex,
                                    unsigned       numVertices ) const
{
    glDrawArrays(BIND_PRIMITIVE_TYPE[primType], firstVertex, numVertices);
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::DrawInstanced( PRIMITIVE_TYPE primType,
                                             unsigned       firstVertex,
                                             unsigned       numVertices,
                                             unsigned       numInstances ) const
{
    glDrawArraysInstancedEXT(BIND_PRIMITIVE_TYPE[primType], firstVertex, numVertices, numInstances);
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::DrawIndexed( PRIMITIVE_TYPE primType,
                                           unsigned       firstIndex,
                                           unsigned       numIndices ) const
{
    glDrawElements(BIND_PRIMITIVE_TYPE[primType], numIndices, glIndexType, (GLvoid*)(firstIndex * glIndexSize));
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::DrawIndexedInstanced( PRIMITIVE_TYPE primType,
                                                    unsigned       firstIndex,
                                                    unsigned       numIndices,
                                                    unsigned       numInstances ) const
{
    glDrawElementsInstanced(BIND_PRIMITIVE_TYPE[primType], numIndices, glIndexType, (GLvoid*)(firstIndex * glIndexSize), numInstances);
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::Clear(bool colorBuffer, bool depthBuffer, bool stencilBuffer) const
{
    GLbitfield mask = (colorBuffer   ? GL_COLOR_BUFFER_BIT   : 0)
                    | (depthBuffer   ? GL_DEPTH_BUFFER_BIT   : 0)
                    | (stencilBuffer ? GL_STENCIL_BUFFER_BIT : 0);
    glClear(mask);;
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SwapBuffers() const
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
template<DEVICE_VERSION DeviceVersion>
Texture2D* GLDevice<DeviceVersion>::CreateTexture2D(const Texture2D::DESC& desc)
{
    try
    {
        return new GLTexture2D<DeviceVersion>(this, desc);
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

template<DEVICE_VERSION DeviceVersion>
Texture2D* SGL_DLLCALL GLDevice<DeviceVersion>::CreateTexture2DMS(const Texture2D::DESC_MS& desc)
{
    try
    {
        return new GLTexture2D<DeviceVersion>(this, desc);
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

template<DEVICE_VERSION DeviceVersion>
Texture3D* SGL_DLLCALL GLDevice<DeviceVersion>::CreateTexture3D(const Texture3D::DESC& desc)
{
    try
    {
        return new GLTexture3D<DeviceVersion>(this, desc);
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

template<DEVICE_VERSION DeviceVersion>
Texture3D* SGL_DLLCALL GLDevice<DeviceVersion>::CreateTexture3DMS(const Texture3D::DESC_MS& desc)
{
    try
    {
        return new GLTexture3D<DeviceVersion>(this, desc);
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

template<DEVICE_VERSION DeviceVersion>
TextureCube* SGL_DLLCALL GLDevice<DeviceVersion>::CreateTextureCube(const TextureCube::DESC& desc)
{
    try
    {
        return new GLTextureCube<DeviceVersion>(this, desc);
    }
    catch(gl_error& err)
    {
        sglSetError( SGLERR_UNKNOWN, err.what() );
        return 0;
    }
}

// ============================ BUFFERS ============================ //

template<DEVICE_VERSION DeviceVersion>
VertexLayout* SGL_DLLCALL GLDevice<DeviceVersion>::CreateVertexLayout(unsigned int                 numElements,
                                                                      const VertexLayout::ELEMENT* elements)
{
    try
    {
        VertexLayout* layout = sglCreateVertexLayout(this, numElements, elements);
        return layout;
    }
    catch(gl_error& err)
    {
        sglSetError( err.result(), err.what() );
        return 0;
    }
}

template<DEVICE_VERSION DeviceVersion>
VertexBuffer* SGL_DLLCALL GLDevice<DeviceVersion>::CreateVertexBuffer()
{
    return new GLVertexBuffer<DeviceVersion>(this);
}

template<DEVICE_VERSION DeviceVersion>
IndexBuffer* SGL_DLLCALL GLDevice<DeviceVersion>::CreateIndexBuffer()
{
    return new GLIndexBuffer<DeviceVersion>(this);
}
/*
UniformBufferView* SGL_DLLCALL GLDevice::CreateUniformBuffer()
{
    return new GLUniformBufferView( this, new GLBuffer(this) );
}
*/
// ============================ STATES ============================ //

template<DEVICE_VERSION DeviceVersion>
BlendState* SGL_DLLCALL GLDevice<DeviceVersion>::CreateBlendState(const BlendState::DESC& desc)
{
    try
    {
        BlendState* state = sglCreateBlendState(this, desc);
        return state;
    }
    catch(gl_error& err)
    {
        sglSetError( err.result(), err.what() );
        return 0;
    }
}

template<DEVICE_VERSION DeviceVersion>
DepthStencilState* SGL_DLLCALL GLDevice<DeviceVersion>::CreateDepthStencilState(const DepthStencilState::DESC& desc)
{
    try
    {
        DepthStencilState* state = sglCreateDepthStencilState(this, desc);
        return state;
    }
    catch(gl_error& err)
    {
        sglSetError( err.result(), err.what() );
        return 0;
    }
}

template<DEVICE_VERSION DeviceVersion>
RasterizerState* SGL_DLLCALL GLDevice<DeviceVersion>::CreateRasterizerState(const RasterizerState::DESC& desc)
{
    try
    {
        RasterizerState* state = sglCreateRasterizerState(this, desc);
        return state;
    }
    catch(gl_error& err)
    {
        sglSetError( err.result(), err.what() );
        return 0;
    }
}

template<DEVICE_VERSION DeviceVersion>
SamplerState* SGL_DLLCALL GLDevice<DeviceVersion>::CreateSamplerState(const SamplerState::DESC& desc)
{
    return new GLSamplerState<DeviceVersion>(this, desc);
}

template<DEVICE_VERSION DeviceVersion>
void SGL_DLLCALL GLDevice<DeviceVersion>::PushState(State::TYPE type)
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

        default:
            break;
    }
}

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT SGL_DLLCALL GLDevice<DeviceVersion>::PopState(State::TYPE type)
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

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetClearColor(const math::Vector4f& color)
{
    glClearColor(color.x, color.y, color.z, color.w);
}

template<DEVICE_VERSION DeviceVersion>
void GLDevice<DeviceVersion>::SetClearDepthStencil(double depthClear, int stencilClear)
{
    glClearDepth(depthClear);
    glClearStencil(stencilClear);
}

template<DEVICE_VERSION DeviceVersion>
math::Vector4f GLDevice<DeviceVersion>::ClearColor() const
{
    math::Vector4f color;
    glGetFloatv(GL_COLOR_CLEAR_VALUE, &color[0]);
    return color;
}

template<DEVICE_VERSION DeviceVersion>
double GLDevice<DeviceVersion>::ClearDepth() const
{
    double depth;
    glGetDoublev(GL_DEPTH_CLEAR_VALUE, &depth);
    return depth;
}

template<DEVICE_VERSION DeviceVersion>
int GLDevice<DeviceVersion>::ClearStencil() const
{
    int stencil;
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &stencil);
    return stencil;
}


template<DEVICE_VERSION DeviceVersion>
RenderTarget* GLDevice<DeviceVersion>::CreateRenderTarget()
{
    return new GLRenderTarget<DeviceVersion>(this);
}

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLDevice<DeviceVersion>::CopyTexture2D( Texture2D*     texture,
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

    GLTexture2D<DeviceVersion>*                     glTexture = static_cast<GLTexture2D<DeviceVersion>*>(texture);
    GLTexture2D<DeviceVersion>::guarded_binding_ptr texBinding( new GLTexture2D<DeviceVersion>::guarded_binding(this, glTexture, 0) );
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

// explicit template instantiation
template class GLDevice<DV_OPENGL_1_3>;
template class GLDevice<DV_OPENGL_1_4>;
template class GLDevice<DV_OPENGL_1_5>;
template class GLDevice<DV_OPENGL_2_0>;
template class GLDevice<DV_OPENGL_2_1>;
template class GLDevice<DV_OPENGL_3_0>;
template class GLDevice<DV_OPENGL_3_1>;
template class GLDevice<DV_OPENGL_3_2>;

#ifdef _WIN32
#pragma pop_macro("CreateFont")
#endif
