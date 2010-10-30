#ifndef SIMPLE_GL_GL_FONT_H
#define SIMPLE_GL_GL_FONT_H

#include "GLForward.h"
#include "../Font.h"

namespace sgl {

/* Font class */
class GLFont :
    public ResourceImpl<Font>
{
public:
    GLFont(GLDevice* _pDevice);

    // Override Font
    SGL_HRESULT     SGL_DLLCALL SetTexture(sgl::Texture2D* texture);
    sgl::Texture2D* SGL_DLLCALL Texture() const;

protected:
    // drawing
    GLDevice*					device;
    ref_ptr<Texture2D>          texture;
    ref_ptr<VertexBuffer>       vbo;
    ref_ptr<VertexLayout>       vertexLayout;
    ref_ptr<RasterizerState>    rasterizerState;
    ref_ptr<BlendState>         blendState;
    ref_ptr<DepthStencilState>  depthStencilState;
};

/* Font class */
class GLFontFixed :
	public GLFont
{
public:
	GLFontFixed(GLDevice* device);

	// Override Font
	SGL_HRESULT SGL_DLLCALL SetTexture(sgl::Texture2D* texture);
	SGL_HRESULT SGL_DLLCALL Bind(int width, int height, const math::Vector4f& color) const;
	void	    SGL_DLLCALL Unbind() const;

	void SGL_DLLCALL Print(float x, float y, const char* str) const;

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
class GLFontProgrammable :
	public GLFont
{
public:
	GLFontProgrammable(GLDevice* device);
	
	// Override Font
	SGL_HRESULT SGL_DLLCALL Bind(int width, int height, const math::Vector4f& color) const;
	void	    SGL_DLLCALL Unbind() const;

	void SGL_DLLCALL Print(float x, float y, const char* str) const;

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

} // namespace sgl

#endif // SIMPLE_GL_GL_FONT_H
