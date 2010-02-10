#ifndef SIMPLE_GL_FONT_H
#define SIMPLE_GL_FONT_H

#include "Texture.h"

namespace sgl {

/** Font class */
class Font : 
	public Resource
{
public:
    /** Load font texture from file
     * Could et SGLERR_INVALID_CALL if file doesn't exist
     * @param file name of the texture file
     */
    virtual SGL_HRESULT SGL_DLLCALL LoadFromFile(const char* fileName) = 0;

    /** Begin using font. Could set SGlERR_INVALID_CALL if font is dirty
     * @param width - character width in pixels
     * @param height - character height in pixels
	 * @return result of the operation.
     */
    virtual SGL_HRESULT SGL_DLLCALL Bind(int width, int height) const = 0;

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
