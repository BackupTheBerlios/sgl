#ifndef SIMPLE_GL_CONSTANT_BUFFER_H
#define SIMPLE_GL_CONSTANT_BUFFER_H

#include "Buffer.h"

namespace sgl {

/** Constant buffer for shader */
class ConstantBuffer :
    public Buffer
{
public:
    virtual SGL_DLLCALL ~ConstantBuffer() {}
};

} // namespace sgl

#endif // SIMPLE_GL_CONSTANT_BUFFER_H
