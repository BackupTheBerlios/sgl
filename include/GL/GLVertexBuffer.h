#ifndef SIMPLE_GL_GL_VERTEX_BUFFER_H
#define SIMPLE_GL_GL_VERTEX_BUFFER_H

#include "GLBuffer.h"
#include "GLVertexLayout.h"

namespace sgl {

class GLVertexBuffer :
    public GLBuffer<VertexBuffer>
{
public:
    GLVertexBuffer(Device* device);
    ~GLVertexBuffer();

    // Override VertexBufferView
    void    SGL_DLLCALL Bind(const VertexLayout* layout) const;
    void    SGL_DLLCALL Unbind() const;

protected:
	// settings
	mutable ref_ptr<GLVertexLayout> layout;
};

}

#endif // SIMPLE_GL_GL_VERTEX_BUFFER_H
