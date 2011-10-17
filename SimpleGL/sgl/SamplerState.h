#ifndef SIMPLE_GL_SAMPLER_STATE_H
#define SIMPLE_GL_SAMPLER_STATE_H

#include "State.h"

namespace sgl {

/** SamplerState state.
 */
class SamplerState :
    public State
{
public:
    /** clamping mode */
    enum WRAPPING
    {
        REPEAT,
        CLAMP_TO_EDGE,
        __NUMBER_OF_CLAMP_MODES__
    };

    /** filter modes */
    enum FILTER
    {
        NONE,       /// Actual for mip filtering, NONE means no mipmaping
        NEAREST,
        LINEAR,
        __NUMBER_OF_FILTERS__
    };

    /** Sampler state */
    struct DESC
    {
        FILTER              filter[3];      /// min, mag and mip filters
        WRAPPING            wrapping[3];    /// S,T,R texture coordinate wrapping
        unsigned int        maxAnisotropy;  /// Maximum anisotropy. 1 - no anisotropy. If greater then filter is ignored.
        float               minLod;
        float               maxLod;
        float               lodBias; /// Let the device calculates that mip level for the specified texture fetch should be <desiredLod>
                                     /// Then lod is calculated using formula clamp(minLod, maxLod, loadBias + desiredLod)

        DESC() :
            maxAnisotropy(0),
            minLod(0.0f),
            maxLod(1024.0f),
            lodBias(0.0f)
        {
            filter[0] = NEAREST;
            filter[1] = NEAREST;
            filter[2] = NEAREST;

            wrapping[0] = REPEAT;
            wrapping[1] = REPEAT;
            wrapping[2] = REPEAT;
        }
    };

public:
    // Override State
    virtual TYPE SGL_DLLCALL Type() const { return SAMPLER_STATE; }

    /** Get description of the state */
    virtual const DESC& SGL_DLLCALL Desc() const = 0;

    /** Bind state to the stage */
    // virtual void SGL_DLLCALL Bind(unsigned stage) const = 0;

    virtual ~SamplerState() {}
};

} // namepsace sgl

#endif // SIMPLE_GL_SAMPLER_STATE_H
