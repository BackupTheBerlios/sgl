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
        FILL_MODE   fillMode      : 4;
        CULL_MODE   cullMode      : 4;

        DESC() :
            fillMode(SOLID),
            cullMode(BACK)
        {}
    };

public:
    // Override State
    virtual TYPE SGL_DLLCALL Type() const { return RASTERIZER_STATE; }

    /** Get description of the state */
    virtual const DESC& SGL_DLLCALL Desc() const = 0;

    /** Setup state to the device */
    virtual void SGL_DLLCALL Bind() const = 0;

    virtual ~RasterizerState() {}
};

} // namepsace sgl

#endif // SIMPLE_GL_RASTERIZER_STATE_H
