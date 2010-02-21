#ifndef SIMPLE_GL_FONT_H
#define SIMPLE_GL_FONT_H

#include "Texture.h"

namespace sgl {

/** Font class */
class Font : 
	public Resource
{
public:
    /** Set texture of font character table. Texture must contain
     * 16x16 character table and must have alpha channel.
     * @return result of the operation.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetTexture(sgl::Texture2D* texture) = 0;

    /** Get texture used for font character table. */
    virtual sgl::Texture2D* SGL_DLLCALL Texture() const = 0;

    /** Begin using font. Could set SGlERR_INVALID_CALL if font is dirty
     * @param width - character width in pixels.
     * @param height - character height in pixels.
     * @param color - character color.
	 * @return result of the operation.
     */
    virtual SGL_HRESULT SGL_DLLCALL Bind( int width, 
                                          int height, 
                                          const math::Vector4f& color = math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f) ) const = 0;

    /** End using font */
    virtual void SGL_DLLCALL Unbind() const = 0;

    /** Print string
     * @param x - x coord of the string rectangle
     * @param y - y coord of the string rectangle
     * @param str - cstring to print
     */
    virtual void SGL_DLLCALL Print(float x, float y, const char* str) const = 0;

    virtual SGL_DLLCALL ~Font() {}
};

} // namespace sgl

#endif // SIMPLE_GL_FONT_H
