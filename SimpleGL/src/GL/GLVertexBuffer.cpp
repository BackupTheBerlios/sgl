#include "GL/GLVertexBuffer.h"
#include "GL/GLCommon.h"
#include <algorithm>
#include <functional>
#include <cassert>
#include <cstring>

namespace sgl {

template<typename BufferImpl>
GLVertexBuffer<BufferImpl>::GLVertexBuffer(GLDevice* device) :
    BufferImpl(device, GL_ARRAY_BUFFER)
{
}

template<typename BufferImpl>
GLVertexBuffer<BufferImpl>::~GLVertexBuffer()
{
    if (BufferImpl::device->Valid()) {
    	Unbind();
    }
}

// other
template<typename BufferImpl>
void GLVertexBuffer<BufferImpl>::Bind(const VertexLayout* layout) const
{
    glBindBuffer(GL_ARRAY_BUFFER, BufferImpl::glBuffer);
    BufferImpl::device->SetVertexBuffer(this);

    if (layout) {
        layout->Bind();
    }
}

template<typename BufferImpl>
void GLVertexBuffer<BufferImpl>::Unbind() const
{
    if (BufferImpl::device->CurrentVertexBuffer() == this)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        BufferImpl::device->SetVertexBuffer(0);
    }
}

template class GLVertexBuffer< GLBufferDefault<VertexBuffer> >;
#ifndef SIMPLE_GL_ES
template class GLVertexBuffer< GLBufferModern<VertexBuffer> >;
#endif

} // namespace sgl
