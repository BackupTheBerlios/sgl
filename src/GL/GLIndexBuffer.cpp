#include "GL/GLIndexBuffer.h"
#include <assert.h>
#include <string.h>

using namespace sgl;

GLIndexBuffer::GLIndexBuffer(GLDevice* device) :
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
        device->SetIndexBuffer(this, format);
    }
}

void GLIndexBuffer::Unbind() const
{
    if ( device->CurrentIndexBuffer() == this ) 
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        device->SetIndexBuffer(0, UINT_8);
    }
}