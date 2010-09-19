#ifndef SIMPLE_GL_VBO_H
#define SIMPLE_GL_VBO_H

#include <vector>
#include "Math/Matrix.hpp"
#include "Buffer.h"
#include "VertexLayout.h"

namespace sgl {

/** Vertex buffer object interface */
class VertexBuffer :
    public Buffer
{
public:
    /** Bind vertex buffer as source for vertex attributes. 
     * @param layout - layout of the atrtibutes in the vertex buffer.
     */
    virtual void SGL_DLLCALL Bind(const VertexLayout* layout) const = 0;

    /** Unbind vertex buffer from the device */
    virtual void SGL_DLLCALL Unbind() const = 0;
   
    virtual ~VertexBuffer() {}
};

} // namespace sgl

#endif // SIMPLE_GL_VBO_H
