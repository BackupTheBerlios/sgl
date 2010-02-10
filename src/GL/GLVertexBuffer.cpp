#include "GL/GLVertexBuffer.h"
#include "GL/GLCommon.h"
#include <algorithm>
#include <functional>
#include <cassert>
#include <cstring>

using namespace std;
using namespace sgl;
using namespace math;

GLVertexBuffer::GLVertexBuffer(GLDevice* device) :
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
        device->SetVertexBuffer(this);
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
	    device->SetVertexBuffer(0);
        device->SetVertexLayout(0);
    }
}
