#ifndef SIMPLE_GL_VERTEX_LAYOUT_H
#define SIMPLE_GL_VERTEX_LAYOUT_H

#include "Utility/Referenced.h"

namespace sgl {

/** Specifies layout of the vertices in the buffer. */
class VertexLayout :
    public Referenced
{
public:
	/** Semantic of the vertex element */
	enum SEMANTIC
	{
        VERTEX,
		NORMAL,
		COLOR,
		TEXCOORD,
        ATTRIBUTE,
    };

	/** VertexDeclaration element */
	struct ELEMENT
	{
		unsigned int	index;      /// Index of the attribute. For FFP rendering can be texture coordinate index if semantic is TEXCOORD.
		unsigned int	size;       /// Number of the components (1,2,3 or 4).
		unsigned int	offset;     /// Offset from the beginning of the vertex.
        unsigned int    stride;     /// Stride between elements.
        SCALAR_TYPE		type;       /// Type of the each component in the vertex.
		SEMANTIC		semantic;   /// Optional value - semantic of the attribute.
	};

public:
	/** Get number of elements in the vertex declaration */
	virtual unsigned int SGL_DLLCALL NumElements() const = 0;

    /** Get i'th element of the vertex declaration. Elements can be shuffled
     * for better performance during the initialization. 
     */
	virtual ELEMENT SGL_DLLCALL Element(unsigned int index) const = 0;

    /** Setup vertex declaration to the device. */
    virtual void SGL_DLLCALL Bind() const = 0;

    /** End work with vertex declaration */
    virtual void SGL_DLLCALL Unbind() const = 0;

    virtual ~VertexLayout() {}
};

} // namespace sgl

#endif // SIMPLE_GL_VERTEX_LAYOUT_H
