#ifndef SIMPLE_GL_BUFFER_H
#define SIMPLE_GL_BUFFER_H

#include "Resource.h"

namespace sgl {

/** GPU-side buffer view. */
class Buffer :
    public Resource
{
public:
    /** Send data in the GPU buffer.
     * @param dataSize - size of the data
     * @param data - data to store in the buffer
	 * @return the result of the operation. Can be SGLERR_OUT_OF_MEMORY
     */
    virtual SGL_HRESULT SGL_DLLCALL SetData( unsigned int   dataSize,
                                             const void*    data ) = 0;

    /** Send sub data chunk in the GPU buffer.
     * @param offset - offset in the buffer where to store the data
     * @param dataSize - size of the data
     * @param data - data to store in the buffer
	 * @return the result of the operation. Can be SGLERR_INVALID_CALL if
     * the offset and dataSize parameters incorrect.
     */
    virtual SGL_HRESULT SGL_DLLCALL SetSubData( unsigned int    offset,
                                                unsigned int    dataSize,
                                                const void*     data ) = 0;

    /** Get data from the buffer.
     * @param data[out] - memory to store the data.
     * @param offset - offset from the beginning of the buffer for retreiving data.
     * @param dataSize - size of the buffer chunk for retrieving.
     */
    virtual SGL_HRESULT SGL_DLLCALL GetData( void*         data,
                                             unsigned int  offset,
                                             unsigned int  dataSize ) = 0;

    /** Get size of the data stored in the buffer
	 * @return size of the data buffer
	 */
	virtual unsigned int SGL_DLLCALL Size() const = 0;

    virtual ~Buffer() {}
};

} // namespace sgl

#endif // SIMPLE_GL_BUFFER_H
