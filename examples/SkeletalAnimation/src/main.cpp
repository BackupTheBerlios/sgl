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

// sgl
sgl::ref_ptr<sgl::Device>   device;

// gui
sgl::ref_ptr<sgl::Font>     font;
math::Vector2f              cursorPos;

// render
class RoboHand;
sgl::ref_ptr<RoboHand>      roboHand;

struct Bone
#ifdef SIMPLE_GL_USE_SSE
    : public sgl::Aligned16
#endif
{
    math::Quaternionf   orient;
    math::Vector3f      origin;

    Bone() {}

    Bone( const math::Quaternionf& _orient,
          const math::Vector3f&    _origin ) :
        orient(_orient),
        origin(_origin)
    {}
};

struct skin_vertex
{
    math::Vector3f      weights;
    math::FPUVector4f   position_index[3];
};

class RoboHand :
    public ReferencedImpl<Referenced>
{
private:
    typedef sgl::vector< Bone,
                         sgl::aligned_allocator<Bone> > bone_vector;

public:
    RoboHand( float    length      = 1.0f,
              float    width       = 0.1f,
              unsigned numBones    = 10,
              unsigned numVertices = 100 ) :
        bones(numBones)
    {
        assert( numBones >= 4 );
        assert( !(numVertices & 1) && "Number of vertices must be even" );

        // create skeleton
        {
            math::Vector3f origin(length / numBones, 0.0f, 0.0f);
            bones[0] = Bone( math::Quaternionf(1.0f, 0.0f, 0.0f, 0.0f), math::Vector3f(0.0f, 0.0f, 0.0f) );
            for (size_t i = 1; i<numBones; ++i) {
                bones[i] = Bone( math::Quaternionf(1.0f, 0.0f, 0.0f, 0.0f), origin);
            }   

            // calculate world transforms
            math::vector_of_vector3f      boneOrigins(numBones);
            math::vector_of_quaternionf   boneOrients(numBones);
            calculateWorldTransforms( boneOrigins.begin(), boneOrients.begin() );

            // create vertices
            std::vector<skin_vertex> vertices(numVertices);
            origin = math::Vector3f(0.0f, 0.0f, 0.0f);
            for (size_t i = 0; i<numVertices; ++i)
            {
                if (i < numVertices / 2) 
                {
                    origin.x = 2.0f * i * length / numVertices; 
                    origin.y = width * powf(1.0f - origin.x / length, 0.5f);
                }
                else 
                {
                    origin.x = 2.0f * (numVertices - i - 1) * length / numVertices;
                    origin.y = -width * powf(1.0f - origin.x / length, 0.5f);
                }

                // find nearest bone
                float minDist     = std::numeric_limits<float>::max();
                int   nearestBone = 1;
                for (size_t j = 1; j<numBones; ++j) 
                {
                    float dist = math::length(boneOrigins[j] - origin);
                    if (dist < minDist) 
                    {
                        minDist     = dist;
                        nearestBone = j;
                    }
                }

                // find 3 nearest bones
                int bone0 = std::max<int>(nearestBone - 1, 1);
                int bone1 = nearestBone;
                int bone2 = std::min<int>(nearestBone + 1, numBones - 1);

                // calculate weights
                float weights[3];
                weights[0] = math::length(origin - boneOrigins[bone0]);
                weights[1] = math::length(origin - boneOrigins[bone1]);
                weights[2] = math::length(origin - boneOrigins[bone2]);

                float weightSum = weights[0] + weights[1] + weights[2];
                vertices[i].weights[0] = 0.5f * (weightSum - weights[0]) / weightSum;
                vertices[i].weights[1] = 0.5f * (weightSum - weights[1]) / weightSum;
                vertices[i].weights[2] = 0.5f * (weightSum - weights[2]) / weightSum;

                vertices[i].position_index[0] = math::make_vec( origin - boneOrigins[bone0], float(bone0) );
                vertices[i].position_index[1] = math::make_vec( origin - boneOrigins[bone1], float(bone1) );
                vertices[i].position_index[2] = math::make_vec( origin - boneOrigins[bone2], float(bone2) );
            }

            // create buffer for skinned mesh
            VertexDeclaration* vDecl = device->CreateVertexDeclaration();
            vDecl->AddVertex(3, FLOAT);
            vDecl->AddTexCoord(3, 4, FLOAT);
            vDecl->AddTexCoord(4, 4, FLOAT);
            vDecl->AddTexCoord(5, 4, FLOAT);

            vertexBuffer.reset( device->CreateVertexBuffer() );
            vertexBuffer->SetVertexDecl(vDecl);
            vertexBuffer->SetData( vertices.size() * sizeof(skin_vertex), &vertices[0] );

            // create buffer for ckeleton
            vDecl = device->CreateVertexDeclaration();
            vDecl->AddVertex(3, FLOAT);

            skeletonBuffer.reset( device->CreateVertexBuffer() );
            skeletonBuffer->SetVertexDecl(vDecl);
        }

        // create programs
        {
            // shaders
            Shader* vertexShader   = device->CreateShader();
            Shader* fragmentShader = device->CreateShader();

            vertexShader->LoadFromFile(sgl::Shader::VERTEX, "data/Shaders/skeletal.vert");
            fragmentShader->LoadFromFile(sgl::Shader::FRAGMENT, "data/Shaders/fill.frag");

            skeletalProgram.reset( device->CreateProgram() );
            skeletalProgram->AddShader(vertexShader);
            skeletalProgram->AddShader(fragmentShader);

            // create uniforms
            mvpMatrixUniform = skeletalProgram->GetUniform4x4F("modelViewProjectionMatrix");
            colorUniform     = skeletalProgram->GetUniform4F("color");
            boneQuatUniform  = skeletalProgram->GetUniform4F("boneQuat", 100);
            bonePosUniform   = skeletalProgram->GetUniform3F("bonePos", 100);
        }

        // default values
        lastIKBone = bones.size() - 1;
    }

    void animateIK(math::Vector2f target, float dt)
    {
        for(size_t i = 1; i<bones.size(); ++i)
        {
            math::vector_of_quaternionf boneOrients( bones.size() );
            math::vector_of_vector3f    boneOrigins( bones.size() );
            calculateWorldTransforms( boneOrigins.begin(), boneOrients.begin() );

            // calculate rotation angle
            math::Vector2f boneVec  = normalize( math::xy(boneOrigins[i] - boneOrigins[i - 1]) );
            math::Vector2f force    = target - math::xy(boneOrigins[i]);
            float          alpha    = -dot( math::Vector2f(-boneVec.y, boneVec.x), force ) * dt;// * powf(i, 0.02f) * 2.0f;

            // rotate around z
            bones[i].orient *= math::Quaternionf( cos(alpha * 0.5f), 0.0f, 0.0f, sin(alpha * 0.5f) );
            bones[i].orient  = math::normalize(bones[i].orient);
        }
    }

    void render(const math::Matrix4f& mvpMatrix)
    {
        mvpMatrixUniform->Set(mvpMatrix);

        // calculate world transforms
        math::vector_of_quaternionf boneOrients( bones.size() );
        math::vector_of_vector4f    boneOrientVecs( bones.size() );
        math::vector_of_vector3f    boneOrigins( bones.size() );
        calculateWorldTransforms( boneOrigins.begin(), boneOrients.begin() );

        // transform quats
        for (size_t i = 0; i<boneOrients.size(); ++i) {
            boneOrientVecs[i] = math::Vector4f(-boneOrients[i].x, -boneOrients[i].y, -boneOrients[i].z, boneOrients[i].w);
        }

        // setup uniforms
        colorUniform->Set( math::Vector4f(0.0f, 1.0f, 0.0f, 1.0f) );
        boneQuatUniform->Set( &boneOrientVecs[0], boneOrientVecs.size() );
        bonePosUniform->Set( &boneOrigins[0], boneOrigins.size() ); 

        skeletalProgram->Bind();
        vertexBuffer->Bind();
        vertexBuffer->Draw(LINE_STRIP);
        skeletalProgram->Unbind();

        // render skeleton
        skeletonBuffer->SetData(boneOrigins.size() * sizeof(math::Vector3f), &boneOrigins[0].x);
        skeletonBuffer->Bind();
        skeletonBuffer->Draw(LINE_STRIP);
    }

private:
    template<typename OutOriginIter, typename OutOrientIter>
    void calculateWorldTransforms(OutOriginIter originIter, OutOrientIter orientIter)
    {
        math::Vector3f      origin = (*originIter++) = bones[0].origin;
        math::Quaternionf   orient = (*orientIter++) = bones[0].orient;
        
        // calculate world-space transforms
        for (size_t i = 1; i<bones.size(); ++i) 
        {
            orient *= bones[i].orient;
            origin += orient * bones[i].origin;
            (*originIter++) = origin;
            (*orientIter++) = orient;
        }
    }

private:
    bone_vector             bones;
    ref_ptr<VertexBuffer>   vertexBuffer;
    ref_ptr<VertexBuffer>   skeletonBuffer;

    // animation
    unsigned int    lastIKBone;

    // shaders
    sgl::ref_ptr<sgl::Program>  skeletalProgram;

    sgl::Uniform4x4F*           mvpMatrixUniform;
    sgl::Uniform4F*             colorUniform;
    sgl::Uniform4F*             boneQuatUniform;
    sgl::Uniform3F*             bonePosUniform;
};

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
    }
}


void RenderScene()
{
    // animate robo hand
    static float lastUpdate = SDL_GetTicks() / 1000.0f;
    float curTime = SDL_GetTicks() / 1000.0f;
    if (curTime - lastUpdate > 0.01f) 
    {
        roboHand->animateIK(cursorPos, curTime - lastUpdate);
        lastUpdate = curTime;
    }
    
    // render robo hand
    roboHand->render( Mat4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f) );
}


void HandleKeys()
{
}

void Render()
{
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
        case SDL_MOUSEMOTION:
            cursorPos = math::Vector2f(event.motion.x, event.motion.y) / math::Vector2f(screenWidth, screenHeight);
            cursorPos = 2.0f * math::Vector2f(cursorPos.x, 1.0f - cursorPos.y) - math::Vector2f(1.0f, 1.0f);
            break;

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
    while (working)
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

    // create gui
    font.reset( device->CreateFont() );
	font->LoadFromFile("data/Fonts/font.png");

    // create robo hand
    roboHand.reset( new RoboHand(0.7f) );
}

bool ParseCommandLine(int /*argc*/, char** /*argv*/)
{

    return true;
}

int main(int argc, char** argv)
{
    math::Matrix4f a = math::Mat4f::identity();
    math::Matrix4f b = math::Mat4f::scale(10.0f, 4.0f, 3.0f);
    a *= b;

    if ( !ParseCommandLine(argc, argv) ) {
        return 1;
    }
    CreateScene();
    Run();

    return 0;
}
