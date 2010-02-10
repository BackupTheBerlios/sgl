#ifndef SIMPLE_GL_GL_TEXTURE_3D_H
#define SIMPLE_GL_GL_TEXTURE_3D_H

#include "GLSamplerState.h"
#include "GLTexture.h"

namespace sgl {

class GLTexture3D :
    public GLTexture<Texture3D>
{
public:
    GLTexture3D(GLDevice* device, const DESC& desc);
    GLTexture3D(GLDevice* device, const DESC_MS& desc);

    // Override Texture2
    TYPE            SGL_DLLCALL Type() const        { return TEXTURE_3D; }
    Texture::FORMAT SGL_DLLCALL Format() const      { return format; }
    unsigned int    SGL_DLLCALL Samples() const     { return numSamples; }
    unsigned int    SGL_DLLCALL Width() const       { return width; }
    unsigned int    SGL_DLLCALL Height() const      { return height; }
    unsigned int    SGL_DLLCALL Depth() const       { return depth; }
    unsigned int    SGL_DLLCALL NumMipmaps() const  { return numMipmaps; }

    SGL_HRESULT     SGL_DLLCALL GenerateMipmap();
    SGL_HRESULT     SGL_DLLCALL SetSubImage( unsigned int    mipmap,
                                             unsigned int    offsetx,
                                             unsigned int    offsety,
                                             unsigned int    offsetz,
                                             unsigned int    width, 
                                             unsigned int    height,
                                             unsigned int    depth,
                                             const void*     data );
    SGL_HRESULT     SGL_DLLCALL GetImage( unsigned int  mipmap,
                                          void*         data );

    SGL_HRESULT SGL_DLLCALL Bind(unsigned int stage) const;
    void        SGL_DLLCALL Unbind() const;

private:
    Texture::FORMAT format;
    unsigned int    width;
    unsigned int    height;
    unsigned int    depth;
    unsigned int    numSamples;
    unsigned int    numMipmaps;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_TEXTURE_3D_H
