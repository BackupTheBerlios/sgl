#include "Device.h"
#include "Utility/FX/SharedUniform.h"
#include "Utility/FX/UberShaderProgram.h"
#include "Tables.h"
#include "Image.h"
#include "SDL.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>

#include "GL/GLProgram.h"
#undef CreateFont
#undef POINTS
#undef FLOAT

bool	working;
bool    mouseDown;
bool	keys[1024];
float	fps;
float	screenWidth;
float	screenHeight;

using namespace std;
using namespace sgl;
using namespace math;

// sgl
ref_ptr<Device> device;

// gui
ref_ptr<Font> font;

// states
ref_ptr<VertexColorState>         vertexColorState;
ref_ptr<ProjectionMatrixState>    projectionMatrixState;
ref_ptr<ModelViewMatrixState>     modelViewMatrixState;

// 3d volume
std::vector<string>     imageFileNames;
ref_ptr<Texture3D>      volumeTexture;
ref_ptr<VertexBuffer>   volumeVertices;
ref_ptr<VertexBuffer>   streamOutVertices;

// shaders
ref_ptr<Program>        generateProgram;
ref_ptr<Program>        drawProgram;
SharedUniform3F         vertDecalsUniform;
SharedUniform3F         vertDecalsNormUniform;
SharedUniform3F         dataStepUniform;
SharedUniformF          isoLevelUniform;
SharedSamplerUniform3D  volumeUniform;
SharedSamplerUniform1D  edgeTableUniform;
SharedSamplerUniform2D  triTableUniform;

void HandleInput()
{
    if ( keys[SDLK_UP] )
    {
        float isoLevel = isoLevelUniform.Value();
        isoLevelUniform.Set(isoLevel + 0.01f);
    }
    if ( keys[SDLK_DOWN] )
    {
        float isoLevel = isoLevelUniform.Value();
        isoLevelUniform.Set(isoLevel - 0.01f);
    }
    if ( keys[SDLK_ESCAPE] )
    {
        working = false;
        keys[SDLK_ESCAPE] = false;
    }
}


void RenderCommon(float time)
{
    // font color
    vertexColorState->SetColor(1.0, 1.0, 0.0, 1.0);
    vertexColorState->Setup();

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
	std::ostringstream ss;
	ss << "FPS: " << fps << std::endl;

	if ( sgl::SGL_OK == font->Bind(10, 12) )
	{
		font->Print( screenWidth / 2.0f - 40.0f, 10.0f, ss.str() );
		font->Unbind();
	}
}

void RenderScene()
{
    generateProgram->Bind();

    volumeVertices->Bind();
    volumeVertices->Draw(sgl::POINTS, 0);
    volumeVertices->Unbind();
/*
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    glVertex3f(-0.1f, -0.1f, 0.0f);
    glVertex3f(-0.1f, 0.1f, 0.0f);
    glVertex3f(0.1f, 0.1f, 0.0f);
    glVertex3f(0.1f, -0.1f, 0.0f);
    glEnd();
*/
    generateProgram->Unbind();
    //volumeTexture->Unbind();
}

void Render()
{
    glDisable(GL_CULL_FACE);

    // render
    device->Clear();

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

        case SDL_MOUSEMOTION:
        {
            //cout << event.motion.x << " " << event.motion.y << endl;
            if (mouseDown)
            {
                modelViewMatrixState->SetMatrix( modelViewMatrixState->Transform() * Mat4f::rotate_z(event.motion.xrel / 150.0f) );
                modelViewMatrixState->SetMatrix( modelViewMatrixState->Transform() * Mat4f::rotate_x(event.motion.yrel / 150.0f) );
                modelViewMatrixState->Setup();
            }
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            if(event.button.button == SDL_BUTTON_LEFT) {
                mouseDown = true;
            }
            break;
        }

        case SDL_MOUSEBUTTONUP:
        {
            if(event.button.button == SDL_BUTTON_LEFT) {
                mouseDown = false;
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
    mouseDown = false;
    while(working)
    {
        HandleEvents();
        HandleInput();
        Render();
    }
}

// Create volume texture from the list of 2d textures
sgl::Texture3D* CreateVolumeTexture( const std::vector<std::string>& imageFileNames,
                                     Texture::FORMAT format = Texture::UNKNOWN,
                                     unsigned int maxTextureWidth  = 512,
                                     unsigned int maxTextureHeight = 512,
                                     unsigned int maxTextureDepth  = 512,
                                     FormatConversion* conversion = 0 )
{

    std::vector< ref_ptr<Image> > images( imageFileNames.size() );

    unsigned int maxWidth  = 0;
    unsigned int maxHeight = 0;
    unsigned int depth     = 0;
    for(size_t i = 0; i<imageFileNames.size(); ++i)
    {
        images[i].reset( device->CreateImage() );
        if ( SGL_OK != images[i]->LoadFromFile( imageFileNames[i].c_str() ) ) {
            images[i].reset();
        }
        else if (format == Texture::UNKNOWN) {
            format = images[i]->Format();
        }

        maxWidth  = std::max( maxWidth, images[i]->Width() );
        maxHeight = std::max( maxHeight, images[i]->Height() );
        depth    += images[i]->Depth();
    }

    // compute nearest powers of two for each axis.
    unsigned int xNearestPowerOfTwo = 1;
    unsigned int yNearestPowerOfTwo = 1;
    unsigned int zNearestPowerOfTwo = 1;

    while(xNearestPowerOfTwo < maxWidth && xNearestPowerOfTwo < maxTextureWidth ) {
        xNearestPowerOfTwo *= 2;
    }
    while(yNearestPowerOfTwo < maxHeight && yNearestPowerOfTwo < maxTextureHeight) {
        yNearestPowerOfTwo *= 2;
    }
    while(zNearestPowerOfTwo < depth && zNearestPowerOfTwo < maxTextureDepth) {
        zNearestPowerOfTwo *= 2;
    }

    // now allocate the 3d texture;
    ref_ptr<Image> image3D( device->CreateImage() );
    image3D->SetImage( format,
                       xNearestPowerOfTwo,
                       yNearestPowerOfTwo,
                       zNearestPowerOfTwo );

    // copy across the values from the source images into the image_3d.
    unsigned int zOffset = (depth < zNearestPowerOfTwo) ? zNearestPowerOfTwo / 2 - depth / 2 : 0;
    for(size_t i = 0; i<images.size(); ++i)
    {
        if (images[i]) {
            image3D->CopySubImage(images[i].get(), 0, 0, 0, zOffset + i, conversion);
        }
    }

    // clear unfilled data
    unsigned int sliceSize = xNearestPowerOfTwo * yNearestPowerOfTwo * Texture::FORMAT_TRAITS[format].sizeInBits / 8;
    for (unsigned int i = 0; i<zOffset; ++i)
    {
        memset(image3D->Data() + sliceSize * i, 255, sliceSize);
        memset(image3D->Data() + sliceSize * (zNearestPowerOfTwo - i - 1), 255, sliceSize);
    }

    return image3D->CreateTexture3D();
}

void CreateVolume(size_t sizeX, size_t sizeY, size_t sizeZ)
{
    assert( sizeX > 0 && sizeY > 0 && sizeZ > 0 );

    // step
    float stepX = 2.0f / sizeX;
    float stepY = 2.0f / sizeY;
    float stepZ = 2.0f / sizeZ;

    // create vertices
    std::vector<Vector3f> verts( (sizeX - 1) * (sizeY - 1) * (sizeZ - 1) );
    size_t index = 0;
    float x = -1.0f + stepX * 0.5f;
    for(size_t i = 0; i<sizeX - 1; ++i, x += stepX)
    {
        float y = -1.0f + stepX * 0.5f;
        for(size_t j = 0; j<sizeY - 1; ++j, y += stepY)
        {
            float z = -1.0f + stepX * 0.5f;
            for(size_t k = 0; k<sizeZ - 1; ++k, z += stepZ) {
                verts[index++] = Vector3f(x, y, z);
            }
        }
    }

    // create vbo
    VertexDeclaration* vDecl = device->CreateVertexDeclaration();
    vDecl->AddVertex(3, sgl::FLOAT);

    volumeVertices.reset( device->CreateVertexBuffer() );
    volumeVertices->SetVertexDecl(vDecl);
    volumeVertices->SetData(verts.size() * sizeof(Vector3f), &verts[0]);

    // create vertex decals
    Vector3f vertDecals[8];
    vertDecals[0] = Vector3f(0.0f,  0.0f,  0.0f);
    vertDecals[1] = Vector3f(stepX, 0.0f,  0.0f);
    vertDecals[2] = Vector3f(stepX, stepY, 0.0f);
    vertDecals[3] = Vector3f(0.0f,  stepY, 0.0f);
    vertDecals[4] = Vector3f(0.0f,  0.0f,  stepZ);
    vertDecals[5] = Vector3f(stepX, 0.0f,  stepZ);
    vertDecals[6] = Vector3f(stepX, stepY, stepZ);
    vertDecals[7] = Vector3f(0.0f,  stepY, stepZ);
    vertDecalsUniform.Set(&vertDecals[0], 8);

    Vector3f vertDecalsNorm[8];
    vertDecalsNorm[0] = Vector3f(0.0f,  0.0f,  0.0f);
    vertDecalsNorm[1] = Vector3f(1.0f,  0.0f,  0.0f);
    vertDecalsNorm[2] = Vector3f(-1.0f, 0.0f,  0.0f);
    vertDecalsNorm[3] = Vector3f(0.0f,  0.0f,  0.0f);
    vertDecalsNorm[4] = Vector3f(0.0f,  1.0f,  0.0f);
    vertDecalsNorm[5] = Vector3f(0.0f, -1.0f,  0.0f);
    vertDecalsNorm[6] = Vector3f(0.0f,  0.0f,  1.0f);
    vertDecalsNorm[7] = Vector3f(0.0f,  0.0f, -1.0f);
    vertDecalsNormUniform.Set(&vertDecalsNorm[0], 8);
}

void ConfigureStreamOut()
{
    VertexDeclaration* vDecl = device->CreateVertexDeclaration();
    vDecl->AddAttribute(0, 1, sgl::UBYTE);

    streamOutVertices.reset( device->CreateVertexBuffer() );
    streamOutVertices->SetVertexDecl(vDecl);
}

class AlphaExtracter :
    public FormatConversion
{
public:
    bool Convert( Texture::FORMAT sourceFormat,
                  Texture::FORMAT destFormat,
                  unsigned int numPixels,
                  const void* source,
                  void* dest ) const
    {
        if ( destFormat == Texture::ALPHA8 && sourceFormat == Texture::RGBA8 )
        {
            RGBA_TO_A8(numPixels, source, dest);
            return true;
        }
        return false;
    }

    struct RGBA
    {
        unsigned char r,g,b,a;
    };

    void RGBA_TO_A8(unsigned int numPixels, const void* source, void* dest) const
    {
        const RGBA* rgba     = reinterpret_cast<const RGBA*>(source);
        unsigned char* alpha = reinterpret_cast<unsigned char*>(dest);
        for(unsigned int i = 0; i<numPixels; ++i)
        {
            Vector4i color(rgba[i].r, rgba[i].g, rgba[i].b, rgba[i].a);
            alpha[i] = 255 - static_cast<unsigned char>( (color.w * (color.x + color.y + color.z) / 3) / 255 );
        }
    }
};

void CreateScene()
{
	device.reset( sglCreateDevice(sgl::OPENGL_DEVICE) );

    screenWidth = 800.0f;
    screenHeight = 600.0f;
    device->SetVideoMode( (unsigned int)screenWidth,
						  (unsigned int)screenHeight,
						  32,
						  false,
						  false );

    // get geometry program limits
    GLint max_output_vertices;
    GLint max_total_output_components;
    GLint max_geometry_bindable_uniforms;
    GLint max_bindable_uniform_size;
    glGetProgramivARB(GL_GEOMETRY_PROGRAM_NV, GL_MAX_PROGRAM_OUTPUT_VERTICES_NV, &max_output_vertices);
    glGetProgramivARB(GL_GEOMETRY_PROGRAM_NV, GL_MAX_PROGRAM_TOTAL_OUTPUT_COMPONENTS_NV, &max_total_output_components);
    glGetIntegerv(GL_MAX_GEOMETRY_BINDABLE_UNIFORMS_EXT, &max_geometry_bindable_uniforms);
    glGetIntegerv(GL_MAX_BINDABLE_UNIFORM_SIZE_EXT, &max_bindable_uniform_size);
    printf("max output vertices = %d\n", max_output_vertices);
    printf("max total output components = %d\n", max_total_output_components);
    printf("max geometry bindable uniforms = %d\n", max_geometry_bindable_uniforms);
    printf("max bindable uniform size = %d\n", max_bindable_uniform_size);

    // Setup error handler
	sglSetErrorHandler( new sgl::PrintErrorHandler() );

	// Create states
    vertexColorState.reset( device->CreateVertexColorState() );

    projectionMatrixState.reset( device->CreateProjectionMatrixState() );
    projectionMatrixState->SetMatrix( Mat4f::perspective( 0.7853982f, static_cast<float>(screenWidth) / screenHeight, 0.01f, 10.0f) );
    projectionMatrixState->Setup();

    modelViewMatrixState.reset( device->CreateModelViewMatrixState() );
    modelViewMatrixState->SetMatrix( Mat4f::translate(0.0f, 0.0f, -2.0f) );
    modelViewMatrixState->Setup();

	// create gui
    font.reset( device->CreateFont() );
	font->LoadFromFile("data/Fonts/font.png");

    // create volume
    if ( !imageFileNames.empty() )
    {
        AlphaExtracter conversion;
        volumeTexture.reset( CreateVolumeTexture(imageFileNames, Texture::ALPHA8, 512, 512, 512, &conversion) );
        volumeTexture->GenerateMipmap();
        volumeTexture->SetFilter(Texture::MIN_FILTER, Texture::LINEAR);
        volumeTexture->SetFilter(Texture::MAG_FILTER, Texture::LINEAR);
    }

    // create shaders
    generateProgram.reset( device->CreateProgram() );
    sgl::Shader* vertexShader   = device->CreateShader();
    sgl::Shader* fragmentShader = device->CreateShader();
    sgl::Shader* geometryShader = device->CreateShader();

    const char* vertexShaderDefines[] = {"GENERATE_VOLUME", 0};
    if ( SGL_OK != vertexShader->LoadFromFile(Shader::VERTEX, "data/Shaders/marching_cubes.vert", vertexShaderDefines) ) {
        throw std::runtime_error("Couldn't load shader: data/Shaders/marching_cubes.vert");
    }
    generateProgram->AddShader(vertexShader);

    const char* geometryShaderDefinesDefines[] = {"TRANSFORM_POSITION", 0};
    if ( SGL_OK != geometryShader->LoadFromFile(Shader::GEOMETRY, "data/Shaders/marching_cubes.geom", geometryShaderDefinesDefines) ) {
        throw std::runtime_error("Couldn't load shader: data/Shaders/marching_cubes.geom");
    }
    generateProgram->AddShader(geometryShader);

    if ( SGL_OK != fragmentShader->LoadFromFile( Shader::FRAGMENT, "data/Shaders/marching_cubes.frag" ) ) {
        throw std::runtime_error("Couldn't load shader: data/Shaders/marching_cubes.frag");
    }
    generateProgram->AddShader(fragmentShader);

    // set propertices
    generateProgram->SetGeometryInputType(sgl::POINTS);
    generateProgram->SetGeometryOutputType(sgl::TRIANGLES);
    generateProgram->SetGeometryNumVerticesOut(16);

    // print log
    cout << "Program link log:\n" << generateProgram->CompilationLog();

    // get uniforms
    vertDecalsUniform.AddUniform( generateProgram->GetUniform3F("vertDecals", 8) );
    vertDecalsNormUniform.AddUniform( generateProgram->GetUniform3F("vertDecalsNorm", 8) );
    dataStepUniform.AddUniform( generateProgram->GetUniform3F("dataStep") );
    isoLevelUniform.AddUniform( generateProgram->GetUniformF("isolevel") );
    volumeUniform.AddUniform( generateProgram->GetSamplerUniform3D("dataFieldTex") );
    edgeTableUniform.AddUniform( generateProgram->GetSamplerUniform1D("edgeTableTex") );
    triTableUniform.AddUniform( generateProgram->GetSamplerUniform2D("triTableTex") );

    // setup uniforms
    volumeUniform.Set( 0, volumeTexture.get() );
    dataStepUniform.Set( Vector3f( 1.0f / volumeTexture->Width(), 1.0f / volumeTexture->Height(), 1.0f / volumeTexture->Depth() ) );
    isoLevelUniform.Set(0.11f);

    // setup tables
    Texture1D* edgeTableTexture = device->CreateTexture1D();
    Texture2D* triTableTexture  = device->CreateTexture2D();

    edgeTableTexture->SetImage(Texture::ALPHA32I, 256, edgeTable);
    triTableTexture->SetImage(Texture::ALPHA32I, 16, 256, triTable);

    edgeTableUniform.Set(1, edgeTableTexture);
    triTableUniform.Set(2, triTableTexture);

//    UniformI* uniform = generateProgram->GetUniformI("triTable[1024]");
//    UniformI* uniform = generateProgram->GetUniformI("triTable[1025]");

    /* flattern tri table
    int triTableFlattern[4096];
    for(int i = 0; i<256; ++i)
    {
        for(int j = 0; j<16; ++j) {
            triTableFlattern[i*16 + j] = triTable[i][j];
        }
    }
    //triTableUniform.Set( (const int*)triTable, 4096);

    Vector4i colors[256];
    fill(colors, colors + 256, Vector4i(0));
    colors[33] = Vector4i(255);
    //colors[4091] = 255.0f;

    GLuint buffer;
    glGenBuffersARB( 1, &buffer );
    glBindBufferARB( GL_UNIFORM_BUFFER_EXT, buffer );
    glBufferDataARB( GL_UNIFORM_BUFFER_EXT, 256 * sizeof(Vector4i), colors, GL_STREAM_DRAW_ARB );
    glBindBufferARB( GL_UNIFORM_BUFFER_EXT, 0 );

    generateProgram->Bind();
    GLuint glProgram = static_cast<GLProgram*>( generateProgram.get() )->GLProgramHandle();
    GLint glLocation = glGetUniformLocation(glProgram, "colors");
    glBindBufferARB( GL_UNIFORM_BUFFER_EXT, buffer );
    glUniformBufferEXT( glProgram,
                        glLocation,
                        buffer );
    glBindBufferARB( GL_UNIFORM_BUFFER_EXT, 0 );

    //void* data = glMapBufferARB ( GL_UNIFORM_BUFFER_EXT, GL_WRITE_ONLY );
    //memcpy( data, colors, 10 * sizeof(Vector4f) );
    //glUnmapBufferARB ( GL_UNIFORM_BUFFER_EXT );

    generateProgram->Unbind();*/

    // create volume object
    CreateVolume(32, 32, 32);
}

// Read list of the images from the file
void ReadImagesFile(const std::string& imagesFile)
{
    ifstream file;
    file.open( imagesFile.c_str() );
    if ( !file.is_open() ) {
        std::cerr << "Couldn't open image description file " << imagesFile << std::endl;
    }
    else
    {
        while( !file.eof() )
        {
            string imageFileName;
            file >> imageFileName;
            imageFileNames.push_back(imageFileName);
        }
    }
}

bool ParseCommandLine(int argc, char** argv)
{
    for(int i = 0; i<argc; ++i)
    {
        std::string strArgv(argv[i]);
        if (strArgv == "--images-file" && i + 1 < argc)
        {
            // create volume texture from file with images
            ReadImagesFile(argv[i+1]);
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    if ( !ParseCommandLine(argc, argv) ) {
        return 1;
    }

    // run demo
    try
    {
        CreateScene();
        Run();
    }
    catch(std::exception& ex)
    {
        cerr << "Error occured: " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return 0;
}
