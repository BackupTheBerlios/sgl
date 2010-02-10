#include "Utility/RawImage.h"
#include <string>
#include <string.h>

#ifdef SIMPLE_GL_USE_SDL_IMAGE
#   include <SDL_image.h>
#elif defined(SIMPLE_GL_USE_DEVIL)
#   include <IL/il.h>
#   include "GL/GLTexture.h"
#endif // SIMPLE_GL_USE_SDL_IMAGE

using namespace std;
using namespace sgl;

namespace {

#ifdef SIMPLE_GL_USE_SDL_IMAGE
    Texture::FORMAT FindTextureFormat(const SDL_PixelFormat& format)
    {
        switch( format.BitsPerPixel )
        {
        case 8:
            return Texture::ALPHA8;

        case 16:
            return Texture::ALPHA16;

        case 24:
            return Texture::RGB8;

        case 32:
            return Texture::RGBA8;
        }

        return Texture2D::RGBA8;
    }

#elif defined(SIMPLE_GL_USE_DEVIL)
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

#endif // SIMPLE_GL_USE_SDL_IMAGE

} // anonymous namespace

// Define texture formats
const Texture::format_desc Texture::FORMAT_TRAITS[] =
{
    {0,      0,  false, false},

    {64,     4,  false, false},
    {48,     3,  false, false},
    {16,     1,  false, false},

    {32,     4,  false, false},
    {24,     3,  false, false},
    {8,      1,  false, false},
/*
    {16,     4,  false,  false, true,  false, false},
    {12,     3,  false,  false, true,  false, false},
    {4,      1,  false,  false, true,  false, false},
*/
    {32,     1,  true,  false},
    {24,     1,  true,  false},
    {16,     1,  true,  false},

    {32,     2,  true,  false},
    {32,     2,  true,  false},
    {40,     2,  true,  false},

    {128,    4,  false, false},
    {96,     3,  false, false},
    {64,     2,  false, false},
    {32,     1,  false, false},
    {32,     1,  false, false},

    {64,     4,  false, false},
    {48,     3,  false, false},
    {32,     2,  false, false},
    {16,     1,  false, false},
    {16,     1,  false, false},

    {128,    4,  false, false},
    {96,     3,  false, false},
    {32,     1,  false, false},

    {64,     4,  false, false},
    {48,     3,  false, false},
    {16,     1,  false, false},

    {32,     4,  false, false},
    {24,     3,  false, false},
    {8,      1,  false, false},

    {128,    4,  false, false},
    {96,     3,  false, false},
    {32,     1,  false, false},

    {64,     4,  false, false},
    {48,     3,  false, false},
    {16,     1,  false, false},

    {32,     4,  false, false},
    {24,     3,  false, false},
    {8,      1,  false, false},

    {96,     3,  false, true},
    {32,     4,  false, true},
    {64,     4,  false, true},
    {48,     4,  false, true},

    {16,     1,  false, true},
    {32,     1,  false, true},
    {24,     2,  false, true},
    {8,      2,  false, true}
};

RawImage::~RawImage()
{
    Clear();
}

char* SGL_DLLCALL RawImage::Data(unsigned int mipmap)
{
    return mipmap >= mipData.size() ? 0 : mipData[mipmap];
}

const char* SGL_DLLCALL RawImage::Data(unsigned int mipmap) const
{
    return mipmap >= mipData.size() ? 0 : mipData[mipmap];
}

SGL_HRESULT SGL_DLLCALL RawImage::LoadFromFile(const char* fileName)
{
    Clear();

#ifdef SIMPLE_GL_USE_SDL_IMAGE
    // load image
    SDL_Surface* image = IMG_Load(fileName);
    if( !image ) {
        return EUnknown( (string("RawImage::LoadFromFile failed: ") + IMG_GetError()).c_str() );
    }

    // copy glTexture
	width           = image->w;
	height          = image->h;
    depth           = 1;
    format          = FindTextureFormat(*image->format);
    size_t dataSize = Texture::GetSize(width, height, 1, format);

    mipData.resize(1);
    mipData[0] = (char*)malloc(dataSize);
    if (!mipData[0]) {
        return EOutOfMemory("RawImage::LoadFromFile failed. Can't allocate memory for image");
    }
    memcpy(mipData[0], image->pixels, dataSize);

	// free image
	SDL_FreeSurface(image);
#elif defined(SIMPLE_GL_USE_DEVIL)
    // create image
    ILuint image;
    ilGenImages(1, &image);

    // load image
    ilBindImage(image);
    ILboolean imageLoaded = ilLoadImage(fileName);
    if (!imageLoaded)
    {
        // TODO: Add error check
        ilBindImage(0);
        ilDeleteImages(1, &image);
        return EFileNotFound( (std::string("RawImage::LoadFromFile failed. Can't find image file: ") + fileName).c_str() );
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
        size_t dataSize = Texture::GetSize(width_, height_, depth_, format);

        mipData[i] = (char*)malloc(dataSize);
        if (!mipData[i])
        {
            ilBindImage(0);
            ilDeleteImages(1, &image);
            return EOutOfMemory("RawImage::LoadFromFile failed. Can't allocate memory for image");
        }
        ilActiveMipmap(i);
        ilCopyPixels(0, 0, 0, width_, height_, depth_, ilFormat, ilType, mipData[i]);

        width_  >>= 1;
        height_ >>= 1;
        depth_  >>= 1;
    }

    ilBindImage(0);
    ilDeleteImages(1, &image);
#endif

	return SGL_OK;
}

SGL_HRESULT SGL_DLLCALL RawImage::SaveToFile(const char* fileName) const
{
#ifndef SGL_NO_STATUS_CHECK
    if ( mipData.empty() ) {
        return EInvalidCall("RawImage::SaveToFile failed. Can't save empty image");
    }
#endif // SGL_NO_STATUS_CHECK
#ifdef SIMPLE_GL_USE_SDL_IMAGE
    // reserve data
    SDL_Surface* surface;
    Uint32 rmask, gmask, bmask;

    // SDL interprets each pixel as a 32-bit number, so our masks must depend
    // on the endianness (byte order) of the machine
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    #endif

    surface = SDL_CreateRGBSurface( SDL_SWSURFACE, width, height, 24,
                                    rmask, gmask, bmask, 0 );
    if ( !surface ) {
        return EUnknown( "Can't save glTexture: can't create sdl surface");
    }

    // copy data
    size_t dataSize = Texture::GetSize(width, height, 1, format);
    memcpy(surface->pixels, mipData[0], dataSize);

    // Save
    if ( SDL_SaveBMP(surface, fileName) != 0)
    {
        SDL_FreeSurface(surface);
        return EUnknown( "Can't save glTexture: can't save non bmp image" );
    }

    // free memory
    SDL_FreeSurface(surface);
#elif defined(SIMPLE_GL_USE_DEVIL)
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
        return EUnknown( (std::string("RawImage::SaveToFile failed. Can't save to image file: ") + fileName).c_str() );
    }

    // save
    ilEnable(IL_FILE_OVERWRITE);
    ILboolean imageSaved = ilSaveImage(fileName);
    if (!imageSaved)
    {
        // TODO: Add error check
        ilBindImage(0);
        ilDeleteImages(1, &image);
        return EUnknown( (std::string("RawImage::SaveToFile failed. Can't save to image file: ") + fileName).c_str() );
    }

    ilBindImage(0);
    ilDeleteImages(1, &image);
#endif // SIMPLE_GL_USE_SDL_IMAGE

    return SGL_OK;
}

SGL_HRESULT SGL_DLLCALL RawImage::SetImage( Texture::FORMAT _format,
                                            unsigned        _width,
                                            unsigned        _height,
                                            unsigned        _depth,
                                            const void*     rawData )
{
    Clear();

    format          = _format;
    width           = _width;
    height          = _height;
    depth           = _depth;
    size_t dataSize = Texture::GetSize(width, height, 1, format);

    mipData.resize(1);
    mipData[0] = (char*)malloc(dataSize);
    if (!mipData[0]) {
        return EOutOfMemory("RawImage::LoadFromFile failed. Can't allocate memory for image");
    }

    if (rawData) {
        memcpy(mipData[0], rawData, dataSize);
    }

    return SGL_OK;
}

SGL_HRESULT SGL_DLLCALL RawImage::SetMipLevel( unsigned int mipmap,
                                               const void*  rawData )
{
#ifndef SGL_NO_STATUS_CHECK
    if ( mipData.empty() ) {
        return EInvalidCall("RawImage::SetMipLevel failed. Can't setup mipmap of empty image");
    }
#endif // SGL_NO_STATUS_CHECK

    if ( mipmap > mipData.size() ) {
        mipData.resize(1);
    }
    size_t dataSize = Texture::GetSize( width >> mipmap,
                                        height >> mipmap,
                                        depth == 1 ? 1 : depth >> mipmap,
                                        format );

    mipData[mipmap] = (char*)malloc(dataSize);
    if (!mipData[mipmap]) {
        return EOutOfMemory("RawImage::SetMipLevel failed. Can't allocate memory for mipmap");
    }

    if (rawData) {
        memcpy(mipData[mipmap], rawData, dataSize);
    }

    return SGL_OK;
}

SGL_HRESULT SGL_DLLCALL RawImage::SetSubImageData( const void*              srcData,
                                                   Texture::FORMAT          dataFormat,
                                                   unsigned int             mipmap,
                                                   unsigned int             offsetX,
                                                   unsigned int             offsetY,
                                                   unsigned int             offsetZ,
                                                   unsigned int             regionWidth,
                                                   unsigned int             regionHeight,
                                                   unsigned int             regionDepth,
                                                   const FormatConversion*  conversion )
{
    #ifndef SGL_NO_STATUS_CHECK
    if (!srcData) {
        return EInvalidCall("RawImage::SetSubImageData filed. Data must not be NULL");
    }

    if ( mipData.empty() ) {
        return EInvalidCall("RawImage::SetSubImageData filed. Image is not allocated");
    }

    if ( mipmap >= mipData.size() ) {
        return EInvalidCall("RawImage::SetSubImageData filed. Image doesn't have requested mipmap");
    }

    // check dimensions
    if ( offsetX + regionWidth > width
        || offsetY + regionHeight > height
        || offsetZ + regionDepth > depth )
    {
        return EInvalidCall("RawImage::SetSubImageData filed. Sub image outbounds");
    }
    #endif

    // copy sub image
    unsigned int elemSize       = Texture::FORMAT_TRAITS[format].sizeInBits / 8;
    unsigned int sliceSize      = width * height * elemSize;
    unsigned int offsetInSlice  = (offsetY * width + offsetX) * elemSize;
    const char*  srcDataPtr     = (const char*)srcData;
    if (conversion)
    {
        unsigned int srcElemSize = Texture::FORMAT_TRAITS[dataFormat].sizeInBits / 8;
        for(unsigned int i = 0; i<regionDepth; ++i)
        {
            char* dataPtr = mipData[i] + (i + offsetZ) * sliceSize + offsetInSlice;
            for(unsigned int j = 0; j<regionHeight; ++j)
            {
                if ( !conversion->Convert(dataFormat, format, regionWidth, srcDataPtr, dataPtr) ) {
                    return EInvalidCall("RawImage::SetSubImageData filed. Couldn't perform conversion from source format to images format");
                }
                dataPtr    += width * elemSize;
                srcDataPtr += regionWidth * srcElemSize;
            }
        }
    }
    else if (dataFormat == format)
    {
        for(unsigned int i = 0; i<regionDepth; ++i)
        {
            char* dataPtr = mipData[i] + (i + offsetZ) * sliceSize + offsetInSlice;
            for(unsigned int j = 0; j<regionHeight; ++j)
            {
                memcpy(dataPtr, srcDataPtr, regionWidth * elemSize);
                dataPtr += width * elemSize;
                srcDataPtr += regionWidth * elemSize;
            }
        }
    }
    else {
        return EInvalidCall("RawImage::SetSubImageData filed. Converter is not specified and image formats differs");
    }

    return SGL_OK;
}

SGL_HRESULT SGL_DLLCALL RawImage::CopySubImage( const Image*            subImage,
                                                unsigned int            mipmap,
                                                unsigned int            offsetX,
                                                unsigned int            offsetY,
                                                unsigned int            offsetZ,
                                                const FormatConversion* conversion )
{
    #ifndef SGL_NO_STATUS_CHECK
    if (!subImage) {
        return EInvalidCall("RawImage::CopySubImage failed. SubImage must not be NULL");
    }

    if ( mipmap >= subImage->NumMipmaps() ) {
        return EInvalidCall("RawImage::CopySubImage failed. Image doesn't have requested mipmap");
    }
    #endif

    return SetSubImageData( subImage->Data(),
                            subImage->Format(),
                            mipmap,
                            offsetX,
                            offsetY,
                            offsetZ,
                            subImage->Width(),
                            subImage->Height(),
                            subImage->Depth(),
                            conversion );
}

void SGL_DLLCALL RawImage::Clear()
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

unsigned int SGL_DLLCALL RawImage::SizeOfData() const
{
    size_t dataSize = 0;

    size_t width_  = width;
    size_t height_ = height;
    size_t depth_  = depth;
    for (size_t i = 0; i<mipData.size(); ++i)
    {
        dataSize += Texture::GetSize(width_, height_, depth_, format);

        width_  >>= 1;
        height_ >>= 1;
        depth_  >>= 1;
    }

    return dataSize;
}

sgl::Texture3D* SGL_DLLCALL RawImage::CreateTexture3D() const
{
    #ifndef SGL_NO_STATUS_CHECK
    if ( mipData.empty() )
    {
        EInvalidCall("RawImage::CreateTexture3D failed. Image is not allocated");
        return 0;
    }
    #endif

    sgl::Texture3D* texture = device->CreateTexture3D();
    texture->SetImage(format, width, height, depth, mipData[0]);

    return texture;
}

sgl::Texture3D* SGL_DLLCALL RawImage::CreateTexture3D( unsigned int offsetX,
                                                       unsigned int offsetY,
                                                       unsigned int offsetZ,
                                                       unsigned int regionWidth,
                                                       unsigned int regionHeight,
                                                       unsigned int regionDepth ) const
{
#ifndef SGL_NO_STATUS_CHECK
    if ( mipData.empty() )
    {
        EInvalidCall("RawImage::CreateTexture3D failed. Image is not allocated");
        return 0;
    }

    if ( offsetX + regionWidth> width
         || offsetY + regionHeight > height
         || offsetZ + regionDepth > depth )
    {
        EInvalidCall("RawImage::CreateTexture3D filed. Sub region is out of bounds");
        return 0;
    }
#endif

    sgl::Texture3D* texture = device->CreateTexture3D();
    texture->SetImage(format, regionWidth, regionHeight, regionDepth, 0);

    unsigned int elemSize      = Texture::FORMAT_TRAITS[format].sizeInBits / 8;
    unsigned int sliceSize     = width * height * elemSize;
    unsigned int offsetInSlice = (offsetY * width + offsetX) * elemSize;
    for(unsigned int i = 0; i<depth; ++i)
    {
        char* dataPtr = mipData[0] + i * sliceSize + offsetInSlice;
        for(unsigned int j = offsetY; j<offsetY + regionHeight; ++j)
        {
            texture->SetSubImage(0, 0, i, width, height, 1, dataPtr);
            dataPtr += width * elemSize;
        }
    }

    return texture;
}

sgl::Texture2D* SGL_DLLCALL RawImage::CreateTexture2D() const
{
    Texture2D* texture = device->CreateTexture2D();
    if ( SGL_OK != texture->SetImage(this) )
    {
        delete texture;
        return 0;
    }

    return texture;
}

sgl::Texture2D* SGL_DLLCALL RawImage::CreateTexture2D( unsigned int offsetX,
                                                       unsigned int offsetY,
                                                       unsigned int offsetZ,
                                                       unsigned int width,
                                                       unsigned int height ) const
{
    return 0;
}
