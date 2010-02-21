#ifndef SIMPLE_GL_IL_IMAGE_H
#define SIMPLE_GL_IL_IMAGE_H

#include "Device.h"
#include <vector>

namespace sgl {

class IlImage :
    public ReferencedImpl<Image>
{
public:
    IlImage(Device* _device) :
        device(_device),
        width(0),
        height(0),
        depth(0),
        format(Texture::UNKNOWN)
    {}
    ~IlImage();

    // Creation
    SGL_HRESULT SGL_DLLCALL LoadFromFile(const char* fileName);
    SGL_HRESULT SGL_DLLCALL LoadFromFileInMemory( FILE_TYPE    type,
                                                  unsigned int dataSize,
                                                  const void*  data );

    SGL_HRESULT SGL_DLLCALL SaveToFile(const char* fileName) const;
    SGL_HRESULT SGL_DLLCALL SaveToFileInMemory(FILE_TYPE    type,
                                               unsigned int dataSize,
                                               void*        data) const;
/*
    SGL_HRESULT SGL_DLLCALL SetImage(Texture::FORMAT   _format,
                                     unsigned          _width,
                                     unsigned          _height,
                                     unsigned          _depth,
                                     const void*       rawData = 0 );

    SGL_HRESULT SGL_DLLCALL SetSubImage(unsigned int mipmap,
                                        unsigned int offsetx,
                                        unsigned int offsety,
                                        unsigned int width,
                                        unsigned int height,
                                        const void*  rawData );
*/
    void            SGL_DLLCALL Clear();

    Texture::FORMAT SGL_DLLCALL Format() const                          { return format; }
    unsigned int    SGL_DLLCALL NumMipmaps() const                      { return mipData.size(); }
    unsigned int    SGL_DLLCALL Height() const                          { return height; }
    unsigned int    SGL_DLLCALL Width() const                           { return width; }
    unsigned int    SGL_DLLCALL Depth() const                           { return depth; }
    char*           SGL_DLLCALL Data(unsigned int mipmap = 0);
    const char*     SGL_DLLCALL Data(unsigned int mipmap = 0) const;

    //Texture1D*      SGL_DLLCALL CreateTexture1D() const;
    Texture2D*      SGL_DLLCALL CreateTexture2D() const;
    Texture3D*      SGL_DLLCALL CreateTexture3D() const;
    //TextureCube*    SGL_DLLCALL CreateTextureCube() const;

protected:
    ref_ptr<Device>     device;
    unsigned int        width;
    unsigned int        height;
    unsigned int        depth;
    Texture::FORMAT     format;
    std::vector<char*>  mipData;
};

} // namespace sgl

#endif // SIMPLE_GL_IL_IMAGE_H
