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
        uniform vec2 position;\
        uniform vec2 scale;\
        \
        attribute vec4 position_texcoord;\
        \
        varying vec2 fp_texcoord;\
        \
        void main()\
        {\
            fp_texcoord = position_texcoord.zw;\
            gl_Position = vec4(position + scale * position_texcoord.xy, 1.0, 1.0);\
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
			{0, 2, 0, 16, sgl::FLOAT, VertexLayout::VERTEX},
			{0, 2, 8, 16, sgl::FLOAT, VertexLayout::TEXCOORD}
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

	math::vector_of_vector4f vertices;
	const float ds = 1.0f / 16.0f;
	const float dt = 1.0f / 16.0f;
	for(int y = 0, i = 0; y < 16; y++)
	{
		for(int x = 0; x < 16; x++, i++)
		{
			float s = (float)x / 16.0f;
			float t = (float)y / 16.0f;

			// counter-clockwise because of projection matrix
			vertices.push_back( Vector4f(0.0f, 0.0f, s, t) );
			vertices.push_back( Vector4f(charHeight, 0.0f, s + ds, t) );
			vertices.push_back( Vector4f(charHeight, charHeight, s + ds, t + dt) );
			vertices.push_back( Vector4f(0.0f, charHeight, s, t + dt) );
		}
	}

	vbo.reset( device->CreateVertexBuffer() );
	if (!vbo) {
		return sglGetLastError();
	}
	vbo->SetData( vertices.size() * sizeof(Vector4f), &vertices[0] );

	return SGL_OK;
}

SGL_HRESULT GLFontFixed::Bind(int width, int height, const math::Vector4f& color) const
{
	this->width  = width;
	this->height = height;

	// setup states
	projectionMatrix = projectionMatrixUniform->Value();
	modelViewMatrix  = modelViewMatrixUniform->Value();

	rectangle vp = device->Viewport();
    projectionMatrixUniform->Set( Matrix4f::ortho( 0.0f, vp.width / (width / charHeight),
		                                           vp.height / (height / charHeight), 0.0f,
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

void GLFontFixed::Print(float x, float y, const char* str) const
{
	float charHeight = texture->Height() / 16.0f;
	float charWidth  = texture->Width()  / 24.0f;

	float scaleX = charHeight / width;
	float scaleY = charHeight / height;

	math::Matrix4f modelViewMatrix;
    modelViewMatrixUniform->Set( modelViewMatrix = Matrix4f::translation(x * scaleX, y * scaleY, 0.0f) );
	for(int lines = 0; *str; str++)
	{
		if(*str == '\n')
		{
			lines++;
			modelViewMatrixUniform->Set( modelViewMatrix = Matrix4f::translation(x * scaleX, y * scaleY + charHeight * lines, 0.0f) );
		}
		else if(*str == '\r')
		{
			modelViewMatrixUniform->Set( modelViewMatrix = Matrix4f::translation(x * scaleX, y * scaleY + charHeight * lines, 0.0f) );
		}
		else if(*str == '\t')
		{
			modelViewMatrixUniform->Set( modelViewMatrix *= Matrix4f::translation(charHeight * 2.0f, 0.0f, 0.0f) );
		}
		else if(*str == ' ')
		{
			modelViewMatrixUniform->Set( modelViewMatrix *= Matrix4f::translation(charHeight * 0.5f, 0.0f, 0.0f) );
		}
		else
		{
			unsigned char i = *reinterpret_cast<const unsigned char*>(str);
			device->Draw(QUADS, 4*i, 4);
			modelViewMatrixUniform->Set( modelViewMatrix *= Matrix4f::translation(float(charWidth), 0.0f, 0.0f) );
		}
	}
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
	textureUniform   = program->GetSamplerUniform2D("texture");
	colorUniform     = program->GetUniform4F("color");
	positionUniform  = program->GetUniform2F("position");
	scaleUniform     = program->GetUniform2F("scale");

	// create vertex layout
	{
		int location = program->AttributeLocation("position_texcoord");
		if (location == -1) {
			throw gl_error("GLFont::GLFont failed. Can't get location of font attribute.");
		}

		VertexLayout::ELEMENT elements[] =
		{
			{location, 4, 0, 16, sgl::FLOAT, VertexLayout::ATTRIBUTE},
		};
		vertexLayout.reset( device->CreateVertexLayout(1, elements) );
	}

	// create vbo
	{
		math::vector_of_vector4f vertices;
		const float ds = 1.0f / 16.0f;
		const float dt = 1.0f / 16.0f;
		for(int y = 0, i = 0; y < 16; y++)
		{
			for(int x = 0; x < 16; x++, i++)
			{
				float s = (float)x / 16.0f;
				float t = (float)y / 16.0f;

				vertices.push_back( Vector4f( 0.0f,  0.0f, s, t) );
				vertices.push_back( Vector4f( 1.0f,  0.0f, s + ds, t) );
				vertices.push_back( Vector4f( 1.0f, -1.0f, s + ds, t + dt) );
				vertices.push_back( Vector4f( 0.0f, -1.0f, s, t + dt) );
			}
		}

		vbo.reset( device->CreateVertexBuffer() );
		if (!vbo) {
			throw gl_error("GLFont::GLFont failed. Can't create vbo for font.");
		}
		vbo->SetData( vertices.size() * sizeof(Vector4f), &vertices[0] );
	}
}

SGL_HRESULT GLFontProgrammable::Bind(int width, int height, const math::Vector4f& color) const
{
	rectangle vp = device->Viewport();
	vpWidth      = float(vp.width);
	vpHeight     = float(vp.height);
	charWidth    = width * 2.0f / vp.width;
	charHeight   = height * 2.0f / vp.height;

	program->Bind();
	colorUniform->Set(color);
	scaleUniform->Set( math::Vector2f(charWidth, charHeight) );
	textureUniform->Set( 0, texture.get() );

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

void GLFontProgrammable::Print(float x, float y, const char* str) const
{
	float offsetx = 2.0f * x / vpWidth - 1.0f;
	float offsety = 2.0f * (vpHeight - y) / vpHeight - 1.0f;

	positionUniform->Set( math::Vector2f(offsetx, offsety) );
	for(int lines = 0; *str; str++)
	{
		if(*str == '\n')
		{
			lines++;
			positionUniform->Set( math::Vector2f(offsetx, offsety - charHeight * lines) );
		}
		else if(*str == '\r')
		{
			positionUniform->Set( math::Vector2f(offsetx, offsety - charHeight * lines) );
		}
		else if(*str == '\t')
		{
			positionUniform->Set( positionUniform->Value() + math::Vector2f(charWidth * 2.0f, 0.0f) );
		}
		else if(*str == ' ')
		{
			positionUniform->Set( positionUniform->Value() + math::Vector2f(charWidth * 0.5f, 0.0f) );
		}
		else
		{
			unsigned char i = *reinterpret_cast<const unsigned char*>(str);
			device->Draw(QUADS, 4*i, 4);
			positionUniform->Set( positionUniform->Value() + math::Vector2f(charWidth * 0.666667f, 0.0f) );
		}
	}
}

} // namespace sgl
