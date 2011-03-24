#ifndef SIMPLE_GL_BUFFER_H
#define SIMPLE_GL_BUFFER_H

#include "Resource.h"

namespace sgl {

/** GPU-side buffer view. */
class Buffer :
    public Resource
{
public:
    static const int MAP_READ_BIT              = 1;
    static const int MAP_WRITE_BIT             = 1 << 1;
    static const int MAP_INVALIDATE_RANGE_BIT  = 1 << 2;
    static const int MAP_INVALIDATE_BUFFER_BIT = 1 << 3;
    static const int MAP_UNSYNCHRONIZED_BIT    = 1 << 4;

public:
    /** Gain access to the buffer data. Force buffer to be binded.
     * @param hint - data access hints.
     * @param data - pointer to the beginning of mapped data.
     */
    virtual SGL_HRESULT SGL_DLLCALL Map(int     hint, 
                                        void**  data) = 0;

    /** Gain access to the buffer data. Force buffer to be binded.
     * @param offset - offset in data to lock in bytes.
     * @param size - size of the data for lock.
     * @param hint - data access hints.
     * @param data - pointer to the beginning of mapped data.
     */
    virtual SGL_HRESULT SGL_DLLCALL MapRange(unsigned   offset, 
                                             unsigned   size, 
                                             int        hint, 
                                             void**     data) = 0;

    /** Flush currently mapped range to the buffer.
     * @param offset - offset in data to lock in bytes.
     * @param size - size of the data for lock.
     */
    virtual SGL_HRESULT SGL_DLLCALL FlushMappedRange(unsigned offset, unsigned size) = 0;

    /** End work with buffer data */
    virtual SGL_HRESULT SGL_DLLCALL Unmap() = 0;

    /** Check wether buffer is mapped */
    virtual bool SGL_DLLCALL Mapped() const = 0;

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
