#ifndef SIMPLE_GL_GL_CONSTANT_BUFFER_H
#define SIMPLE_GL_GL_CONSTANT_BUFFER_H

#include "GLBuffer.h"

namespace sgl {

/* Vertex buffer object interface */
class GLConstantBuffer :
    public GLBuffer<ConstantBuffer>
{
friend class GLDevice;
protected:
    GLConstantBuffer(Device* _pDevice);

public:
    virtual TYPE SGL_DLLCALL Type() const { return CONSTANT_BUFFER; }

    virtual ~GLConstantBuffer() {}
};

}

#endif // SIMPLE_GL_GL_CONSTANT_BUFFER_H
