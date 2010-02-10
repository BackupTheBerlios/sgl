#ifndef SIMPLE_GL_GL_VERTEX_LAYOUT_H
#define SIMPLE_GL_GL_VERTEX_LAYOUT_H

#include "GLDevice.h"

namespace sgl {

extern const GLenum BIND_SCALAR_TYPE[__NUMBER_OF_SCALAR_TYPES__];

/* Vertex declaration */
class GLVertexLayout:
    public ReferencedImpl<VertexLayout>
{
public:
	GLVertexLayout( GLDevice*       device, 
                    unsigned int    numElements, 
                    const ELEMENT*  elements );
    ~GLVertexLayout();

    // Override VertexLayout
    unsigned int    SGL_DLLCALL NumElements() const                 { return elements.size(); }
    ELEMENT         SGL_DLLCALL Element(unsigned int index) const   { return elements[index]; }

    void            SGL_DLLCALL Bind() const;
    void            SGL_DLLCALL Unbind() const;

private:
    ref_ptr<GLDevice>       device;
	std::vector<ELEMENT>	elements;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_VERTEX_LAYOUT_H
