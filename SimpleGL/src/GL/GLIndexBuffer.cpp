#include "GL/GLIndexBuffer.h"

namespace sgl {

template<typename BufferImpl>
GLIndexBuffer<BufferImpl>::GLIndexBuffer(GLDevice* device) :
    BufferImpl(device, GL_ELEMENT_ARRAY_BUFFER)
{
}

template<typename BufferImpl>
GLIndexBuffer<BufferImpl>::~GLIndexBuffer()
{
    Unbind();
}

template<typename BufferImpl>
void GLIndexBuffer<BufferImpl>::Bind(IndexBuffer::INDEX_TYPE format) const
{   
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferImpl::glBuffer);
    BufferImpl::device->SetIndexBuffer(this, format);
}

template<typename BufferImpl>
void GLIndexBuffer<BufferImpl>::Unbind() const
{
    if (BufferImpl::device->CurrentIndexBuffer() == this)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        BufferImpl::device->SetIndexBuffer(0, IndexBuffer::UINT_8);
    }
}

template class GLIndexBuffer< GLBufferDefault<IndexBuffer> >;
#ifndef SIMPLE_GL_ES
template class GLIndexBuffer< GLBufferModern<IndexBuffer> >;
#endif

} // namespace sgl
