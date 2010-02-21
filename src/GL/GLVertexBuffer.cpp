#include "GL/GLVertexBuffer.h"
#include "GL/GLCommon.h"
#include <algorithm>
#include <functional>
#include <cassert>
#include <cstring>

using namespace std;
using namespace sgl;
using namespace math;

GLVertexBuffer::GLVertexBuffer(Device* device) :
    GLBuffer<VertexBuffer>(device,  GL_ARRAY_BUFFER)
{
}

GLVertexBuffer::~GLVertexBuffer()
{
}

// other
void GLVertexBuffer::Bind(const VertexLayout* layout) const
{
    if (device->CurrentVertexBuffer() != this)
    {
        glBindBufferARB(GL_ARRAY_BUFFER, glBuffer);
        static_cast< GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>* >(device.get())->SetVertexBuffer(this);
    }

    if (layout) {
        layout->Bind();
    }
}

void GLVertexBuffer::Unbind() const
{
    if (device->CurrentVertexBuffer() == this)
    {
        glBindBufferARB(GL_ARRAY_BUFFER, 0);
	    static_cast< GLDevice<DV_OPENGL_2_1_PROGRAMMABLE>* >(device.get())->SetVertexBuffer(0);
    }
}
