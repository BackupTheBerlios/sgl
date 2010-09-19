#ifndef SIMPLE_GL_GL_RASTERIZER_STATE_H
#define SIMPLE_GL_GL_RASTERIZER_STATE_H

#include "GLForward.h"
#include "../RasterizerState.h"

namespace sgl {

/** Make suitable rasterizer state */
template<DEVICE_VERSION DeviceVersion>
sgl::RasterizerState* sglCreateRasterizerState(GLDevice<DeviceVersion>* device, const RasterizerState::DESC& desc);

} // namepsace sgl

#endif // SIMPLE_GL_GL_RASTERIZER_STATE_H
