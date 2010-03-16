#ifndef SIMPLE_GL_GL_TEXTURE_1D_H
#define SIMPLE_GL_GL_TEXTURE_1D_H

#include "Texture1D.h"
#include "GLTexture.h"

#ifdef SIMPLE_GL_USE_SDL_IMAGE
#   include <SDL_image.h>
#endif

namespace sgl {

/* GL texture wrapper */
class GLTexture1D :
    public GLTexture<Texture1D>
{
public:
    GLTexture1D(Device* _pDevice);
    ~GLTexture1D();
/*
    // type of the texture
    TYPE SGL_DLLCALL Type() const { return TEXTURE_1D; }

    // create the texture with specified parameters
    void SGL_DLLCALL Clear();

    // files
    SGL_HRESULT SGL_DLLCALL LoadFromFile(const char* fileName);
    SGL_HRESULT SGL_DLLCALL SaveToFile(const char* fileName) const;

    // creation
    SGL_HRESULT SGL_DLLCALL SetImage(FORMAT _format, unsigned int _size, const void* _data);
    void        SGL_DLLCALL SetResizeNonPowerOfTwoHint(bool toggle);

    // properties
    unsigned int    SGL_DLLCALL Size() const                    { return size; }
    FORMAT          SGL_DLLCALL Format() const                  { return format; }
    bool            SGL_DLLCALL ResizeNonPowerOfTwoHint() const { return resizeNonPowerOfTwo; }

    GLenum SGL_DLLCALL GLFormat() const;
*/
protected:
    // settings
    unsigned int	size;
    FORMAT          format;
    bool            resizeNonPowerOfTwo;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_TEXTURE_2D_H
