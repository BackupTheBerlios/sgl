#ifndef SIMPLE_GL_GL_BLEND_STATE_H
#define SIMPLE_GL_GL_BLEND_STATE_H

#include "GLForward.h"
#include "../BlendState.h"

namespace sgl {

/** Make suitable blend state */
template<DEVICE_VERSION DeviceVersion>
sgl::BlendState* sglCreateBlendState(GLDevice<DeviceVersion>* device, const BlendState::DESC& desc);

} // namepsace sgl

#endif // SIMPLE_GL_GL_BLEND_STATE_H
