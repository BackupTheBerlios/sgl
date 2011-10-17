#include "GL/GLCommon.h"
#include "GL/GLDevice.h"
#include "GL/GLFont.h"
#include "Math/Containers.hpp"
#include "Math/Matrix.hpp"

namespace {

    using namespace sgl;
    using namespace std;
    using namespace math;

    const char* font_vertex_shader = "\
        uniform mat4 mvp;\
        \
        attribute vec4 position;\
        attribute vec2 texcoord;\
        \
        varying vec2 fp_texcoord;\
        \
        void main()\
        {\
            fp_texcoord = texcoord;\
            gl_Position = mvp * position;\
        }";

    const char* font_fragment_shader = "\
        uniform vec4      color;\
        uniform sampler2D texture;\
        \
        varying vec2 fp_texcoord;\
        \
        void main()\
        {\
            gl_FragColor = color * texture2D(texture, fp_texcoord);\
        }";

} // anonymous namespace

namespace sgl {

GLFont::GLFont(GLDevice* device_) :
    device(device_)
{
    {
        RasterizerState::DESC desc;
        desc.colorMask  = RasterizerState::RGBA;
        desc.cullMode   = RasterizerState::NONE;
        desc.fillMode   = RasterizerState::SOLID;
        rasterizerState.reset( device->CreateRasterizerState(desc) );
    }

    {
        BlendState::DESC_SIMPLE desc;
        desc.blendEnable = true;
        desc.srcBlend    = BlendState::ONE;
        desc.destBlend   = BlendState::ONE_MINUS_SRC_ALPHA;
        desc.blendOp     = BlendState::ADD;
        blendState.reset( device->CreateBlendState(Extend(desc)) );
    }

    {
        DepthStencilState::DESC desc;
        desc.depthEnable    = false;
        desc.depthWriteMask = 0;
        desc.stencilEnable  = false;
        depthStencilState.reset( device->CreateDepthStencilState(desc) );
    }

    vbo.reset( device->CreateVertexBuffer() );
    if (!vbo) {
	    throw gl_error("Can't create vertex buffer for font rendering.");
    }

    const size_t maxStrLength = 1024;
    const size_t bufSize      = maxStrLength * 96 * sizeof(float);
    if ( SGL_OK != vbo->SetData(bufSize, 0, Buffer::DYNAMIC_DRAW) ) {
	    throw gl_error("Can't allocate vertex buffer for font rendering.");
    }
}

GLFont::~GLFont()
{
}

SGL_HRESULT GLFont::SetTexture(sgl::Texture2D* texture_)
{
    texture.reset(texture_);
    if (texture)
    {
        bool        haveMipmaps = true;
        SGL_HRESULT result      = texture->GenerateMipmap();
	    if (SGL_OK != result) {
		    haveMipmaps = false;
	    }

        SamplerState::DESC desc;

        desc.filter[0]   = SamplerState::LINEAR;
        desc.filter[1]   = SamplerState::LINEAR;
        desc.filter[2]   = haveMipmaps ? SamplerState::LINEAR : SamplerState::NONE;

        desc.wrapping[0] = SamplerState::REPEAT;
        desc.wrapping[1] = SamplerState::REPEAT;

        texture->BindSamplerState( device->CreateSamplerState(desc) );
    }

    return SGL_OK;
}

void GLFont::Print(float x, float y, const char* str) const
{
    data.clear();
    {
        math::Matrix4f transform(charWidth, 0.0f,       0.0f,       x,
                                 0.0f,      charHeight, 0.0f,       y,
                                 0.0f,      0.0f,       1.0f,       0.0f,
                                 0.0f,      0.0f,       0.0f,       1.0f);
        math::Vector4f position[6];
        {
            position[0] = math::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
            position[1] = math::Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
            position[2] = math::Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
            position[3] = math::Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
            position[4] = math::Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
            position[5] = math::Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
        }
        math::Vector2f texcoord[6];
        
	    for(; *str; ++str)
	    {
		    if (*str == '\n')
		    {
                transform[0][3]  = x;
                transform[1][3] += charHeight;
		    }
		    else if (*str == '\r')
		    {
                transform[0][3] = x;
		    }
		    else if (*str == '\t')
		    {
                transform[0][3] += charWidth * 2.0f;
		    }
		    else if (*str == ' ')
		    {
                transform[0][3] += charWidth * 0.5f;
		    }
		    else
		    {
                unsigned char c = *reinterpret_cast<const unsigned char*>(str);

	            const float ds = 1.0f / 16.0f;
	            const float dt = 1.0f / 16.0f;

                float s = float(c % 16) / 16.0f;
                float t = float(c / 16) / 16.0f;

                texcoord[0] = math::Vector2f(s,      t);
                texcoord[1] = math::Vector2f(s + ds, t);
                texcoord[2] = math::Vector2f(s + ds, t + dt);
                texcoord[3] = math::Vector2f(s + ds, t + dt);
                texcoord[4] = math::Vector2f(s + ds, t);
                texcoord[5] = math::Vector2f(s,      t + dt);

                for (int j = 0; j<6; ++j)
                {
                    math::Vector4f pos = transform * position[j];
                    data.push_back(pos.x);
                    data.push_back(pos.y);
                    data.push_back(pos.z);
                    data.push_back(pos.w);
                    data.push_back(texcoord[j].x);
                    data.push_back(texcoord[j].y);
                }

                transform[0][3] += charWidth * 0.666667f;
		    }
	    }
    }

    size_t bufSize = data.size() * sizeof(float);
    if ( bufSize > vbo->Size() ) {
        vbo->SetData(bufSize, &data[0], Buffer::DYNAMIC_DRAW);
    }
    else {
        vbo->SetSubData(0, bufSize, &data[0]);
    }

    device->Draw(TRIANGLES, 0, data.size() / 6);
}

sgl::Texture2D* GLFont::Texture() const
{
    return texture.get();
}

GLFontFixed::GLFontFixed(GLDevice* device) :
	GLFont(device)
{
	// create states
	ffpProgram.reset( device->FixedPipelineProgram() );
	projectionMatrixUniform = ffpProgram->GetProjectionMatrixUniform();
	modelViewMatrixUniform  = ffpProgram->GetModelViewMatrixUniform();
	lightingEnableUniform   = ffpProgram->GetLightingToggleUniform();
	ambientUniform          = ffpProgram->GetAmbientUniform();
	materialAmbientUniform  = ffpProgram->GetMaterialAmbientUniform();

	{
		VertexLayout::ELEMENT elements[] =
		{
			{0, 4, 0,  24, sgl::FLOAT, VertexLayout::VERTEX},
			{0, 2, 16, 24, sgl::FLOAT, VertexLayout::TEXCOORD}
		};
		vertexLayout.reset( device->CreateVertexLayout(2, elements) );
	}
}

SGL_HRESULT GLFontFixed::SetTexture(sgl::Texture2D* texture)
{
    SGL_HRESULT result = GLFont::SetTexture(texture);
	if (SGL_OK != result) {
		return result;
	}

	return SGL_OK;
}

SGL_HRESULT GLFontFixed::Bind(int width, int height, const math::Vector4f& color) const
{
	rectangle vp = device->Viewport();
	vpWidth      = float(vp.width);
	vpHeight     = float(vp.height);
	charWidth    = float(width);
	charHeight   = float(height);

	// setup states
	projectionMatrix = projectionMatrixUniform->Value();
	modelViewMatrix  = modelViewMatrixUniform->Value();

    modelViewMatrixUniform->Set( Matrix4f::identity() );
    projectionMatrixUniform->Set( Matrix4f::ortho( 0.0f, vpWidth,
		                                           vpHeight, 0.0f,
		                                           -1.0f, 1.0f ) );

    device->PushState(State::RASTERIZER_STATE);
	device->PushState(State::DEPTH_STENCIL_STATE);
	device->PushState(State::BLEND_STATE);

	ffpProgram->Bind();
    rasterizerState->Bind();
	depthStencilState->Bind();
	blendState->Bind();
	vbo->Bind( vertexLayout.get() );

	return texture->Bind(0);
}

void GLFontFixed::Unbind() const
{
	vbo->Unbind();
	texture->Unbind();
    device->PopState(State::RASTERIZER_STATE);
	device->PopState(State::DEPTH_STENCIL_STATE);
	device->PopState(State::BLEND_STATE);

	projectionMatrixUniform->Set(projectionMatrix);
	modelViewMatrixUniform->Set(modelViewMatrix);
}

GLFontProgrammable::GLFontProgrammable(GLDevice* device) :
	GLFont(device)
{
	// create states
	program.reset( device->CreateProgram() );
	{
		Shader::DESC desc;
		desc.source = font_vertex_shader;
		desc.type   = Shader::VERTEX;
		sgl::Shader* vertexShader = device->CreateShader(desc);
		if (!vertexShader) {
			throw gl_error("GLFont::GLFont failed. Can't create vertex shader for font.");
		}

		desc.source = font_fragment_shader;
		desc.type   = Shader::FRAGMENT;
		sgl::Shader* fragmentShader = device->CreateShader(desc);
		if (!fragmentShader) {
			throw gl_error("GLFont::GLFont failed. Can't create fragment shader for font.");
		}

		program->AddShader(vertexShader);
		program->AddShader(fragmentShader);
		if ( SGL_OK != program->Dirty() ) {
			throw gl_error("GLFont::GLFont failed. Can't create shader program for font.");
		}
	}
	textureUniform = program->GetSamplerUniform2D("texture");
	colorUniform   = program->GetUniform4F("color");
	mvpUniform     = program->GetUniform4x4F("mvp");

	// create vertex layout
	{
		int positionLoc = program->AttributeLocation("position");
		if (positionLoc == -1) {
			throw gl_error("GLFont::GLFont failed. Can't get location of font attribute.");
		}

		int texcoordLoc = program->AttributeLocation("texcoord");
		if (texcoordLoc == -1) {
			throw gl_error("GLFont::GLFont failed. Can't get location of font attribute.");
		}

		VertexLayout::ELEMENT elements[] =
		{
			{positionLoc, 4, 0,  24, sgl::FLOAT, VertexLayout::ATTRIBUTE},
			{texcoordLoc, 2, 16, 24, sgl::FLOAT, VertexLayout::ATTRIBUTE},
		};
		vertexLayout.reset( device->CreateVertexLayout(2, elements) );
	}
}

SGL_HRESULT GLFontProgrammable::Bind(int width, int height, const math::Vector4f& color) const
{
	rectangle vp = device->Viewport();
	vpWidth      = float(vp.width);
	vpHeight     = float(vp.height);
	charWidth    = float(width);
	charHeight   = float(height);

	program->Bind();
	colorUniform->Set(color);
	textureUniform->Set( 0, texture.get() );
    mvpUniform->Set( Matrix4f::ortho( 0.0f, vpWidth,
		                              vpHeight, 0.0f,
		                              -1.0f, 1.0f ) );

	device->PushState(State::RASTERIZER_STATE);
	device->PushState(State::DEPTH_STENCIL_STATE);
	device->PushState(State::BLEND_STATE);

    rasterizerState->Bind();
	depthStencilState->Bind();
	blendState->Bind();
	vbo->Bind( vertexLayout.get() );

	return SGL_OK;
}

void GLFontProgrammable::Unbind() const
{
	vbo->Unbind();
	texture->Unbind();
	device->PopState(State::RASTERIZER_STATE);
	device->PopState(State::DEPTH_STENCIL_STATE);
	device->PopState(State::BLEND_STATE);
	program->Unbind();
}

} // namespace sgl
