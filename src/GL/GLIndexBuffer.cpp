#include "GL/GLIndexBuffer.h"

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
GLIndexBuffer<DeviceVersion>::GLIndexBuffer(GLDevice<DeviceVersion>* device) :
    GLBuffer<DeviceVersion, IndexBuffer>(device, GL_ELEMENT_ARRAY_BUFFER)
{
}

template<DEVICE_VERSION DeviceVersion>
GLIndexBuffer<DeviceVersion>::~GLIndexBuffer()
{
}

template<DEVICE_VERSION DeviceVersion>
void GLIndexBuffer<DeviceVersion>::Bind(IndexBuffer::INDEX_TYPE format) const
{   
    if (device->CurrentIndexBuffer() != this || device->CurrentIndexFormat() != format)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer);
        device->SetIndexBuffer(this, format);
    }
}

template<DEVICE_VERSION DeviceVersion>
void GLIndexBuffer<DeviceVersion>::Unbind() const
{
    if ( device->CurrentIndexBuffer() == this ) 
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        device->SetIndexBuffer(0, UINT_8);
    }
}

template class GLIndexBuffer<DV_OPENGL_1_3>;
template class GLIndexBuffer<DV_OPENGL_1_4>;
template class GLIndexBuffer<DV_OPENGL_1_5>;
template class GLIndexBuffer<DV_OPENGL_2_0>;
template class GLIndexBuffer<DV_OPENGL_2_1>;
template class GLIndexBuffer<DV_OPENGL_3_0>;
template class GLIndexBuffer<DV_OPENGL_3_1>;
template class GLIndexBuffer<DV_OPENGL_3_2>;

} // namespace sgl
