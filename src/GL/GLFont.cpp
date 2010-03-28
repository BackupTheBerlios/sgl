#include "GL/GLCommon.h"
#include "GL/GLDevice.h"
#include "GL/GLFont.h"
#include "Math/Containers.hpp"
#include "Math/MatrixFunctions.hpp"

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

    /* Font class */
    template<DEVICE_VERSION DeviceVersion>
    class GLFontFixed :
        public GLFont<DeviceVersion>
    {
    public:
        typedef GLFont<DeviceVersion> base_type;

    public:
        GLFontFixed(GLDevice<DeviceVersion>* device) :
            base_type(device)
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
                base_type::vertexLayout.reset( device->CreateVertexLayout(2, elements) );
            }
        }

        SGL_HRESULT SGL_DLLCALL SetTexture(sgl::Texture2D* texture)
        {
            SGL_HRESULT result = base_type::SetTexture(texture);
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

            base_type::vbo.reset( base_type::device->CreateVertexBuffer() );
            if (!base_type::vbo) {
                return sglGetLastError();
            }
            base_type::vbo->SetData( vertices.size() * sizeof(Vector4f), &vertices[0] );

            return SGL_OK;
        }

        SGL_HRESULT SGL_DLLCALL Bind(int width, 
                                     int height,
                                     const math::Vector4f& color) const
        {
	        this->width  = width;
	        this->height = height;

            // setup states
            projectionMatrix = projectionMatrixUniform->Value();
            modelViewMatrix  = modelViewMatrixUniform->Value();

            rectangle vp = base_type::device->Viewport();
            projectionMatrixUniform->Set( make_ortho( 0.0f, vp.width / (width / charHeight),
                                                      vp.height / (height / charHeight), 0.0f,
                                                      -1.0f, 1.0f ) );

            base_type::device->PushState(State::DEPTH_STENCIL_STATE);
            base_type::device->PushState(State::BLEND_STATE);

            ffpProgram->Bind();
            base_type::depthStencilState->Bind();
            base_type::blendState->Bind();
            base_type::vbo->Bind( base_type::vertexLayout.get() );

            return base_type::texture->Bind(0);
        }

        void SGL_DLLCALL Unbind() const
        {
            base_type::vbo->Unbind();
            base_type::texture->Unbind();
            base_type::device->PopState(State::DEPTH_STENCIL_STATE);
            base_type::device->PopState(State::BLEND_STATE);

            projectionMatrixUniform->Set(projectionMatrix);
            modelViewMatrixUniform->Set(modelViewMatrix);
        }

        void SGL_DLLCALL Print(float x, float y, const char* str) const
        {
            float charHeight = base_type::texture->Height() / 16.0f;
            float charWidth  = base_type::texture->Width()  / 24.0f;

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
                    base_type::device->Draw(QUADS, 4*i, 4);
			        modelViewMatrixUniform->Set( modelViewMatrixUniform->Value() * make_translation(static_cast<float>(charWidth), 0.0f, 0.0f) );
		        }
	        }
        }

    private:
        ref_ptr<FFPProgram>         ffpProgram;
        Uniform4x4F*                projectionMatrixUniform;
        Uniform4x4F*                modelViewMatrixUniform;
        sgl::UniformI*              lightingEnableUniform;
        sgl::Uniform4F*             ambientUniform;
        sgl::Uniform4F*             materialAmbientUniform;    

        // remember states
        mutable math::Matrix4f      projectionMatrix;
        mutable math::Matrix4f      modelViewMatrix;

        // Char info
        float   charWidth;
        float   charHeight;

        // Print info
        mutable int width;
        mutable int height;
    };

    /* Font class */
    template<DEVICE_VERSION DeviceVersion>
    class GLFontProgrammable :
        public GLFont<DeviceVersion>
    {
    public:
        typedef GLFont<DeviceVersion> base_type;

    public:
        GLFontProgrammable(GLDevice<DeviceVersion>* device) :
            base_type(device)
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
                base_type::vertexLayout.reset( device->CreateVertexLayout(1, elements) );
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

                base_type::vbo.reset( device->CreateVertexBuffer() );
                if (!base_type::vbo) {
                    throw gl_error("GLFont::GLFont failed. Can't create vbo for font.");
                }
                base_type::vbo->SetData( vertices.size() * sizeof(Vector4f), &vertices[0] );
            }
        }

        SGL_HRESULT SGL_DLLCALL Bind(int width, 
                                     int height,
                                     const math::Vector4f& color) const
        {
            rectangle vp = base_type::device->Viewport();
            vpWidth      = float(vp.width);
            vpHeight     = float(vp.height);
            charWidth    = width * 2.0f / vp.width;
            charHeight   = height * 2.0f / vp.height;

            program->Bind();
            colorUniform->Set(color);
            scaleUniform->Set( math::Vector2f(charWidth, charHeight) );
            textureUniform->Set( 0, base_type::texture.get() );

            base_type::device->PushState(State::DEPTH_STENCIL_STATE);
            base_type::device->PushState(State::BLEND_STATE);

            base_type::depthStencilState->Bind();
            base_type::blendState->Bind();
            base_type::vbo->Bind( base_type::vertexLayout.get() );

	        return SGL_OK;
        }

        void SGL_DLLCALL Unbind() const
        {
            base_type::vbo->Unbind();
            base_type::texture->Unbind();
            base_type::device->PopState(State::DEPTH_STENCIL_STATE);
            base_type::device->PopState(State::BLEND_STATE);
            program->Unbind();
        }

        void SGL_DLLCALL Print(float x, float y, const char* str) const
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
                    base_type::device->Draw(QUADS, 4*i, 4);
                    positionUniform->Set( positionUniform->Value() + math::Vector2f(charWidth * 0.666667f, 0.0f) );
		        }
	        }
        }

    private:
        // States
        ref_ptr<Program>    program;
        SamplerUniform2D*   textureUniform;
        Uniform4F*          colorUniform;
        Uniform2F*          positionUniform;  
        Uniform2F*          scaleUniform;

        // Char info
        mutable float       charWidth;
        mutable float       charHeight;
        mutable float       vpWidth;
        mutable float       vpHeight;
    };

} // anonymous namespace

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
GLFont<DeviceVersion>::GLFont(GLDevice<DeviceVersion>* device_) :
    device(device_)
{
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

template<DEVICE_VERSION DeviceVersion>
SGL_HRESULT GLFont<DeviceVersion>::SetTexture(sgl::Texture2D* texture_)
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

        texture->BindSamplerState( device->CreateSamplerState(desc) );
    }

    return SGL_OK;
}

template<DEVICE_VERSION DeviceVersion>
sgl::Texture2D* GLFont<DeviceVersion>::Texture() const
{
    return texture.get();
}

template<DEVICE_VERSION DeviceVersion>
sgl::Font* sglCreateFont(GLDevice<DeviceVersion>* device)
{
    if ( device_traits<DeviceVersion>::support_programmable_pipeline() ) {
        return new GLFontProgrammable<DeviceVersion>(device);
    }

    return new GLFontFixed<DeviceVersion>(device);
}

// explicit template instantiation
template class GLFont<DV_OPENGL_1_3>;
template class GLFont<DV_OPENGL_1_4>;
template class GLFont<DV_OPENGL_1_5>;
template class GLFont<DV_OPENGL_2_1>;
template class GLFont<DV_OPENGL_3_0>;
template class GLFont<DV_OPENGL_3_1>;
template class GLFont<DV_OPENGL_3_2>;

template sgl::Font* sglCreateFont<DV_OPENGL_1_3>(GLDevice<DV_OPENGL_1_3>*);
template sgl::Font* sglCreateFont<DV_OPENGL_1_4>(GLDevice<DV_OPENGL_1_4>*);
template sgl::Font* sglCreateFont<DV_OPENGL_1_5>(GLDevice<DV_OPENGL_1_5>*);
template sgl::Font* sglCreateFont<DV_OPENGL_2_0>(GLDevice<DV_OPENGL_2_0>*);
template sgl::Font* sglCreateFont<DV_OPENGL_2_1>(GLDevice<DV_OPENGL_2_1>*);
template sgl::Font* sglCreateFont<DV_OPENGL_3_0>(GLDevice<DV_OPENGL_3_0>*);
template sgl::Font* sglCreateFont<DV_OPENGL_3_1>(GLDevice<DV_OPENGL_3_1>*);
template sgl::Font* sglCreateFont<DV_OPENGL_3_2>(GLDevice<DV_OPENGL_3_2>*);

} // namespace sgl
