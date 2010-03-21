#ifndef SIMPLE_GL_GL_DEPTH_STENCIL_STATE_H
#define SIMPLE_GL_GL_DEPTH_STENCIL_STATE_H

#include "GLForward.h"
#include "../DepthStencilState.h"

namespace sgl {

/** Make suitable depth stencil state */
template<DEVICE_VERSION DeviceVersion>
sgl::DepthStencilState* sglCreateDepthStencilState(GLDevice<DeviceVersion>* device, const DepthStencilState::DESC& desc);

} // namepsace sgl

#endif // SIMPLE_GL_GL_DEPTH_STENCIL_STATE_H
