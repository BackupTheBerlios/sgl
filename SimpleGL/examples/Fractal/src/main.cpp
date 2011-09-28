#include "Math/Matrix.hpp"
#include "Utility/FX/ShaderUtility.h"
#include "SDL.h"
#include "SDL_main.h"
#include <GL/glew.h>
#include <memory>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <time.h>

using namespace sgl;
using namespace math;
using namespace std;

enum FRACTAL_TYPE
{
    JULIA,
    TAILOR
};

ref_ptr<Device>	   device;
PrintErrorHandler  errorHandler;

// quad for fractal
ref_ptr<VertexBuffer>		quadVBO;
ref_ptr<VertexLayout>       vertexLayout;

// gui ^)
ref_ptr<Font>		font;
bool				nogui;
bool				showInfo;
bool                translating;
Vector2f            sceneScale;
Vector2f            cameraPos;
float               toggleAnimation;

// misc
float       screenWidth;
float       screenHeight;
bool        working;
float       fps;
bool        keys[SDLK_LAST];
float       animationSpeed;
float       animationTime;
bool        takeScreenShot;
std::string outputFile;

// States
ref_ptr<FFPProgram>     ffpProgram;
Uniform4x4F*            projectionMatrixUniform;

// fractals
FRACTAL_TYPE            fractalType;
ref_ptr<Shader>			transformShader;
Vector4f                color;
float                   density;
float                   paramA;
float                   paramB;
float                   paramC;

// julia set
ref_ptr<Shader>		juliaShader;
ref_ptr<Program>	juliaProgram;
UniformF*			juliaUniformA;
UniformF*			juliaUniformB;
UniformF*			juliaUniformDensity;
Uniform4x4F*        juliaUniformMVP;

// tailor
ref_ptr<Shader>		tailorShader;
ref_ptr<Program>	tailorProgram;
UniformF*			tailorUniformA;
UniformF*			tailorUniformB;
UniformF*			tailorUniformC;
UniformF*			tailorUniformDensity;
Uniform4x4F*        tailorUniformMVP;

void CreateCommonScene()
{
    std::vector<Vector2f>   vertices;

    vertices.push_back( Vector2f(-200.0, -200.0) );
    vertices.push_back( Vector2f(-200.0, 200.0) );
    vertices.push_back( Vector2f(200.0, 200.0) );
    vertices.push_back( Vector2f(200.0, -200.0) );

    quadVBO.reset( device->CreateVertexBuffer() );
	quadVBO->SetData( vertices.size() * sizeof(Vector2f), &vertices[0] );

    VertexLayout::ELEMENT elements[] =
    {
        {0, 2, 0, 8, FLOAT, VertexLayout::VERTEX},
    };
    vertexLayout.reset( device->CreateVertexLayout(1, elements) );

    // States
    ffpProgram.reset( device->FixedPipelineProgram() );

    //glViewport(0, 0, 1280, 600);
    //clearColorState->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    //clearColorState->Setup();

    // create font
    font.reset( device->CreateFont() );
    {
        sgl::Image* image = device->CreateImage();
        image->LoadFromFile("data/Fonts/font.png");
	    font->SetTexture( image->CreateTexture2D() );
    }

    // empty keys
    fill(keys, keys + SDLK_LAST, false);

    // misc
    cameraPos = Vector2f(0.0f, 0.0f);
    sceneScale = Vector2f(1.0f, 1.0f);
    animationTime = 0.0f;
    animationSpeed = 1.0f;
    toggleAnimation = 1.0f;
    working = true;
    showInfo = true;
    translating = false;

    // shaders
    transformShader.reset( CreateShaderFromFile(device, Shader::VERTEX, "data/Shaders/transform.vert") );
    if (!transformShader) {
        throw std::runtime_error("Shader file not loaded");
    }
}

void SetJuliaParameters()
{
    animationSpeed = 1.0;
    density = 32.0;
    toggleAnimation = true;
    sceneScale = Vector2f(1.0f, 1.0f);
}

void CreateJuliaScene()
{
    // Nothing to init
    if ( juliaShader.get() ) {
        return;
    }

    // Load shaders
    try
    {
        juliaShader.reset( CreateShaderFromFile(device, Shader::FRAGMENT, "data/Shaders/julia.frag") );
        if (!juliaShader) {
            throw std::runtime_error("Shader file not loaded");
        }

        juliaProgram.reset( device->CreateProgram() );
        juliaProgram->AddShader( transformShader.get() );
        juliaProgram->AddShader( juliaShader.get() );
        if ( SGL_OK != juliaProgram->Dirty() ) {
            throw std::runtime_error("Shader program not compiled");
        }

        juliaUniformMVP = juliaProgram->GetUniform4x4F("modelViewProjectionMatrix");
        juliaUniformA   = juliaProgram->GetUniformF("a");
        juliaUniformB   = juliaProgram->GetUniformF("b");
        juliaUniformDensity = juliaProgram->GetUniformF("density");
        assert( juliaUniformA );
        assert( juliaUniformB );
        assert( juliaUniformDensity );
    }
    catch(std::runtime_error& err)
    {
        cerr << err.what() << endl;
    }
}

void SetTailorParameters()
{
    animationSpeed = 0.5;
    density = 16;
    toggleAnimation = true;
    sceneScale = Vector2f(0.5f, 0.5f);
}

void CreateTailorScene()
{
    // Nothing to init
    if ( tailorShader.get() ) {
        return;
    }

    // Load shaders
    try
    {
        tailorShader.reset( CreateShaderFromFile(device, Shader::FRAGMENT, "data/Shaders/tailor.frag") );
        if (!tailorShader) {
            throw std::runtime_error("Shader file not loaded");
        }

        tailorProgram.reset( device->CreateProgram() );
        tailorProgram->AddShader( transformShader.get() );
        tailorProgram->AddShader( tailorShader.get() );
        if ( SGL_OK != tailorProgram->Dirty() ) {
            throw std::runtime_error("Shader program not compiled");
        }

        tailorUniformMVP = tailorProgram->GetUniform4x4F("modelViewProjectionMatrix");
        tailorUniformA   = tailorProgram->GetUniformF("a");
        tailorUniformB   = tailorProgram->GetUniformF("b");
        tailorUniformC   = tailorProgram->GetUniformF("c");
        tailorUniformDensity = tailorProgram->GetUniformF("density");
        assert( tailorUniformA );
        assert( tailorUniformB );
        assert( tailorUniformC );
        assert( tailorUniformDensity );

        //tailorUniformA->SetValue(-0.7);
        //tailorUniformB->SetValue(0.5);
    }
    catch(std::runtime_error& err)
    {
        cerr << err.what() << endl;
    }
}

void CreateScene()
{
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 && !SDL_GetVideoInfo() ) {
        throw std::runtime_error("Can't init SDL");
    }

    // misc
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // create window
    unsigned flags = SDL_HWSURFACE | SDL_OPENGL;
    if ( !SDL_SetVideoMode( static_cast<int>(screenWidth),
                            static_cast<int>(screenHeight),
                            32,
                            flags ) )
    {
        throw std::runtime_error( (string("Can't set video mode: ") + SDL_GetError()).c_str() );
    }

    // grab current context for rendering
    device.reset( sglCreateDeviceFromCurrent(DV_OPENGL_2_1) );

    // Setup error handler
    sglSetErrorHandler(&errorHandler);

    CreateCommonScene();
    CreateJuliaScene();
    CreateTailorScene();
    switch(fractalType)
    {
    case JULIA:
        SetJuliaParameters();
        break;

    case TAILOR:
        SetTailorParameters();
        break;

    default:
        assert(!"Can't get here");
        break;
    }
}

Uint32 Update(Uint32 /*interval*/)
{
    if ( keys[SDLK_t] ) 
    {
        SetTailorParameters();
        keys[SDLK_t] = false;
        fractalType  = TAILOR;
    }
    if ( keys[SDLK_j] ) 
    {
        SetJuliaParameters();
        keys[SDLK_j] = false;
        fractalType  = JULIA;
    }

    if ( keys[SDLK_UP] ) {
        density += 1.0f;
    }
    if ( keys[SDLK_DOWN] ) {
        density -= 1.0f;
    }
    if ( keys[SDLK_RIGHT] ) {
        animationSpeed += 0.1f;
    }
    if ( keys[SDLK_LEFT] ) {
        animationSpeed -= 0.1f;
    }
    if ( keys[SDLK_ESCAPE] )
    {
        working = false;
        keys[SDLK_ESCAPE] = false;
    }
    if ( keys[SDLK_F1] )
    {
        showInfo = !showInfo;
        keys[SDLK_F1] = false;
    }
    if ( keys[SDLK_SPACE] )
    {
        toggleAnimation = 1.0f - toggleAnimation;
        keys[SDLK_SPACE] = false;
    }

    // take screenshot
    if ( keys[SDLK_F9] )
    {
        takeScreenShot = true;
        keys[SDLK_F9]  = false;
    }

    return 100;
}

void RenderCommon(float time)
{
    // font color
    //vertexColorState->SetColor(1.0, 1.0, 0.0, 1.0);
    //vertexColorState->Setup();

    // print info text
    if ( showInfo && SGL_OK == font->Bind(10, 12) )
    {
        ostringstream ss;
        ss << setprecision(5)
           << "t - tailor fractal\n"
           << "j - julia set fractal\n"
           << "density = " << density << endl
           << "animation speed = " << animationSpeed << endl
           << "up/down - modify density\n"
           << "left/right - modify animation speed\n"
           << "space - toggle animation\n"
           << "F9 - take screenshot(saved automatically)\n"
           << "F1 - toggle help text\n"
           << "escape - quit";

        font->Print( 10, 10, ss.str().c_str() );
        font->Unbind();
    }

    static float lastTime = time;
    static int frames = 0;
    ++frames;
    if ( time - lastTime > 0.5f )
    {
        fps = 2.0f * frames / (1.0f + (time - lastTime - 0.5f) / 0.5f);
		frames = 0;
		lastTime = time;
    }

    // draw fps
    if ( SGL_OK == font->Bind(10, 12) )
	{
        ostringstream ss;
        ss << "FPS: " << fps << endl;
        font->Print( screenWidth / 2.0f - 40.0f, 10.0f, ss.str().c_str() );
        font->Unbind();
	}
    // buttons
    //juliaButton->Draw();
    //tailorButton->Draw();
    //aBar->Draw();
    //bBar->Draw();
    //cBar->Draw();
}

void RenderJulia(float time)
{
    // draw
    paramA = cos(time + 0.5f);
    paramB = sin(time);
    paramC = 0.0f;
    color = Vector4f(fabs(paramA) + 0.2f, fabs(paramB) + 0.2f, fabs(paramC) + 0.4f, 1.0);

    //glDisable(GL_CULL_FACE);
    //vertexColorState->SetColor(color);
    //vertexColorState->Setup();
    juliaProgram->Bind();
    {
        juliaUniformMVP->Set( Matrix4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f) 
                              * Matrix4f::scaling(sceneScale.x, sceneScale.y, 1.0f)
                              * Matrix4f::translation(cameraPos.x, cameraPos.y, 0.0f) );
        juliaUniformA->Set(paramA);
        juliaUniformB->Set(paramB);
        juliaUniformDensity->Set(density);
        quadVBO->Bind( vertexLayout.get() );
        device->Draw(QUADS, 0, 4);
        quadVBO->Unbind();
        juliaProgram->Unbind();
    }
}

void RenderTailor(float time)
{
    // draw
    paramA = cos(time + 0.5f);
    paramB = sin(time);
    paramC = paramA * paramB;
    color  = Vector4f(fabs(paramA) + 0.2f, fabs(paramB) + 0.2f, fabs(paramC) + 0.4f, 1.0);

    //vertexColorState->SetColor(color);
    //vertexColorState->Setup();
    tailorProgram->Bind();
    {
        tailorUniformMVP->Set( Matrix4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f) 
                               * Matrix4f::scaling(sceneScale.x, sceneScale.y, 1.0f)
                               * Matrix4f::translation(cameraPos.x, cameraPos.y, 0.0f) );
        tailorUniformA->Set(paramA);
        tailorUniformB->Set(paramB);
        tailorUniformC->Set(paramC);
        tailorUniformDensity->Set(density);
        quadVBO->Bind( vertexLayout.get() );
        device->Draw(QUADS, 0, 4);
        quadVBO->Unbind();
        tailorProgram->Unbind();
    }
}

void Render()
{
    // render
    device->Clear(true, true);

    static float lastTime = -2.0;
    float curTime = SDL_GetTicks() / 1000.0f;
    switch(fractalType)
    {
    case JULIA:
        RenderJulia(animationTime += (curTime - lastTime) * animationSpeed * toggleAnimation);
        break;

    case TAILOR:
        RenderTailor(animationTime += (curTime - lastTime) * animationSpeed * toggleAnimation);
        break;

    default:
        assert(!"Fractal is not supported");
        break;
    }
    RenderCommon(curTime);
    lastTime = curTime;

    // here we can take screenshot
    if ( takeScreenShot )
    {
        if ( outputFile.empty() )
        {
            // get time
            static int screenNum = 0;

            ostringstream numSS;
            numSS << screenNum++;
            outputFile = string("screen_") + numSS.str()
        #ifdef SIMPLE_GL_USE_SDL_IMAGE
                       + ".bmp";
        #else // SIMPLE_GL_USE_DEVIL
                       + ".jpg";
        #endif // SIMPLE_GL_USE_SDL_IMAGE
        }

        // screenshot
        ref_ptr<Image> image( device->CreateImage() );
        //device->TakeScreenshot( image.get() );
        image->SaveToFile( outputFile.c_str() );
        outputFile.clear();

        takeScreenShot = false;
    }

    // all done
    device->SwapBuffers();
}

void HandleEvents()
{
    // events
    SDL_Event event;
    while( SDL_PollEvent( &event ) )
    {
        switch( event.type )
        {
        /* SDL_QUIT event (window close) */
        case SDL_QUIT:
            working = false;
            break;

        case SDL_KEYDOWN:
            keys[event.key.keysym.sym] = true;
            break;

        case SDL_KEYUP:
            keys[event.key.keysym.sym] = false;
            break;

        case SDL_MOUSEMOTION:
        {
            //cout << event.motion.x << " " << event.motion.y << endl;
            Vector2i cursorPos = Vector2i(event.motion.x, event.motion.y);
            if ( translating ) {
                cameraPos += Vector2f( (float)event.motion.xrel, -(float)event.motion.yrel ) / (sceneScale * 500.0f);
            }
            cameraPos = clamp(cameraPos, -100.0f, 100.0f);
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            Vector2i cursorPos = Vector2i(event.motion.x, event.motion.y);

            if(event.button.button == SDL_BUTTON_LEFT) {
                translating = true;
            }
            else if(event.button.button == SDL_BUTTON_WHEELDOWN) {
                sceneScale *= 0.95f;
            }
            else if(event.button.button == SDL_BUTTON_WHEELUP) {
                sceneScale *= 1.05f;
            }
            sceneScale = clamp(sceneScale, 0.005f, 50.0f);
            break;
        }

        case SDL_MOUSEBUTTONUP:
        {
            Vector2i cursorPos = Vector2i(event.motion.x, event.motion.y);

            if(event.button.button == SDL_BUTTON_LEFT) {
                translating = false;
            }
            break;
        }

        default:
            break;
        }
    }
}

void Run()
{
    working = true;
    while(working)
    {
        if ( !nogui ) {
            HandleEvents();
        }
        else {
            working = false;
        }

        Render();
    }
}

void PrintHelp()
{
    cout << "Command line options:\n"
         << "  -julia - setup julia set fractal(default)\n"
         << "  -tailor - setup tailor fractal\n"
         << "  -time <time> - setup animation starting time\n"
         << "  -nogui - hide gui window\n"
         << "  -o <file> - save fractal to the file\n";
}

bool ParseCommandLine(int argc, char** argv)
{
    screenWidth     = 800.0f;
    screenHeight    = 600.0f;
    fractalType     = JULIA;
    nogui           = false;
    takeScreenShot  = false;

    for(int i = 1; i<argc; ++i)
    {
        string argstr = argv[i];

        if (argstr == "-julia") {
            fractalType = JULIA;
        }
        else if (argstr == "-tailor") {
            fractalType = TAILOR;
        }
        else if (argstr == "-nogui") {
            nogui = true;
        }
        else if (argstr == "-o")
        {
            if (++i >= argc)
            {
                cout << "Invalid command line parameters!\n";
                PrintHelp();
            }
            takeScreenShot = true;
            outputFile = argv[i];
        }
        else if (argstr == "-help")
        {
            PrintHelp();
        }
    }

    if (argc == 1) {
        PrintHelp();
    }

    return true;
}

int main(int argc, char** argv)
{
    /*
    std::cout << "sizeof vec: " << sizeof(math::Vector4f) << std::endl
              << "sizeof mat: " << sizeof(math::Matrix4f) << std::endl;
     */
    if ( !ParseCommandLine(argc, argv) ) {
        return 1;
    }

    CreateScene();
    SDL_SetTimer(100, Update);
    Run();

    sglSetErrorHandler(0);
    SDL_Quit();

    return 0;
}
