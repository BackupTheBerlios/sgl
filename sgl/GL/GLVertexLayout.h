#ifndef SIMPLE_GL_GL_VERTEX_LAYOUT_H
#define SIMPLE_GL_GL_VERTEX_LAYOUT_H

#include "GLForward.h"
#include "../VertexLayout.h"

namespace sgl {

extern const unsigned BIND_SCALAR_TYPE[__NUMBER_OF_SCALAR_TYPES__];

/* Vertex declaration */
class GLVertexLayoutMixed:
	public ReferencedImpl<VertexLayout>
{
public:
	GLVertexLayoutMixed( GLDevice*		device, 
						 unsigned int   numElements, 
					 	 const ELEMENT* elements );
	~GLVertexLayoutMixed();

	// Override VertexLayout
	unsigned int    SGL_DLLCALL NumElements() const                 { return elements.size(); }
	ELEMENT         SGL_DLLCALL Element(unsigned int index) const   { return elements[index]; }

	void SGL_DLLCALL Bind() const;
	void SGL_DLLCALL Unbind() const;

private:
	GLDevice*				device;
	std::vector<ELEMENT>	elements;
};

/* Vertex layout with attributes only */
class GLVertexLayoutAttribute :
	public ReferencedImpl<VertexLayout>
{
public:
	GLVertexLayoutAttribute( GLDevice*		device, 
							 unsigned int   numElements, 
							 const ELEMENT* elements );
	~GLVertexLayoutAttribute();

	// Override VertexLayout
	unsigned int    SGL_DLLCALL NumElements() const                 { return elements.size(); }
	ELEMENT         SGL_DLLCALL Element(unsigned int index) const   { return elements[index]; }

	void SGL_DLLCALL Bind() const;
	void SGL_DLLCALL Unbind() const;

private:
	GLDevice*				device;
	std::vector<ELEMENT>	elements;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_VERTEX_LAYOUT_H
