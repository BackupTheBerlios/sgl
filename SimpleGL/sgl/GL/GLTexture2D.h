#ifndef SIMPLE_GL_GL_TEXTURE_2D_H
#define SIMPLE_GL_GL_TEXTURE_2D_H

#include "GLSamplerState.h"
#include "GLTexture.h"

namespace sgl {

class GLTexture2D :
    public GLTexture<Texture2D>
{
public:
    typedef GLTexture<Texture2D>		   base_type;
    typedef base_type::guarded_binding     guarded_binding;
    typedef base_type::guarded_binding_ptr guarded_binding_ptr;

public:
    GLTexture2D(GLDevice* device, const Texture2D::DESC& desc);
    GLTexture2D(GLDevice* device, const Texture2D::DESC_MS& desc);
    ~GLTexture2D();

    // Override Texture2
    Texture::TYPE   SGL_DLLCALL Type() const        { return Texture::TEXTURE_2D; }
    Texture::FORMAT SGL_DLLCALL Format() const      { return format; }
    unsigned int    SGL_DLLCALL Samples() const     { return numSamples; }
    unsigned int    SGL_DLLCALL Width() const       { return width; }
    unsigned int    SGL_DLLCALL Height() const      { return height; }
    unsigned int    SGL_DLLCALL NumMipmaps() const  { return numMipmaps; }

    SGL_HRESULT     SGL_DLLCALL GenerateMipmap();
    SGL_HRESULT     SGL_DLLCALL SetSubImage( unsigned int    mipmap,
                                             unsigned int    offsetx,
                                             unsigned int    offsety,
                                             unsigned int    width, 
                                             unsigned int    height, 
                                             const void*     data );
    SGL_HRESULT     SGL_DLLCALL GetImage( unsigned int  mipmap,
                                          void*         data );

    SGL_HRESULT     SGL_DLLCALL BindSamplerState(SamplerState* samplerState);
    SGL_HRESULT     SGL_DLLCALL Bind(unsigned int stage) const;
    void            SGL_DLLCALL Unbind() const;

private:
    Texture::FORMAT format;
    unsigned int    width;
    unsigned int    height;
    unsigned int    numSamples;
    unsigned int    numMipmaps;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_TEXTURE_2D_H
