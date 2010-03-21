#ifndef SIMPLE_GL_GL_VERTEX_BUFFER_H
#define SIMPLE_GL_GL_VERTEX_BUFFER_H

#include "GLBuffer.h"
#include "GLVertexLayout.h"

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
class GLVertexBuffer :
    public GLBuffer<DeviceVersion, VertexBuffer>
{
public:
    GLVertexBuffer(GLDevice<DeviceVersion>* device);
    ~GLVertexBuffer();

    // Override VertexBuffer
    void SGL_DLLCALL Bind(const VertexLayout* layout) const;
    void SGL_DLLCALL Unbind() const;
};

}

#endif // SIMPLE_GL_GL_VERTEX_BUFFER_H
