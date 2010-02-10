#include "Device.h"
#include "Math/Containers.hpp"
#include "SDL.h"
#include "SDL_main.h"
#include <GL/glew.h>
#include <sstream>

bool	working;
bool	keys[SDLK_LAST];
float	fps;
float	screenWidth;
float	screenHeight;

using namespace sgl;
using namespace math;

const int max_texture_width  = 1024;
const int max_texture_height = 1024;

// sgl
sgl::ref_ptr<sgl::Device> device;

// gui
sgl::ref_ptr<sgl::Font> font;

// render tech
enum RENDER_TECHNIQUE { PBO_COPY, VERTEX_TEXTURE_FETCH };
RENDER_TECHNIQUE renderTechnique = PBO_COPY;

// scene
sgl::ref_ptr<sgl::FFPProgram>       ffpProgram;
sgl::Uniform4x4F*                   projectionMatrixUniform;
sgl::Uniform4x4F*                   modelViewMatrixUniform;

// states
math::Matrix4f                      modelViewMatrix;
math::Matrix4f                      projectionMatrix;

// r2vb
int sizeX;
int sizeY;

sgl::ref_ptr<sgl::VBORenderTarget>  vboRenderTarget;

sgl::ref_ptr<sgl::Program>	        sphereProgram;
sgl::ref_ptr<sgl::VertexBuffer>     quadVBO;
sgl::ref_ptr<sgl::VertexBuffer>     sphereVerticesVBO;
sgl::ref_ptr<sgl::VertexBuffer>     sphereNormalsVBO;

sgl::Uniform4x4F*                   r2vb_mvpMatrixUniform;  

// vtf
sgl::ref_ptr<sgl::Program>          vtfProgram;
sgl::ref_ptr<sgl::VertexBuffer>     vtfVBO;

sgl::SamplerUniform2D*              verticesMapUniform;         
sgl::Uniform4x4F*                   vtf_mvpMatrixUniform;    
sgl::Uniform2F*                     sizeFactorUniform;

sgl::Texture2D* CreateRenderTarget(Texture::FORMAT format, size_t width, size_t height)
{
    sgl::Texture2D* texture = device->CreateTexture2D();
    texture->SetImage(format, width, height, 0);
    return texture;
}

void RenderCommon(float time)
{
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
    {
	    std::ostringstream ss;
	    ss << "FPS: " << fps << std::endl;

	    if ( sgl::SGL_OK == font->Bind(10, 12) )
	    {
		    font->Print( screenWidth / 2.0f - 40.0f, 10.0f, ss.str() );
		    font->Unbind();
	    }
    }

    // Print additional info
    {
        std::ostringstream ss;
        ss << "Space - switch rendering technique.\n"
           << "Left/Right/Up/Down - Increse/Decrease detail.\n"
           << "Render technique: ";
        switch (renderTechnique)
        {
        case PBO_COPY:
            ss << "PBO copy\n";
            break;

        case VERTEX_TEXTURE_FETCH:
            ss << "Vertex texture fetch\n";
            break;

        default:
            assert(!"Can't get here");
        }

        ss << "size_x: " << sizeX << std::endl;
        ss << "size_y: " << sizeY << std::endl;

	    if ( sgl::SGL_OK == font->Bind(10, 12) )
	    {
		    font->Print( 10.0f, 10.0f, ss.str() );
		    font->Unbind();
	    }
    }
}

void RenderToSphere()
{
    // resize texture if necessary
    sgl::Texture2D* rtTexture = vboRenderTarget->ColorAttachment(0);
    if ( static_cast<unsigned>(sizeX) != rtTexture->Width()
         || static_cast<unsigned>(sizeY) != rtTexture->Height() )
    {
        rtTexture = CreateRenderTarget(Texture::RGBA32F, sizeX, sizeY);
        vboRenderTarget->SetColorAttachment(rtTexture, 0);
        verticesMapUniform->Set(0, rtTexture);
        sizeFactorUniform->Set( Vector2f(max_texture_width, max_texture_height) / Vector2f(sizeX, sizeY) );
    }

    // render
    device->SetViewport( sgl::rectangle(0, 0, sizeX, sizeY) );
    vboRenderTarget->Bind();
    sphereProgram->Bind();

    quadVBO->Bind();
    quadVBO->Draw(sgl::QUADS, 0, 4);
    quadVBO->Unbind();

    sphereProgram->Unbind();
    vboRenderTarget->Unbind();
    device->SetViewport( sgl::rectangle( 0, 0, static_cast<unsigned int>(screenWidth), static_cast<unsigned int>(screenHeight) ) );
}

void RenderScene()
{
    ffpProgram->Bind();

    switch (renderTechnique)
    {
    case PBO_COPY:
        sphereVerticesVBO->Bind();
        sphereVerticesVBO->Draw(sgl::POINTS, 0, sizeX * sizeY);
        sphereVerticesVBO->Unbind();
        break;

    case VERTEX_TEXTURE_FETCH:
        vtfProgram->Bind();
        vtfVBO->Bind();
        for (int i = 0; i<sizeY; ++i) {
            vtfVBO->Draw(sgl::POINTS, i * max_texture_width, sizeX);
        }
        vtfVBO->Unbind();
        vtfProgram->Unbind();
        break;

    default:
        assert(!"Can't get here");
    }
}


void HandleKeys()
{
    if ( keys[SDLK_SPACE] )
    {
        if (renderTechnique == PBO_COPY)
        {
            renderTechnique = VERTEX_TEXTURE_FETCH;
            vboRenderTarget->SetVBOAttachment(0, 0);
        }
        else
        {
            renderTechnique = PBO_COPY;
            vboRenderTarget->SetVBOAttachment( 0, sphereVerticesVBO.get() );
        }

        keys[SDLK_SPACE] = false;
    }

    if ( keys[SDLK_RIGHT] )
    {
        sizeX = std::min(sizeX << 1, max_texture_width);
        keys[SDLK_RIGHT] = false;
    }
    if ( keys[SDLK_LEFT] )
    {
        sizeX = std::max(sizeX >> 1, 1);
        keys[SDLK_LEFT] = false;
    }
    if ( keys[SDLK_UP] )
    {
        sizeY = std::min(sizeY << 1, max_texture_height);
        keys[SDLK_UP] = false;
    }
    if ( keys[SDLK_DOWN] )
    {
        sizeY = std::max(sizeY >> 1, 1);
        keys[SDLK_DOWN] = false;
    }
}

void Render()
{
    // render
    device->Clear();

    RenderToSphere();
    RenderScene();

	float curTime = SDL_GetTicks() / 1000.0f;
    RenderCommon(curTime);

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
        HandleEvents();
        HandleKeys();
        Render();
    }
}

void CreateScene()
{
	device.reset( sglCreateDevice(sgl::OPENGL_DEVICE) );

    screenWidth  = 800.0f;
    screenHeight = 600.0f;

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 && !SDL_GetVideoInfo() ) {
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
        throw std::runtime_error( (std::string("Can't set video mode: ") + SDL_GetError()).c_str() );
    }

    // grab current context for rendering
    device->GrabCurrentContext();

    // Setup error handler
	sglSetErrorHandler( new sgl::PrintErrorHandler() );

	// Create states
    modelViewMatrix  = math::Mat4f::translate(0.0f, 0.0f, 2.0f);
    projectionMatrix = math::Mat4f::perspective( 1.57f, static_cast<float>(screenWidth) / screenHeight, 0.01f, 100.0f);

    ffpProgram.reset( device->FixedPipelineProgram() );
    projectionMatrixUniform = ffpProgram->GetProjectionMatrixUniform();
    modelViewMatrixUniform  = ffpProgram->GetModelViewMatrixUniform();

    projectionMatrixUniform->Set(projectionMatrix);
    modelViewMatrixUniform->Set(modelViewMatrix);

	// create gui
    font.reset( device->CreateFont() );
	font->LoadFromFile("data/Fonts/font.png");

    // screen quad
    math::vector_of_vector4f vertices;
    vertices.push_back( Vector4f(0.0, 0.0, 0.0, 0.0) );
    vertices.push_back( Vector4f(0.0, 1.0, 0.0, 1.0) );
    vertices.push_back( Vector4f(1.0, 1.0, 1.0, 1.0) );
    vertices.push_back( Vector4f(1.0, 0.0, 1.0, 0.0) );

    sgl::VertexDeclaration* vDecl = device->CreateVertexDeclaration();
    vDecl->AddVertex(2, sgl::FLOAT);
    vDecl->AddTexCoord(0, 2, sgl::FLOAT);

    quadVBO.reset( device->CreateVertexBuffer() );
    quadVBO->SetVertexDecl( vDecl );
	quadVBO->SetData( vertices.size() * vDecl->VertexSize(), &vertices[0] );

    // sphere vbo
    {
        vDecl = device->CreateVertexDeclaration();
        vDecl->AddVertex(4, sgl::FLOAT);

        sphereVerticesVBO.reset( device->CreateVertexBuffer() );
        sphereVerticesVBO->SetVertexDecl(vDecl);
        sphereVerticesVBO->SetData(max_texture_width * max_texture_height * sizeof(Vector4f), NULL);
    }

    // Setup r2vb
    {
        sizeX = 256;
        sizeY = 256;

        // r2vb render target
        vboRenderTarget.reset( device->CreateVBORenderTarget() );
        vboRenderTarget->SetColorAttachment( CreateRenderTarget(Texture::RGBA32F, sizeX, sizeY), 0 );
        vboRenderTarget->SetVBOAttachment( 0, sphereVerticesVBO.get() );

        // shaders
        Shader* vertexShader   = device->CreateShader();
        Shader* fragmentShader = device->CreateShader();

        vertexShader->LoadFromFile(sgl::Shader::VERTEX, "data/Shaders/r2vb.vert");
        fragmentShader->LoadFromFile(sgl::Shader::FRAGMENT, "data/Shaders/r2vb_sphere.frag");

        sphereProgram.reset( device->CreateProgram() );
        sphereProgram->AddShader(vertexShader);
        sphereProgram->AddShader(fragmentShader);

        r2vb_mvpMatrixUniform = sphereProgram->GetUniform4x4F("modelViewProjectionMatrix");
        r2vb_mvpMatrixUniform->Set( Mat4f::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f) );
    }

    // Setup vtf
    {
        Shader* vertexShader   = device->CreateShader();
        Shader* fragmentShader = device->CreateShader();

        vertexShader->LoadFromFile(sgl::Shader::VERTEX, "data/Shaders/vtf.vert");
        fragmentShader->LoadFromFile(sgl::Shader::FRAGMENT, "data/Shaders/fill.frag");

        vtfProgram.reset( device->CreateProgram() );
        vtfProgram->AddShader(vertexShader);
        vtfProgram->AddShader(fragmentShader);

        verticesMapUniform   = vtfProgram->GetSamplerUniform2D("verticesMap");
        vtf_mvpMatrixUniform = vtfProgram->GetUniform4x4F("modelViewProjectionMatrix");
        sizeFactorUniform    = vtfProgram->GetUniform2F("sizeFactor");

        vtf_mvpMatrixUniform->Set(projectionMatrix * modelViewMatrix);
        verticesMapUniform->Set( 0, vboRenderTarget->ColorAttachment(0) );
        sizeFactorUniform->Set( Vector2f(max_texture_width, max_texture_height) / Vector2f(sizeX, sizeY) );

        // create vtf VBO
        std::vector<Vector2f> vtfVertices(max_texture_width * max_texture_width);
        for (int i = 0; i<max_texture_width; ++i)
        {
            for (int j = 0; j<max_texture_width; ++j)
            {
                vtfVertices[i * max_texture_width + j].y = static_cast<float>(i) / (max_texture_height - 1);
                vtfVertices[i * max_texture_width + j].x = static_cast<float>(j) / (max_texture_width - 1);
            }
        }

        vDecl = device->CreateVertexDeclaration();
        vDecl->AddVertex(2, sgl::FLOAT);

        vtfVBO.reset( device->CreateVertexBuffer() );
        vtfVBO->SetVertexDecl(vDecl);
        vtfVBO->SetData( vtfVertices.size() * sizeof(Vector2f), &vtfVertices[0] );
    }
}

bool ParseCommandLine(int /*argc*/, char** /*argv*/)
{

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
