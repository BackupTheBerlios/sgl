#include "Device.h"
#include "RectangleButton.h"
#include "ProgressBar.h"
#include "Utility/ref_ptr.hpp"
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

ref_ptr<Device>		        device;
ref_ptr<PrintErrorHandler>  errorHandler;

// quad for fractal
ref_ptr<VertexBuffer>		quadVBO;

// gui ^)
ref_ptr<Font>		font;
bool				nogui;
bool				showInfo;
bool                translating;
Vector2f            sceneScale;
Vector2f            cameraPos;
float               toggleAnimation;

auto_ptr<RectangleButton>		juliaButton;
auto_ptr<RectangleButton>		tailorButton;
auto_ptr<ProgressBar>			aBar;
auto_ptr<ProgressBar>			bBar;
auto_ptr<ProgressBar>			cBar;

// misc
float       screenWidth;
float       screenHeight;
bool        working;
float       fps;
bool        keys[1024];
float       animationSpeed;
float       animationTime;
bool        takeScreenShot;
std::string outputFile;

// States
ref_ptr<ClearColorState>        clearColorState;
ref_ptr<VertexColorState>       vertexColorState;
ref_ptr<ModelViewMatrixState>   modelviewMatrixState;
ref_ptr<ProjectionMatrixState>  projectionMatrixState;

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

// tailor
ref_ptr<Shader>		tailorShader;
ref_ptr<Program>	tailorProgram;
UniformF*			tailorUniformA;
UniformF*			tailorUniformB;
UniformF*			tailorUniformC;
UniformF*			tailorUniformDensity;

Uint32 Update(Uint32 /*interval*/)
{
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

void CreateCommonScene()
{
    std::vector<Vector2f>   vertices;

    vertices.push_back( Vector2f(-200.0, -200.0) );
    vertices.push_back( Vector2f(-200.0, 200.0) );
    vertices.push_back( Vector2f(200.0, 200.0) );
    vertices.push_back( Vector2f(200.0, -200.0) );

    quadVBO.reset( device->CreateVertexBuffer() );

	ref_ptr<VertexDeclaration> vDecl( device->CreateVertexDeclaration() );
	vDecl->AddVertex(2, FLOAT);

    quadVBO->SetVertexDecl( vDecl.get() );
	quadVBO->SetData( vertices.size() * sizeof(Vector2f), &vertices[0] );

    // States
    modelviewMatrixState.reset( device->CreateModelViewMatrixState() );
    projectionMatrixState.reset( device->CreateProjectionMatrixState() );
    clearColorState.reset( device->CreateClearColorState() );
    vertexColorState.reset( device->CreateVertexColorState() );

    // transform
    projectionMatrixState->SetMatrix( Mat4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f) );
    projectionMatrixState->Setup();

    modelviewMatrixState->SetMatrix( Mat4f::identity() );
    modelviewMatrixState->Setup();

    //glViewport(0, 0, 1280, 600);
    clearColorState->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    clearColorState->Setup();

    // create font
    font.reset( device->CreateFont() );
	font->LoadFromFile("data/Fonts/font.png");

    // gui
    juliaButton.reset( new RectangleButton( device.get() ) );
    juliaButton->SetColor(0.2f, 0.0f, 0.4f, 0.5f);
    juliaButton->SetPosition( (int)screenWidth - 100, 20 );
    juliaButton->SetFont( font.get() );
    juliaButton->SetText( "Julia Set" );

    tailorButton.reset( new RectangleButton( device.get() ) );
    tailorButton->SetColor(0.2f, 0.0f, 0.4f, 0.5f);
    tailorButton->SetPosition( (int)screenWidth - 100, 60 );
    tailorButton->SetFont( font.get() );
    tailorButton->SetText( "Tailor" );

    aBar.reset( new ProgressBar( device.get() ) );
    aBar->SetColor(0.8f, 0.2f, 0.0f, 0.7f);
    aBar->SetPosition( (int)screenWidth - 200, (int)screenHeight - 60);
    aBar->SetSize(180, 12);
    aBar->SetFont( font.get() );
    aBar->SetText( "a" );

    bBar.reset( new ProgressBar( device.get() ) );
    bBar->SetColor(0.8f, 0.8f, 0.0f, 0.7f);
    bBar->SetPosition( (int)screenWidth - 200, (int)screenHeight - 40);
    bBar->SetSize(180, 12);
    bBar->SetFont( font.get() );
    bBar->SetText( "b" );

    cBar.reset( new ProgressBar( device.get() ) );
    cBar->SetColor(0.2f, 0.8f, 0.0f, 0.7f);
    cBar->SetPosition( (int)screenWidth - 200, (int)screenHeight - 20);
    cBar->SetSize(180, 12);
    cBar->SetFont( font.get() );
    cBar->SetText( "c" );

    // empty keys
    fill(keys, keys+1024, false);

    // misc
    cameraPos = Vector2f(0.0f, 0.0f);
    sceneScale = Vector2f(1.0f, 1.0f);
    animationTime = 0.0f;
    animationSpeed = 1.0f;
    toggleAnimation = 1.0f;
    working = true;
    showInfo = true;
    translating = false;
    SDL_SetTimer(100, Update);

    // shaders
    try
    {
        transformShader.reset( device->CreateShader() );
        transformShader->LoadFromFile(Shader::VERTEX, "data/Shaders/transform.vert");
    }
    catch(std::runtime_error& err)
    {
        cerr << err.what() << endl;
    }
}

void CreateJuliaScene()
{
    animationSpeed = 1.0;
    density = 32.0;
    toggleAnimation = true;
    sceneScale = Vector2f(1.0f);

    // Nothing to init
    if ( juliaShader.get() ) {
        return;
    }

    // Load shaders
    try
    {
        juliaShader.reset( device->CreateShader() );
        juliaShader->LoadFromFile(Shader::FRAGMENT, "data/Shaders/julia.frag");

        juliaProgram.reset( device->CreateProgram() );
        juliaProgram->AddShader( transformShader.get() );
        juliaProgram->AddShader( juliaShader.get() );

        juliaUniformA = juliaProgram->GetUniformF("a");
        juliaUniformB = juliaProgram->GetUniformF("b");
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

void CreateTailorScene()
{
    animationSpeed = 0.5;
    density = 16;
    toggleAnimation = true;
    sceneScale = Vector2f(0.5f);

    // Nothing to init
    if ( tailorShader.get() ) {
        return;
    }

    // Load shaders
    try
    {
        tailorShader.reset( device->CreateShader() );
        tailorShader->LoadFromFile(Shader::FRAGMENT, "data/Shaders/tailor.frag");

        tailorProgram.reset( device->CreateProgram() );
        tailorProgram->AddShader( transformShader.get() );
        tailorProgram->AddShader( tailorShader.get() );

        tailorUniformA = tailorProgram->GetUniformF("a");
        tailorUniformB = tailorProgram->GetUniformF("b");
        tailorUniformC = tailorProgram->GetUniformF("c");
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
    device.reset( sglCreateDevice(OPENGL_DEVICE) );

    screenWidth = 800;
    screenHeight = 600;
    device->SetVideoMode( (unsigned int)screenWidth,
						  (unsigned int)screenHeight,
						  32,
						  false,
						  false );

    // Setup error handler
    errorHandler.reset( new PrintErrorHandler() );
    sglSetErrorHandler( errorHandler.get() );

    CreateCommonScene();
    switch(fractalType)
    {
    case JULIA:
        CreateJuliaScene();
        break;

    case TAILOR:
        CreateTailorScene();
        break;

    default:
        assert(!"Fractal is not supported");
        break;
    }
}

void RenderCommon(float time)
{
    // font color
    vertexColorState->SetColor(1.0, 1.0, 0.0, 1.0);
    vertexColorState->Setup();

    // print info text
    if ( showInfo && SGL_OK == font->Bind(10, 12) )
    {
        ostringstream ss;
        ss << setprecision(5)
           << "density = " << density << endl
           << "animation speed = " << animationSpeed << endl
           << "up/down - modify density\n"
           << "left/right - modify animation speed\n"
           << "space - toggle animation\n"
           << "F9 - take screenshot(saved automatically)\n"
           << "F1 - toggle help text\n"
           << "escape - quit";

        font->Print( 10, 10, ss.str() );
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
    ostringstream ss;
    ss << "FPS: " << fps << endl;

    if ( SGL_OK == font->Bind(10, 12) ) 
	{
		font->Print( screenWidth / 2.0f - 40.0f, 10.0f, ss.str() );
		font->Print( screenWidth - 215.0f, screenHeight - 75.0f, "Fractal parameters:" );
		font->Unbind();
	}
    // buttons
    juliaButton->Draw();
    tailorButton->Draw();
    aBar->Draw();
    bBar->Draw();
    cBar->Draw();
}

void RenderJulia(float time)
{
    // draw
    paramA = cos(time + 0.5f);
    paramB = sin(time);
    paramC = 0.0f;
    color = Vector4f(fabs(paramA) + 0.2f, fabs(paramB) + 0.2f, fabs(paramC) + 0.4f, 1.0);

    aBar->SetValue( (paramA + 1.0f) / 2.0f );
    bBar->SetValue( (paramB + 1.0f) / 2.0f );
    cBar->SetValue( (paramC + 1.0f) / 2.0f );

    vertexColorState->SetColor(color);
    vertexColorState->Setup();
    juliaUniformA->Set(paramA);
    juliaUniformB->Set(paramB);
    juliaUniformDensity->Set(density);
    juliaProgram->Bind();
    quadVBO->Bind();
    quadVBO->Draw(QUADS);
    quadVBO->Unbind();
    juliaProgram->Unbind();
}

void RenderTailor(float time)
{
    // draw
    paramA = cos(time + 0.5f);
    paramB = sin(time);
    paramC = paramA * paramB;
    color = Vector4f(fabs(paramA) + 0.2f, fabs(paramB) + 0.2f, fabs(paramC) + 0.4f, 1.0);

    aBar->SetValue( (paramA + 1.0f) / 2.0f );
    bBar->SetValue( (paramB + 1.0f) / 2.0f );
    cBar->SetValue( (paramC + 1.0f) / 2.0f );

    vertexColorState->SetColor(color);
    vertexColorState->Setup();
    tailorUniformA->Set(paramA);
    tailorUniformB->Set(paramB);
    tailorUniformC->Set(paramC);
    tailorUniformDensity->Set(density);
    tailorProgram->Bind();
    quadVBO->Bind();
    quadVBO->Draw(QUADS);
    quadVBO->Unbind();
    tailorProgram->Unbind();
}

void Render()
{
    // render
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Setup transform
    modelviewMatrixState->SetMatrix( Mat4f::scale(sceneScale.x, sceneScale.y, 1.0)
                                     * Mat4f::translate(cameraPos.x, cameraPos.y, 0.0) );
    modelviewMatrixState->Setup();

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
            string name = string("screen_") + numSS.str() + ".bmp";

            // screenshot
            ref_ptr<Texture2D> texture;
            texture.reset( device->CreateTexture2D() );
            device->TakeScreenshot( texture.get() );
            texture->SaveToFile( name.c_str() );
        }
        else
        {
            // screenshot
            ref_ptr<Texture2D> texture;
            texture.reset( device->CreateTexture2D() );
            device->TakeScreenshot( texture.get() );
            texture->SaveToFile( outputFile.c_str() );
        }

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
            juliaButton->CheckCursor(cursorPos);
            tailorButton->CheckCursor(cursorPos);
            if ( translating ) {
                cameraPos += Vector2f( (float)event.motion.xrel, -(float)event.motion.yrel) / (sceneScale * 500.0f);
            }
            cameraPos = clamp(cameraPos, -100.0f, 100.0f);
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            Vector2i cursorPos = Vector2i(event.motion.x, event.motion.y);
            if ( tailorButton->CheckCursor(cursorPos) ) {
                tailorButton->Push();
            }
            if ( juliaButton->CheckCursor(cursorPos) ) {
                juliaButton->Push();
            }

            if(event.button.button == SDL_BUTTON_LEFT) {
                translating = true;
            }
            else if(event.button.button == SDL_BUTTON_WHEELDOWN) {
                sceneScale *= 0.95;
            }
            else if(event.button.button == SDL_BUTTON_WHEELUP) {
                sceneScale *= 1.05;
            }
            sceneScale = clamp(sceneScale, 0.005f, 50.0f);
            break;
        }

        case SDL_MOUSEBUTTONUP:
        {
            Vector2i cursorPos = Vector2i(event.motion.x, event.motion.y);
            if ( tailorButton->CheckCursor(cursorPos) )
            {
                tailorButton->Pop();
                fractalType = TAILOR;
                cameraPos = Vector2f(0.0);
                CreateTailorScene();
            }
            if ( juliaButton->CheckCursor(cursorPos) )
            {
                juliaButton->Pop();
                fractalType = JULIA;
                cameraPos = Vector2f(0.0);
                CreateJuliaScene();
            }

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
    fractalType = JULIA;
    nogui = false;
    takeScreenShot = false;

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
    if ( !ParseCommandLine(argc, argv) ) {
        return 1;
    }
    CreateScene();
    Run();

    return 0;
}
