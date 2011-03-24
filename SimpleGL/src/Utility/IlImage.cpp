#include "GL/GLTexture.h"
#include "Utility/IlImage.h"
#include <cstring>
#include <IL/il.h>

using namespace std;
using namespace sgl;

namespace {

    Texture::FORMAT FindTextureFormat(ILuint type, ILuint format, ILuint dxtc)
    {
        bool compressed = (dxtc == IL_DXT1) || (dxtc == IL_DXT2) || (dxtc == IL_DXT3) || (dxtc == IL_DXT4) || (dxtc == IL_DXT5);

        switch(format)
        {
        case GL_RGB:
            switch (type)
            {
            case GL_UNSIGNED_BYTE:
                if (dxtc == IL_DXT1) {
                    return Texture::COMPRESSED_RGB_S3TC_DXT1;
                }
                else if (compressed) {
                    return Texture::UNKNOWN;
                }
                return Texture::RGB8;

            case GL_HALF_FLOAT_ARB:
                if (compressed) {
                    return Texture::UNKNOWN;
                }
                return Texture::RGB16F;

            case GL_FLOAT:
                if (compressed) {
                    return Texture::UNKNOWN;
                }
                return Texture::RGB32F;

            default:
                assert(!"FindTextureFormat failed. Invalid format/type combination");
                return Texture::UNKNOWN;
            }

        case GL_RGBA:
            switch (type)
            {
            case GL_UNSIGNED_BYTE:
                if (dxtc == IL_DXT1) {
                    return Texture::COMPRESSED_RGBA_S3TC_DXT1;
                }
                else if (dxtc == IL_DXT3) {
                    return Texture::COMPRESSED_RGBA_S3TC_DXT3;
                }
                else if (dxtc == IL_DXT5) {
                    return Texture::COMPRESSED_RGBA_S3TC_DXT5;
                }
                else if (compressed) {
                    return Texture::UNKNOWN;
                }
                return Texture::RGBA8;

            case GL_HALF_FLOAT_ARB:
                if (compressed) {
                    return Texture::UNKNOWN;
                }
                return Texture::RGBA16F;

            case GL_FLOAT:
                if (compressed) {
                    return Texture::UNKNOWN;
                }
                return Texture::RGBA32F;

            default:
                assert(!"FindTextureFormat failed. Invalid format/type combination");
                return Texture::UNKNOWN;
            }

        default:
            assert(!"FindTextureFormat failed. Invalid format/type combination");
            return Texture::UNKNOWN;
        }

        return Texture2D::RGBA8;
    }

    ILuint BIND_IL_IMAGE_TYPE[] =
    {
        IL_TYPE_UNKNOWN,
     	IL_BMP,
        IL_DDS,
        IL_GIF,
        IL_ICO,
        IL_HDR,
        IL_JPG,
        IL_PIC,
        IL_PNG,
        IL_PSD,
        IL_TGA,
        IL_TIF   
    };

} // anonymous namespace


IlImage::~IlImage()
{
    Clear();
}

char* SGL_DLLCALL IlImage::Data(unsigned int mipmap)
{
    return mipmap >= mipData.size() ? 0 : mipData[mipmap];
}

const char* SGL_DLLCALL IlImage::Data(unsigned int mipmap) const
{
    return mipmap >= mipData.size() ? 0 : mipData[mipmap];
}

SGL_HRESULT SGL_DLLCALL IlImage::LoadFromFile(const char* fileName, 
                                              FILE_TYPE   type)
{
    Clear();

    // create image
    ILuint image;
    ilGenImages(1, &image);

    // load image
    ilBindImage(image);
    ILboolean imageLoaded = false;
    if (type == AUTO) {
        imageLoaded = ilLoad(BIND_IL_IMAGE_TYPE[type], fileName);
    }
    else {
        imageLoaded = ilLoadImage(fileName);
    }

    if (!imageLoaded)
    {
        // TODO: Add error check
        ilBindImage(0);
        ilDeleteImages(1, &image);
        return EFileNotFound( (std::string("IlImage::LoadFromFile failed. Can't find image file: ") + fileName).c_str() );
    }

    // get image properties
	int numMipmaps = ilGetInteger(IL_NUM_MIPMAPS);
	numMipmaps     = numMipmaps == 0 ? 1 : numMipmaps;
	width          = ilGetInteger(IL_IMAGE_WIDTH);
	height         = ilGetInteger(IL_IMAGE_HEIGHT);
    depth          = ilGetInteger(IL_IMAGE_DEPTH);

    ILuint ilType   = ilGetInteger(IL_IMAGE_TYPE);
    ILuint ilFormat = ilGetInteger(IL_IMAGE_FORMAT);
    ILuint ilDXTC   = ilGetInteger(IL_DXTC_DATA_FORMAT);
    format          = FindTextureFormat(ilType, ilFormat, ilDXTC);

    // copy image
    size_t width_  = width;
    size_t height_ = height;
    size_t depth_  = depth;

    mipData.resize(numMipmaps);
    for (int i = 0; i<numMipmaps; ++i)
    {
        size_t dataSize = Image::SizeOfData(format, width_, height_, depth_, false);

        mipData[i] = (char*)malloc(dataSize);
        if (!mipData[i])
        {
            ilBindImage(0);
            ilDeleteImages(1, &image);
            return EOutOfMemory("IlImage::LoadFromFile failed. Can't allocate memory for image");
        }
        ilActiveMipmap(i);
        ilCopyPixels(0, 0, 0, width_, height_, depth_, ilFormat, ilType, mipData[i]);

        width_  >>= 1;
        height_ >>= 1;
        depth_  >>= 1;
    }

    ilBindImage(0);
    ilDeleteImages(1, &image);

	return SGL_OK;
}

SGL_HRESULT SGL_DLLCALL IlImage::LoadFromFileInMemory(FILE_TYPE    type,
                                                      unsigned int dataSize,
                                                      const void*  data)
{
    Clear();

    // create image
    ILuint image;
    ilGenImages(1, &image);

    // load image
    ilBindImage(image);
    ILboolean imageLoaded = ilLoadL(BIND_IL_IMAGE_TYPE[type], data, dataSize);
    if (!imageLoaded)
    {
        // TODO: Add error check
        ilBindImage(0);
        ilDeleteImages(1, &image);
        return EFileNotFound("IlImage::LoadFromFileInMemory failed.");
    }

    // get image properties
	int numMipmaps = ilGetInteger(IL_NUM_MIPMAPS);
	numMipmaps     = numMipmaps == 0 ? 1 : numMipmaps;
	width          = ilGetInteger(IL_IMAGE_WIDTH);
	height         = ilGetInteger(IL_IMAGE_HEIGHT);
    depth          = ilGetInteger(IL_IMAGE_DEPTH);

    ILuint ilType   = ilGetInteger(IL_IMAGE_TYPE);
    ILuint ilFormat = ilGetInteger(IL_IMAGE_FORMAT);
    ILuint ilDXTC   = ilGetInteger(IL_DXTC_DATA_FORMAT);
    format          = FindTextureFormat(ilType, ilFormat, ilDXTC);

    // copy image
    size_t width_  = width;
    size_t height_ = height;
    size_t depth_  = depth;

    mipData.resize(numMipmaps);
    for (int i = 0; i<numMipmaps; ++i)
    {
        size_t dataSize = Image::SizeOfData(format, width_, height_, depth_, false);

        mipData[i] = (char*)malloc(dataSize);
        if (!mipData[i])
        {
            ilBindImage(0);
            ilDeleteImages(1, &image);
            return EOutOfMemory("IlImage::LoadFromFile failed. Can't allocate memory for image");
        }
        ilActiveMipmap(i);
        ilCopyPixels(0, 0, 0, width_, height_, depth_, ilFormat, ilType, mipData[i]);

        width_  >>= 1;
        height_ >>= 1;
        depth_  >>= 1;
    }

    ilBindImage(0);
    ilDeleteImages(1, &image);

	return SGL_OK;
}

SGL_HRESULT IlImage::SaveToFile(const char* fileName, 
                                FILE_TYPE   type) const
{
#ifndef SGL_NO_STATUS_CHECK
    if ( mipData.empty() ) {
        return EInvalidCall("IlImage::SaveToFile failed. Can't save empty image");
    }
#endif // SGL_NO_STATUS_CHECK

    // create image
    ILuint image;
    ilGenImages(1, &image);

    // copy image
    ilBindImage(image);
    ILboolean imageCopied = ilTexImage( width,
                                        height,
                                        depth,
                                        Texture::FORMAT_TRAITS[format].numComponents,
                                        BIND_GL_FORMAT_USAGE[format],
                                        BIND_GL_FORMAT_PIXEL_TYPE[format],
                                        mipData[0] );
    if (!imageCopied)
    {
        // TODO: Add error check
        ilBindImage(0);
        ilDeleteImages(1, &image);
        return EUnknown( (std::string("IlImage::SaveToFile failed. Can't save to image file: ") + fileName).c_str() );
    }

    // save
    ilEnable(IL_FILE_OVERWRITE);
    ILboolean imageSaved = false;
    if (type == AUTO) {
        imageSaved = ilSave(BIND_IL_IMAGE_TYPE[type], fileName);
    }
    else {
        imageSaved = ilSaveImage(fileName);
    }

    if (!imageSaved)
    {
        // TODO: Add error check
        ilBindImage(0);
        ilDeleteImages(1, &image);
        return EUnknown( (std::string("IlImage::SaveToFile failed. Can't save to image file: ") + fileName).c_str() );
    }

    ilBindImage(0);
    ilDeleteImages(1, &image);

    return SGL_OK;
}

SGL_HRESULT IlImage::SaveToFileInMemory(FILE_TYPE    type,
                                        unsigned int dataSize,
                                        void*        data) const
{
#ifndef SGL_NO_STATUS_CHECK
    if ( mipData.empty() ) {
        return EInvalidCall("IlImage::SaveToFile failed. Can't save empty image");
    }
#endif // SGL_NO_STATUS_CHECK

    // create image
    ILuint image;
    ilGenImages(1, &image);

    // copy image
    ilBindImage(image);
    ILboolean imageCopied = ilTexImage( width,
                                        height,
                                        depth,
                                        Texture::FORMAT_TRAITS[format].numComponents,
                                        BIND_GL_FORMAT_USAGE[format],
                                        BIND_GL_FORMAT_PIXEL_TYPE[format],
                                        mipData[0] );
    if (!imageCopied)
    {
        // TODO: Add error check
        ilBindImage(0);
        ilDeleteImages(1, &image);
        return EUnknown("IlImage::SaveToFileInMemory failed.");
    }

    // save
    ilEnable(IL_FILE_OVERWRITE);
    ILboolean imageSaved = ilSaveL(BIND_IL_IMAGE_TYPE[type], data, dataSize);
    if (!imageSaved)
    {
        // TODO: Add error check
        ilBindImage(0);
        ilDeleteImages(1, &image);
        return EUnknown("IlImage::SaveToFileInMemory failed.");
    }

    ilBindImage(0);
    ilDeleteImages(1, &image);

    return SGL_OK;
}

void SGL_DLLCALL IlImage::Clear()
{
    // remove mipmap data
    for (size_t i = 0; i<mipData.size(); ++i) {
        delete mipData[i];
    }
    mipData.clear();

    // clear props
    width  = 0;
    height = 0;
    depth  = 0;
    format = Texture::UNKNOWN;
}

Texture2D* IlImage::CreateTexture2D() const
{
    if ( !mipData.empty() )
    {
        // setup desc
        Texture2D::DESC desc;
        desc.format = format;
        desc.width  = width;
        desc.height = height;
        desc.data   = mipData[0];

        // create
        Texture2D* texture = device->CreateTexture2D(desc);
        if (!texture) {
            return 0;
        }

        // set mipmaps
        for (size_t i = 1; i<mipData.size(); ++i)
        {
            texture->SetSubImage( i,
                                  0,
                                  0,
                                  texture->Width()  >> i,
                                  texture->Height() >> i,
                                  mipData[i] );
        }

        return texture;
    }

    return 0;
}

Texture3D* IlImage::CreateTexture3D() const
{
    if ( !mipData.empty() )
    {
        // setup desc
        Texture3D::DESC desc;
        desc.format = format;
        desc.width  = width;
        desc.height = height;
        desc.data   = mipData[0];

        // create
        Texture3D* texture = device->CreateTexture3D(desc);
        if (!texture) {
            return 0;
        }

        // set mipmaps
        for (size_t i = 1; i<mipData.size(); ++i)
        {
            texture->SetSubImage( i,
                                  0,
                                  0,
                                  0,
                                  texture->Width()  >> i,
                                  texture->Height() >> i,
                                  texture->Depth()  >> i,
                                  mipData[i] );
        }

        return texture;
    }

    return 0;
}

