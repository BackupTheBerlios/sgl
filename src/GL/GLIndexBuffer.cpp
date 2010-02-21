#include "GL/GLIndexBuffer.h"
#include <assert.h>
#include <string.h>

using namespace sgl;

GLIndexBuffer::GLIndexBuffer(Device* device) :
    GLBuffer<IndexBuffer>(device, GL_ELEMENT_ARRAY_BUFFER)
{
}

GLIndexBuffer::~GLIndexBuffer()
{
}

void GLIndexBuffer::Bind(INDEX_TYPE format) const
{   
    if (device->CurrentIndexBuffer() != this || device->CurrentIndexFormat() != format)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer);
        static_cast< GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>* >(device.get())->SetIndexBuffer(this, format);
    }
}

void GLIndexBuffer::Unbind() const
{
    if ( device->CurrentIndexBuffer() == this ) 
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        static_cast< GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>* >(device.get())->SetIndexBuffer(0, UINT_8);
    }
}