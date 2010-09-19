#ifndef SIMPLE_GL_INDEX_BUFFER_H
#define SIMPLE_GL_INDEX_BUFFER_H

#include "Buffer.h"

namespace sgl {

/** Index buffer view. */
class IndexBuffer :
    public Buffer
{
public:
    /** Type of the indices in the buffer */
    enum INDEX_TYPE
    {
        UINT_8,                 /// Commonly emulated in hardware
        UINT_16,                /// Hardware support by any hardware
        UINT_32 ,               /// Sometimes emulated in hardware
        __NUM_OF_INDEX_TYPES__
    };

public:
    /** Setup index buffer for fetching indices.
     * @param format - type of the indices in the buffer.
     */
    virtual void SGL_DLLCALL Bind(INDEX_TYPE format) const = 0;

    /** Unbind index buffer from the device. */
    virtual void SGL_DLLCALL Unbind() const = 0;

    virtual ~IndexBuffer() {}
};

} // namespace sgl

#endif // SIMPLE_GL_INDEX_BUFFER_H
