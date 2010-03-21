#include "GL/GLVertexBuffer.h"
#include "GL/GLCommon.h"
#include <algorithm>
#include <functional>
#include <cassert>
#include <cstring>

namespace sgl {

using namespace std;
using namespace math;

template<DEVICE_VERSION DeviceVersion>
GLVertexBuffer<DeviceVersion>::GLVertexBuffer(GLDevice<DeviceVersion>* device) :
    GLBuffer<DeviceVersion, VertexBuffer>(device, GL_ARRAY_BUFFER)
{
}

template<DEVICE_VERSION DeviceVersion>
GLVertexBuffer<DeviceVersion>::~GLVertexBuffer()
{
}

// other
template<DEVICE_VERSION DeviceVersion>
void GLVertexBuffer<DeviceVersion>::Bind(const VertexLayout* layout) const
{
    if (device->CurrentVertexBuffer() != this)
    {
        glBindBufferARB(GL_ARRAY_BUFFER, glBuffer);
        device->SetVertexBuffer(this);
    }

    if (layout) {
        layout->Bind();
    }
}

template<DEVICE_VERSION DeviceVersion>
void GLVertexBuffer<DeviceVersion>::Unbind() const
{
    if (device->CurrentVertexBuffer() == this)
    {
        glBindBufferARB(GL_ARRAY_BUFFER, 0);
	    device->SetVertexBuffer(0);
    }
}

template class GLVertexBuffer<DV_OPENGL_1_3>;
template class GLVertexBuffer<DV_OPENGL_1_4>;
template class GLVertexBuffer<DV_OPENGL_1_5>;
template class GLVertexBuffer<DV_OPENGL_2_0>;
template class GLVertexBuffer<DV_OPENGL_2_1>;
template class GLVertexBuffer<DV_OPENGL_3_0>;
template class GLVertexBuffer<DV_OPENGL_3_1>;
template class GLVertexBuffer<DV_OPENGL_3_2>;

} // namespace sgl