#include "GL/GLFont.h"
#include "Math/Containers.hpp"
#include "Math/MatrixFunctions.hpp"
#include <assert.h>
#include "GL/GLCommon.h"

using namespace std;
using namespace math;

namespace {

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

template<>
GLFont<PT_FIXED>::GLFont(Device* _pDevice) :
    pDevice(_pDevice)
{
    // create states
    myAttr.ffpProgram.reset( pDevice->FixedPipelineProgram() );
    myAttr.projectionMatrixUniform = myAttr.ffpProgram->GetProjectionMatrixUniform();
    myAttr.modelViewMatrixUniform  = myAttr.ffpProgram->GetModelViewMatrixUniform();
    myAttr.lightingEnableUniform   = myAttr.ffpProgram->GetLightingToggleUniform();
    myAttr.ambientUniform          = myAttr.ffpProgram->GetAmbientUniform();
    myAttr.materialAmbientUniform  = myAttr.ffpProgram->GetMaterialAmbientUniform();

    {
        BlendState::DESC_SIMPLE desc;
        desc.blendEnable = true;
        desc.srcBlend    = BlendState::ONE;
        desc.destBlend   = BlendState::ONE_MINUS_SRC_ALPHA;
        desc.blendOp     = BlendState::ADD;
        blendState.reset( pDevice->CreateBlendState(Extend(desc)) );
    }

    {
        DepthStencilState::DESC desc;
        desc.depthEnable    = false;
        desc.depthWriteMask = 0;
        desc.stencilEnable  = false;
        depthStencilState.reset( pDevice->CreateDepthStencilState(desc) );
    }

    {
        VertexLayout::ELEMENT elements[] =
        {
            {0, 2, 0, 16, FLOAT, VertexLayout::VERTEX},
            {0, 2, 8, 16, FLOAT, VertexLayout::TEXCOORD}
        };
        vertexLayout.reset( pDevice->CreateVertexLayout(2, elements) );
    }
}

template<>
GLFont<PT_PROGRAMMABLE>::GLFont(Device* _pDevice) :
    pDevice(_pDevice)
{
    // create states
    myAttr.program.reset( pDevice->CreateProgram() );
    {
        Shader::DESC desc;
        desc.source = font_vertex_shader;
        desc.type   = Shader::VERTEX;
        sgl::Shader* vertexShader = pDevice->CreateShader(desc);
        if (!vertexShader) {
            throw gl_error("GLFont::GLFont failed. Can't create vertex shader for font.");
        }

        desc.source = font_fragment_shader;
        desc.type   = Shader::FRAGMENT;
        sgl::Shader* fragmentShader = pDevice->CreateShader(desc);
        if (!fragmentShader) {
            throw gl_error("GLFont::GLFont failed. Can't create fragment shader for font.");
        }

        myAttr.program->AddShader(vertexShader);
        myAttr.program->AddShader(fragmentShader);
        if ( SGL_OK != myAttr.program->Dirty() ) {
            throw gl_error("GLFont::GLFont failed. Can't create shader program for font.");
        }
    }
    myAttr.textureUniform   = myAttr.program->GetSamplerUniform2D("texture");
    myAttr.colorUniform     = myAttr.program->GetUniform4F("color");
    myAttr.positionUniform  = myAttr.program->GetUniform2F("position");
    myAttr.scaleUniform     = myAttr.program->GetUniform2F("scale");

    // create blend state
    {
        BlendState::DESC_SIMPLE desc;
        desc.blendEnable = true;
        desc.srcBlend    = BlendState::ONE;
        desc.destBlend   = BlendState::ONE_MINUS_SRC_ALPHA;
        desc.blendOp     = BlendState::ADD;
        blendState.reset( pDevice->CreateBlendState(Extend(desc)) );
    }

    // create depth stencil state
    {
        DepthStencilState::DESC desc;
        desc.depthEnable    = false;
        desc.depthWriteMask = 0;
        desc.stencilEnable  = false;
        depthStencilState.reset( pDevice->CreateDepthStencilState(desc) );
    }

    // create vertex layout
    {
        int location = myAttr.program->AttributeLocation("position_texcoord");
        if (location == -1) {
            throw gl_error("GLFont::GLFont failed. Can't get location of font attribute.");
        }

        VertexLayout::ELEMENT elements[] =
        {
            {location, 4, 0, 16, FLOAT, VertexLayout::ATTRIBUTE},
        };
        vertexLayout.reset( pDevice->CreateVertexLayout(1, elements) );
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

	    vbo.reset( pDevice->CreateVertexBuffer() );
        if (!vbo) {
            throw gl_error("GLFont::GLFont failed. Can't create vbo for font.");
        }
	    vbo->SetData( vertices.size() * sizeof(Vector4f), &vertices[0] );
    }
}

template<PIPELINE_TYPE pipeline>
GLFont<pipeline>::~GLFont()
{
}

template<PIPELINE_TYPE pipeline>
SGL_HRESULT GLFont<pipeline>::SetTexture(sgl::Texture2D* texture_)
{
    texture.reset(texture_);
    if (texture)
    {
        SGL_HRESULT result = texture->GenerateMipmap();
	    if (SGL_OK != result) {
		    return result;
	    }

        SamplerState::DESC desc;

        desc.filter[0]   = SamplerState::LINEAR;
        desc.filter[1]   = SamplerState::LINEAR;
        desc.filter[2]   = SamplerState::LINEAR;

        desc.wrapping[0] = SamplerState::REPEAT;
        desc.wrapping[1] = SamplerState::REPEAT;

        texture->BindSamplerState( pDevice->CreateSamplerState(desc) );
    }

    // generate verts
    if (pipeline == PT_FIXED)
    {
        float charHeight = texture->Height() / 16.0f;
        float charWidth  = texture->Width()  / 24.0f;

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

	    vbo.reset( pDevice->CreateVertexBuffer() );
        if (!vbo) {
            return sglGetLastError();
        }
	    vbo->SetData( vertices.size() * sizeof(Vector4f), &vertices[0] );
    }

    return SGL_OK;
}

template<PIPELINE_TYPE pipeline>
sgl::Texture2D* GLFont<pipeline>::Texture() const
{
    return texture.get();
}

template<>
SGL_HRESULT GLFont<PT_FIXED>::Bind(int width,
                                   int height,
                                   const math::Vector4f& color) const
{
	myAttr.width  = width;
	myAttr.height = height;

    float charHeight = texture->Height() / 16.0f;
    float charWidth  = texture->Width()  / 24.0f;

    // setup states
    myAttr.projectionMatrix = myAttr.projectionMatrixUniform->Value();
    myAttr.modelViewMatrix  = myAttr.modelViewMatrixUniform->Value();

    rectangle vp = pDevice->Viewport();
    myAttr.projectionMatrixUniform->Set( make_ortho( 0.0f, vp.width / (width / charHeight),
                                                     vp.height / (height / charHeight), 0.0f,
                                                     -1.0f, 1.0f ) );

    pDevice->PushState(State::DEPTH_STENCIL_STATE);
    pDevice->PushState(State::BLEND_STATE);

    myAttr.ffpProgram->Bind();
    depthStencilState->Bind();
    blendState->Bind();
    vbo->Bind( vertexLayout.get() );

	return texture->Bind(0);
}

template<>
SGL_HRESULT GLFont<PT_PROGRAMMABLE>::Bind(int width,
                                          int height,
                                          const math::Vector4f& color) const
{
    rectangle vp = pDevice->Viewport();
    myAttr.vpWidth    = float(vp.width);
    myAttr.vpHeight   = float(vp.height);
    myAttr.charWidth  = width * 2.0f / vp.width;
    myAttr.charHeight = height * 2.0f / vp.height;

    myAttr.program->Bind();
    myAttr.colorUniform->Set(color);
    myAttr.scaleUniform->Set( math::Vector2f(myAttr.charWidth, myAttr.charHeight) );
    myAttr.textureUniform->Set( 0, texture.get() );

    pDevice->PushState(State::DEPTH_STENCIL_STATE);
    pDevice->PushState(State::BLEND_STATE);

    depthStencilState->Bind();
    blendState->Bind();
    vbo->Bind( vertexLayout.get() );

	return SGL_OK;
}

template<>
void GLFont<PT_FIXED>::Unbind() const
{
    vbo->Unbind();
    texture->Unbind();
    pDevice->PopState(State::DEPTH_STENCIL_STATE);
    pDevice->PopState(State::BLEND_STATE);

    myAttr.projectionMatrixUniform->Set(myAttr.projectionMatrix);
    myAttr.modelViewMatrixUniform->Set(myAttr.modelViewMatrix);
}

template<>
void GLFont<PT_PROGRAMMABLE>::Unbind() const
{
    vbo->Unbind();
    texture->Unbind();
    pDevice->PopState(State::DEPTH_STENCIL_STATE);
    pDevice->PopState(State::BLEND_STATE);
    myAttr.program->Unbind();
}

template<>
void GLFont<PT_FIXED>::Print(float x, float y, const char* str) const
{
    float charHeight = texture->Height() / 16.0f;
    float charWidth  = texture->Width()  / 24.0f;

    float scaleX = charHeight / myAttr.width;
    float scaleY = charHeight / myAttr.height;

    myAttr.modelViewMatrixUniform->Set( make_translation(x * scaleX, y * scaleY, 0.0f) );
	for(int lines = 0; *str; str++)
	{
		if(*str == '\n')
		{
			lines++;
			myAttr.modelViewMatrixUniform->Set( make_translation(x * scaleX, y * scaleY + charHeight * lines, 0.0f) );
		}
        else if(*str == '\r')
		{
			myAttr.modelViewMatrixUniform->Set( make_translation(x * scaleX, y * scaleY + charHeight * lines, 0.0f) );
		}
        else if(*str == '\t')
        {
            myAttr.modelViewMatrixUniform->Set( myAttr.modelViewMatrixUniform->Value() * make_translation(charHeight * 2.0f, 0.0f, 0.0f) );
		}
        else if(*str == ' ')
        {
            myAttr.modelViewMatrixUniform->Set( myAttr.modelViewMatrixUniform->Value() * make_translation(charHeight * 0.5f, 0.0f, 0.0f) );
		}
        else
        {
			unsigned char i = *reinterpret_cast<const unsigned char*>(str);
			pDevice->Draw(QUADS, 4*i, 4);
			myAttr.modelViewMatrixUniform->Set( myAttr.modelViewMatrixUniform->Value() * make_translation(static_cast<float>(charWidth), 0.0f, 0.0f) );
		}
	}
}

template<>
void GLFont<PT_PROGRAMMABLE>::Print(float x, float y, const char* str) const
{
    float offsetx = 2.0f * x / myAttr.vpWidth - 1.0f;
    float offsety = 2.0f * (myAttr.vpHeight - y) / myAttr.vpHeight - 1.0f;

    myAttr.positionUniform->Set( math::Vector2f(offsetx, offsety) );
	for(int lines = 0; *str; str++)
	{
		if(*str == '\n')
		{
			lines++;
            myAttr.positionUniform->Set( math::Vector2f(offsetx, offsety - myAttr.charHeight * lines) );
		}
        else if(*str == '\r')
		{
            myAttr.positionUniform->Set( math::Vector2f(offsetx, offsety - myAttr.charHeight * lines) );
		}
        else if(*str == '\t')
        {
            myAttr.positionUniform->Set( myAttr.positionUniform->Value() + math::Vector2f(myAttr.charWidth * 2.0f, 0.0f) );
		}
        else if(*str == ' ')
        {
            myAttr.positionUniform->Set( myAttr.positionUniform->Value() + math::Vector2f(myAttr.charWidth * 0.5f, 0.0f) );
		}
        else
        {
			unsigned char i = *reinterpret_cast<const unsigned char*>(str);
			pDevice->Draw(QUADS, 4*i, 4);
            myAttr.positionUniform->Set( myAttr.positionUniform->Value() + math::Vector2f(myAttr.charWidth * 0.666667f, 0.0f) );
		}
	}
}

// explicit template instantiation
template class GLFont<PT_FIXED>;
template class GLFont<PT_PROGRAMMABLE>;

} // namespace sgl
