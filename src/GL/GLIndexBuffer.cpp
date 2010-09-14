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
    Unbind();
}

template<DEVICE_VERSION DeviceVersion>
void GLIndexBuffer<DeviceVersion>::Bind(IndexBuffer::INDEX_TYPE format) const
{   
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, base_type::glBuffer);
    base_type::device->SetIndexBuffer(this, format);
}

template<DEVICE_VERSION DeviceVersion>
void GLIndexBuffer<DeviceVersion>::Unbind() const
{
    if (base_type::device->CurrentIndexBuffer() == this)
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
        base_type::device->SetIndexBuffer(0, base_type::UINT_8);
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
