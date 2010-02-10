#ifndef SIMPLE_GL_FORMAT_CONVERSION_H
#define SIMPLE_GL_FORMAT_CONVERSION_H

#include "Texture.h"

namespace sgl {

/** Interface for format converter operators
 */
class FormatConversion
{
public:
    /** Convert pixels from source format to dest format 
     * @param sourceFormat - format of the source pixels
     * @param destFormat - format of the dest pixels
     * @param numPixels - number of pixels to convert
     * @param source - source pixels
     * @param dest [out] - destination pixels
     * @return true if source pixels succesfully converted, false
     * if conversion operation is not supported.
     */
    virtual bool Convert( Texture::FORMAT sourceFormat,
                          Texture::FORMAT destFormat,
                          unsigned int numPixels,
                          const void* source,
                          void* dest ) const = 0;

	virtual ~FormatConversion() {}
};

} // namespace sgl

#endif // SIMPLE_GL_FORMAT_CONVERSION_H
