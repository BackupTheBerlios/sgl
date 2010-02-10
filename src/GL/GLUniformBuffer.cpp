#include "GL/GLConstantBuffer.h"

using namespace sgl;

GLConstantBuffer::GLConstantBuffer(Device* _device) :
    GLBuffer<ConstantBuffer>(_device, GL_UNIFORM_BUFFER_EXT, GL_STATIC_READ)
{
}
