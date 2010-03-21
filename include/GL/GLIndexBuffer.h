#ifndef SIMPLE_GL_GL_INDEX_BUFFER_H
#define SIMPLE_GL_GL_INDEX_BUFFER_H

#include "GLBuffer.h"

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
class GLIndexBuffer :
    public GLBuffer<DeviceVersion, IndexBuffer>
{
public:
    GLIndexBuffer(GLDevice<DeviceVersion>* device);
    ~GLIndexBuffer();

    // Override IndexBufferView
    void SGL_DLLCALL Bind(IndexBuffer::INDEX_TYPE format) const;
    void SGL_DLLCALL Unbind() const;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_INDEX_BUFFER_H
