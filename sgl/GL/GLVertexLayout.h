#ifndef SIMPLE_GL_GL_VERTEX_LAYOUT_H
#define SIMPLE_GL_GL_VERTEX_LAYOUT_H

#include "GLForward.h"
#include "../VertexLayout.h"

namespace sgl {

extern const unsigned BIND_SCALAR_TYPE[__NUMBER_OF_SCALAR_TYPES__];

/** Make suitable vertex layout */
template<DEVICE_VERSION DeviceVersion>
sgl::VertexLayout* sglCreateVertexLayout(GLDevice<DeviceVersion>*       device, 
                                         unsigned int                   numElements, 
                                         const VertexLayout::ELEMENT*   element);


} // namespace sgl

#endif // SIMPLE_GL_GL_VERTEX_LAYOUT_H
