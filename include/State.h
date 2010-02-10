#ifndef SIMPLE_GL_STATE_H
#define SIMPLE_GL_STATE_H

#include "Utility/Referenced.h"

namespace sgl {

/** Device state */
class State :
	public Referenced
{
public:
    /** Type of the state */
    enum TYPE
    {
        BLEND_STATE,
        DEPTH_STENCIL_STATE,
        RASTERIZER_STATE,
        SAMPLER_STATE,
        __NUMBER_OF_STATE_TYPE__,                                           /// Number of states
        __NUMBER_OF_STATES_WITH_SAMPLERS__ = __NUMBER_OF_STATE_TYPE__ + 7   /// Number of states with samplers
    };

public:
    /** Get type of the state. */
    virtual TYPE SGL_DLLCALL Type() const = 0;

    virtual ~State() {}
};

} // namepsace sgl

#endif // SIMPLE_GL_STATE_H
