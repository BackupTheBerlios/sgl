#include "GL/GLIndexBuffer.h"

namespace sgl {

GLIndexBuffer::GLIndexBuffer(GLDevice* device) :
    GLBuffer<IndexBuffer>(device, GL_ELEMENT_ARRAY_BUFFER)
{
}

GLIndexBuffer::~GLIndexBuffer()
{
    Unbind();
}

void GLIndexBuffer::Bind(IndexBuffer::INDEX_TYPE format) const
{   
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, glBuffer);
    device->SetIndexBuffer(this, format);
}

void GLIndexBuffer::Unbind() const
{
    if (device->CurrentIndexBuffer() == this)
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
        device->SetIndexBuffer(0, UINT_8);
    }
}

} // namespace sgl
