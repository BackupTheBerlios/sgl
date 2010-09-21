#ifndef SIMPLE_GL_GL_VERTEX_BUFFER_H
#define SIMPLE_GL_GL_VERTEX_BUFFER_H

#include "GLBuffer.h"
#include "GLVertexLayout.h"

namespace sgl {

class GLVertexBuffer :
    public GLBuffer<VertexBuffer>
{
public:
    GLVertexBuffer(GLDevice* device);
    ~GLVertexBuffer();

    // Override VertexBuffer
    void SGL_DLLCALL Bind(const VertexLayout* layout) const;
    void SGL_DLLCALL Unbind() const;
};

}

#endif // SIMPLE_GL_GL_VERTEX_BUFFER_H
