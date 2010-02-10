#ifndef SIMPLE_GL_RASTERIZER_STATE_H
#define SIMPLE_GL_RASTERIZER_STATE_H

#include "Math/Vector.hpp"
#include "State.h"

namespace sgl {

/** Rasterizer state.
 */
class RasterizerState :
    public State
{
public:
    /** Polygon fill mode */
    enum FILL_MODE
    {
        SOLID,
        WIREFRAME
    };

    /** Culling mode */
    enum CULL_MODE
    {
        NONE,           /// cull none
	    FRONT,			/// cull clockwise polygons
	    BACK,			/// cull counter-clockwise polygons
	    FRONT_AND_BACK	/// cull all
    };

    /** Description of the state */
    struct DESC 
    {
        FILL_MODE   fillMode      : 3;
        CULL_MODE   cullMode      : 4;
        bool        scissorEnable : 1;

        DESC() :
            fillMode(SOLID),
            cullMode(BACK),
            scissorEnable(false)
        {}
    };

public:
    // Override State
    virtual TYPE SGL_DLLCALL Type() const { return RASTERIZER_STATE; }

    /** Get scissor rectangle used for the scissor test */
    virtual const math::Vector4i& SGL_DLLCALL ScissorRectangle() const = 0;
    
    /** Set scissor rectangle for the scissor test */
    virtual void SGL_DLLCALL SetScissorRectangle(const math::Vector4i& rect) = 0;

    /** Get description of the state */
    virtual const DESC& SGL_DLLCALL Desc() const = 0;

    /** Setup state to the device */
    virtual void SGL_DLLCALL Bind() const = 0;

    virtual ~RasterizerState() {}
};

} // namepsace sgl

#endif // SIMPLE_GL_RASTERIZER_STATE_H
