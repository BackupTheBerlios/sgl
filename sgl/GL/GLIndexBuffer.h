#ifndef SIMPLE_GL_GL_INDEX_BUFFER_H
#define SIMPLE_GL_GL_INDEX_BUFFER_H

#include "GLBuffer.h"

namespace sgl {

class GLIndexBuffer :
    public GLBuffer<IndexBuffer>
{
public:
    GLIndexBuffer(GLDevice* device);
    ~GLIndexBuffer();

    // Override IndexBufferView
    void SGL_DLLCALL Bind(IndexBuffer::INDEX_TYPE format) const;
    void SGL_DLLCALL Unbind() const;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_INDEX_BUFFER_H
