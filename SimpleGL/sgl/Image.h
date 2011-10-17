#ifndef SIMPLE_GL_IMAGE_H
#define SIMPLE_GL_IMAGE_H

#include "Utility/Referenced.h"
#include "Texture3D.h"
#include "TextureCube.h"
#include <cmath>
#ifdef SIMPLE_GL_ANDROID
#   include <android/asset_manager.h>
#endif

namespace sgl {

/** Image class. Images are stored in the RAM, you can create
 * load/save images, construct textures from images.
 * Cube map image is represented as 3D image with 6 layers.
 */
class Image :
	public Referenced
{
public:
    /** Type of the image for loading/saving. DevIL image library is used for image manipulation. 
     * List of the supported extensions is not full, se DevIL page for details: http://openil.sourceforge.net/
     */
    enum FILE_TYPE
    {
        AUTO,   /// determine file type by the extension
     	BMP,    /// load & save
        DDS,    /// load & save
        GIF,    /// load
        ICO,    /// load
        HDR,    /// load & save
        JPG,    /// load & save
        PIC,    /// load
        PNG,    /// load & save
        PSD,    /// load & save
        TGA,    /// load & save
        TIF     /// load & save
    };

public:
    /** Load image from file. Format is determined by extension.
     * @param fileName - name of the image file
     * @param type - type of the image file.
     * @param assetMgr - optional android asset manager to load from (can be NULL).
	 * @return result of the operation. Can be SGLERR_OUT_OF_MEMORY, SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL LoadFromFile(const char* 		fileName,
                                                 FILE_TYPE   		type = AUTO
												 #ifdef SIMPLE_GL_ANDROID
												 ,  AAssetManager*  assetMgr = 0
												 #endif
    											 ) = 0;

    /** Load image from file in memory.
     * @param dataSize - size of the image file.
     * @param data - image file data.
     * @param type - type of the image file.
     * @return result of the operation. Can be SGLERR_OUT_OF_MEMORY, SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL LoadFromFileInMemory(unsigned int dataSize,
                                                         const void*  data,
                                                         FILE_TYPE    type = AUTO) = 0;

    /** Save image to file. Format is determined by extension
     * @param fileName - name of the file with texture
     * @param type - type of the image file (if AUTO - determine by extension).
	 * @return result of the operation. Can be SGLERR_OUT_OF_MEMORY, SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL SaveToFile(const char* fileName, 
                                               FILE_TYPE   type = AUTO) const = 0;

    /** Save image to file in memory.
     * @param dataSize - size of the data for storing image.
     * @param data - output data.
     * @param type - type of the image file to save.
	 * @return result of the operation. Can be SGLERR_OUT_OF_MEMORY, SGLERR_INVALID_CALL.
     */
    virtual SGL_HRESULT SGL_DLLCALL SaveToFileInMemory(unsigned int dataSize,
                                                       void*        data,
                                                       FILE_TYPE    type) const = 0;

    /** Free image data */
    virtual void SGL_DLLCALL Clear() = 0;

    /** Setup 2d image data. Removes mipmaps.
     * @param format  - image internal format.
     * @param width   - image width.
     * @param height  - image height.
     * @param depth   - image depth.
	 * @param rawData - image pixels. If NULL then create an empty image.
     * @return result of the operation. Can be SGLERR_OUT_OF_MEMORY.
     *
    virtual SGL_HRESULT SGL_DLLCALL SetImage( Texture::FORMAT   format,
                                              unsigned          width,
                                              unsigned          height,
                                              unsigned          depth,
                                              const void*       rawData = 0 ) = 0;*/

    /** Setup sub image. Mipmap width and height are calculated
     * using mimap level.
     * @param mipmap - mipmap level of the image
     * @param offsetx - x offset of the sub image.
     * @param offsety - y offset of the sub image.
     * @param width - width of the sub image.
     * @param height - height of the sub image.
	 * @param rawData - image pixels in the format of the image.
     * @return result of the operation. Can be SGLERR_OUT_OF_MEMORY.
     *
    virtual SGL_HRESULT SGL_DLLCALL SetSubImage( unsigned int mipmap,
                                                 unsigned int offsetx,
                                                 unsigned int offsety,
                                                 unsigned int width,
                                                 unsigned int height,
                                                 const void*  rawData ) = 0;*/

    /** Create 1d texture from the image. Take into account only first row of zero layer. */
    //virtual Texture1D* SGL_DLLCALL CreateTexture1D() const = 0;

    /** Create 2d texture from the image. Take into account only zero layer. */
    virtual Texture2D* SGL_DLLCALL CreateTexture2D() const = 0;

    /** Create 3d texture from the image. */
    virtual Texture3D* SGL_DLLCALL CreateTexture3D() const = 0;

    /** Create cube texture from the image. Take into account first six layers of the image. */
    //virtual TextureCube* SGL_DLLCALL CreateTextureCube() const = 0;

    /** Get image format */
    virtual Texture::FORMAT SGL_DLLCALL Format() const = 0;

    /** Get number of mipmap levels allocated. */
    virtual unsigned int SGL_DLLCALL NumMipmaps() const = 0;

    /** Get width of the image */
    virtual unsigned int SGL_DLLCALL Width() const = 0;

    /** Get height of the image */
    virtual unsigned int SGL_DLLCALL Height() const = 0;

    /** Get depth of the image. */
    virtual unsigned int SGL_DLLCALL Depth() const = 0;

    /** Get image data. Do not free it! */
    virtual char* SGL_DLLCALL Data(unsigned int mipmap = 0) = 0;

    /** Get image data */
    virtual const char* SGL_DLLCALL Data(unsigned int mipmap = 0) const = 0;

    /** Calculate size of the image in bytes.
     * @param format - image format.
     * @param width - image width.
     * @param height - image height.
     * @param depth - image depth.
     * @param withMipmaps - sum with size of the full mipmap chain. Actual only
     * if image dimensions are power of two.
     * @return image size in bytes.
     */
    static inline unsigned int SizeOfData( Texture::FORMAT  format,
                                           unsigned         width, 
                                           unsigned         height, 
                                           unsigned         depth, 
                                           bool             withMipmaps  = false )
    {
        unsigned size = 0;
        if ( Texture::FORMAT_TRAITS[format].compressed )
        {
            unsigned blockSize = 0;
            switch (format)
            {
            case Texture::COMPRESSED_RGB_S3TC_DXT1:
            case Texture::COMPRESSED_RGBA_S3TC_DXT1:
                blockSize = 8;
                break;

            case Texture::COMPRESSED_RGBA_S3TC_DXT3:
            case Texture::COMPRESSED_RGBA_S3TC_DXT5:
                blockSize = 16;
                break;

            default:
                assert(!"Can't get here");
                break;
            }

            size = size_t( std::ceil(width / 4.0f) * std::ceil(height / 4.0f) * std::ceil(depth / 4.0f) * blockSize );
        }
        else {
            size = width * height * depth * Texture::FORMAT_TRAITS[format].sizeInBits / 8;
        }
        
        unsigned mipsSize = 0;
        if ( withMipmaps 
             && (width > 1 || height > 1 || depth > 1) )
        {
            mipsSize =  SizeOfData( format,
                                    std::max<unsigned>(width  >> 1, 1), 
                                    std::max<unsigned>(height >> 1, 1), 
                                    std::max<unsigned>(depth  >> 1, 1),
                                    true );
        }

        return mipsSize + size;
    }

    virtual ~Image() {}
};

// bindings for OpenIL

} // namespace sgl

#endif // SIMPLE_GL_IMAGE_H
