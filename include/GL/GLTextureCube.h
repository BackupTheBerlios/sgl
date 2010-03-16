#ifndef SIMPLE_GL_GL_TEXTURE_CUBE_H
#define SIMPLE_GL_GL_TEXTURE_CUBE_H

#include "GLTexture2D.h"

namespace sgl {

// forward
class GLTextureCube;

class GLTextureCubeSide :
    public GLTexture<Texture2D>
{
public:
    GLTextureCubeSide( GLTextureCube*           texture,
                       const Texture2D::DESC&   desc,
                       TextureCube::SIDE        side );

    // Override Texture2D
    TYPE            SGL_DLLCALL Type() const    { return TEXTURE_2D; }
    FORMAT          SGL_DLLCALL Format() const  { return format; }
    unsigned int    SGL_DLLCALL Samples() const { return 0; }
    unsigned int    SGL_DLLCALL Width() const   { return width; }
    unsigned int    SGL_DLLCALL Height() const  { return height; }

    unsigned int    SGL_DLLCALL NumMipmaps() const  { return 0; }
    SGL_HRESULT     SGL_DLLCALL GenerateMipmap()    { return EInvalidCall("Can't generate mipmap per cubemap side."); }

    SGL_HRESULT     SGL_DLLCALL SetSubImage( unsigned int    mipmap,
                                             unsigned int    offsetx,
                                             unsigned int    offsety,
                                             unsigned int    width,
                                             unsigned int    height,
                                             const void*     data );

    SGL_HRESULT     SGL_DLLCALL SetImage( unsigned int  mipmap,
                                          const void*   data );

    SGL_HRESULT     SGL_DLLCALL GetImage( unsigned int  mipmap,
                                          void*         data );

    SGL_HRESULT     SGL_DLLCALL BindSamplerState(SamplerState*)    { return EInvalidCall("Can't setup sampler state to cubemap side."); }
    SGL_HRESULT     SGL_DLLCALL Bind(unsigned int) const           { return EInvalidCall("Can't bind cube map side as 2D sampler."); }
    void            SGL_DLLCALL Unbind() const                     {}

private:
    GLTextureCube*      texture;
    TextureCube::SIDE   side;
    Texture::FORMAT     format;
    unsigned int        width;
    unsigned int        height;
};

class GLTextureCube :
    public GLTexture<TextureCube>
{
friend class GLTextureCubeSide;
public:
    GLTextureCube( Device*      device,
                   const DESC&  desc );

    // Override TextureCube
    Texture::TYPE   SGL_DLLCALL Type() const    { return Texture::TEXTURE_CUBE_MAP; }
    Texture2D*      SGL_DLLCALL Side(SIDE side) { return sides[side]; }

    SGL_HRESULT     SGL_DLLCALL GenerateMipmap();

    SGL_HRESULT     SGL_DLLCALL BindSamplerState(SamplerState* samplerState);
    SGL_HRESULT     SGL_DLLCALL Bind(unsigned int stage) const;
    void            SGL_DLLCALL Unbind() const;

private:
    ref_ptr<GLTextureCubeSide>  sides[6];
};

} // namesapce sgl

#endif // SIMPLE_GL_GL_TEXTURE_CUBE_H
