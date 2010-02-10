#include "GL/GLFont.h"
#include "Math/Containers.hpp"
#include "Math/MatrixFunctions.hpp"
#include <assert.h>
#include "GL/GLCommon.h"

using namespace sgl;
using namespace std;
using namespace math;

GLFont::GLFont(Device* _pDevice) :
    pDevice(_pDevice)
{
    // create states
    ffpProgram.reset( pDevice->FixedPipelineProgram() );
    projectionMatrixUniform = ffpProgram->GetProjectionMatrixUniform();
    modelViewMatrixUniform  = ffpProgram->GetModelViewMatrixUniform();
    lightingEnableUniform   = ffpProgram->GetLightingToggleUniform();
    ambientUniform          = ffpProgram->GetAmbientUniform();
    materialAmbientUniform  = ffpProgram->GetMaterialAmbientUniform();

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
        desc.depthEnable = false;
        depthStencilState.reset( pDevice->CreateDepthStencilState(desc) );
    }
}

GLFont::~GLFont()
{
}

SGL_HRESULT GLFont::LoadFromFile(const char* fileName)
{
    // load texture font
	ref_ptr<Image> image( pDevice->CreateImage() );
	SGL_HRESULT result = image->LoadFromFile(fileName);
	if (SGL_OK != result) {
		return result;
	}

	texture.reset( image->CreateTexture2D() );
	if (!texture) {
        return sglGetLastError();
	}

    // generate mipmaps
    result = texture->GenerateMipmap();
	if (SGL_OK != result) {
		return result;
	}

    // create sampler state
    {
        SamplerState::DESC desc;

        desc.filter[0]   = SamplerState::LINEAR;
        desc.filter[1]   = SamplerState::LINEAR;
        desc.filter[2]   = SamplerState::LINEAR;

        desc.wrapping[0] = SamplerState::REPEAT;
        desc.wrapping[1] = SamplerState::REPEAT;

        samplerState.reset( pDevice->CreateSamplerState(desc) );
        texture->BindSamplerState( samplerState.get() );
    }

    // generate font
    {
        charHeight = texture->Height() / 16.0f;
        charWidth = texture->Width() / 24.0f;

        // data
        math::vector_of_vector4f vertices;
	    vbo.reset( pDevice->CreateVertexBuffer() );
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
	    vbo->SetData( vertices.size() * sizeof(Vector4f), &vertices[0] );

        VertexLayout::ELEMENT elements[] = 
        { 
            {0, 2, 0, 16, FLOAT, VertexLayout::VERTEX},
            {0, 2, 8, 16, FLOAT, VertexLayout::TEXCOORD}
        };
        vertexLayout.reset( pDevice->CreateVertexLayout(2, elements) );
    }

	return SGL_OK;
}

SGL_HRESULT GLFont::Bind(int width, int height) const
{
	this->width = width;
	this->height = height;

    // setup states
    projectionMatrix = projectionMatrixUniform->Value();
    modelViewMatrix  = modelViewMatrixUniform->Value(); 

    rectangle vp = pDevice->Viewport();
    projectionMatrixUniform->Set( make_ortho( 0.0f, vp.width / (width / charHeight),
                                                vp.height / (height / charHeight), 0.0f,
                                                -1.0f, 1.0f ) );

    pDevice->PushState(State::DEPTH_STENCIL_STATE);
    pDevice->PushState(State::BLEND_STATE);

    depthStencilState->Bind();
    ffpProgram->Bind();

    blendState->Bind();
    vbo->Bind( vertexLayout.get() );

	return texture->Bind(0);
}

void GLFont::Unbind() const
{
    texture->Unbind();
    pDevice->PopState(State::DEPTH_STENCIL_STATE);
    pDevice->PopState(State::BLEND_STATE);

    projectionMatrixUniform->Set(projectionMatrix);
    modelViewMatrixUniform->Set(modelViewMatrix);
}

void GLFont::Print(float x, float y, const char* str) const
{
    float scaleX = charHeight / width;
    float scaleY = charHeight / height;

    modelViewMatrixUniform->Set( make_translation(x * scaleX, y * scaleY, 0.0f) );
	for(int lines = 0; *str; str++)
	{
		if(*str == '\n')
		{
			lines++;
			modelViewMatrixUniform->Set( make_translation(x * scaleX, y * scaleY + charHeight * lines, 0.0f) );
		}
        else if(*str == '\r')
		{
			modelViewMatrixUniform->Set( make_translation(x * scaleX, y * scaleY + charHeight * lines, 0.0f) );
		}
        else if(*str == '\t')
        {
            modelViewMatrixUniform->Set( modelViewMatrixUniform->Value() * make_translation(charHeight * 2.0f, 0.0f, 0.0f) );
		}
        else if(*str == ' ')
        {
            modelViewMatrixUniform->Set( modelViewMatrixUniform->Value() * make_translation(charHeight * 0.5f, 0.0f, 0.0f) );
		}
        else
        {
			unsigned char i = *reinterpret_cast<const unsigned char*>(str);
			pDevice->Draw(QUADS, 4*i, 4);
			modelViewMatrixUniform->Set( modelViewMatrixUniform->Value() * make_translation(static_cast<float>(charWidth), 0.0f, 0.0f) );
		}
	}

    vbo->Unbind();
}
