#ifndef SIMPLE_GL_BLEND_STATE_H
#define SIMPLE_GL_BLEND_STATE_H

#include "State.h"

namespace sgl {

/** Blend state. 
 * TODO: AlphaToCoverage
 *       RenderTargetWriteMask
 */
class BlendState :
    public State
{
public:
    /** blending operation */
    enum OPERATION
    {
        ADD,
        SUBTRACT,
        REVERSE_SUBTRACT,
        MIN,
        MAX
    };

    /** blending function */
    enum FUNCTION
    {
        ZERO,
        ONE,
        SRC_COLOR,
        ONE_MINUS_SRC_COLOR,
        DST_COLOR,
        ONE_MINUS_DST_COLOR,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
        CONSTANT_COLOR,
        ONE_MINUS_CONSTANT_COLOR,
        CONSTANT_ALPHA,
        ONE_MINUS_CONSTANT_ALPHA,
        SRC_ALPHA_SATURATE
    };

    /** Description of the blend state */
    struct DESC
    {
        enum { ACTUAL_SIZE = 4 };

        unsigned int    blendEnable     : 8;    /// bitfield mask
        FUNCTION        srcBlend        : 4;
        FUNCTION        destBlend       : 4;
        OPERATION       blendOp         : 4;
        FUNCTION        srcBlendAlpha   : 4;
        FUNCTION        destBlendAlpha  : 4;
        OPERATION       blendOpAlpha    : 4;

        DESC() :
            blendEnable(false),
            srcBlend(ONE),
            destBlend(ONE),
            blendOp(ADD),
            srcBlendAlpha(ONE),
            destBlendAlpha(ONE),
            blendOpAlpha(ADD)
        {}

        bool operator == (const DESC& rhs) const
        {
            return blendEnable       == rhs.blendEnable
                   && srcBlend       == rhs.srcBlend
                   && destBlend      == rhs.destBlend
                   && blendOp        == rhs.blendOp
                   && srcBlendAlpha  == rhs.srcBlendAlpha
                   && destBlendAlpha == rhs.destBlendAlpha
                   && blendOpAlpha   == rhs.blendOpAlpha;
        }
    };

    /** Shrinken description of the blend state */
    struct DESC_SIMPLE
    {
        unsigned int    blendEnable : 8;
        FUNCTION        srcBlend    : 4;
        FUNCTION        destBlend   : 4;
        OPERATION       blendOp     : 8;

        DESC_SIMPLE() :
            blendEnable(false),
            srcBlend(ONE),
            destBlend(ONE),
            blendOp(ADD)
        {}

        bool operator == (const DESC& rhs) const
        {
            return blendEnable  == rhs.blendEnable
                   && srcBlend  == rhs.srcBlend
                   && destBlend == rhs.destBlend
                   && blendOp   == rhs.blendOp;
        }
    };

public:
    // Override State
    virtual TYPE SGL_DLLCALL Type() const { return BLEND_STATE; }

    /** Get description of the state */
    virtual const DESC& SGL_DLLCALL Desc() const = 0;

    /** Setup state to the device */
    virtual void SGL_DLLCALL Bind() const = 0;

    virtual ~BlendState() {}
};

/** Create extended description from shrinken one */
inline BlendState::DESC Extend(const BlendState::DESC_SIMPLE& desc_)
{
    BlendState::DESC desc;

    desc.blendEnable    = desc_.blendEnable;
    desc.srcBlend       = desc_.srcBlend;
    desc.srcBlendAlpha  = desc_.srcBlend;
    desc.destBlend      = desc_.destBlend;
    desc.destBlendAlpha = desc_.destBlend;
    desc.blendOp        = desc_.blendOp;
    desc.blendOpAlpha   = desc_.blendOp;

    return desc;
}
} // namepsace sgl

#endif // SIMPLE_GL_BLEND_STATE_H
