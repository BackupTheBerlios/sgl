#ifndef SIMPLE_GL_DEPTH_STENCIL_STATE_H
#define SIMPLE_GL_DEPTH_STENCIL_STATE_H

#include "State.h"

namespace sgl {

class DepthStencilState :
    public State
{
public:
	/** Stencil operation */
	enum OPERATION
	{
		KEEP,
		ZERO,
		REPLACE,
		INCR,
		DECR,
		INCR_WRAP,
		DECR_WRAP,
		INVERT
	};

	/** Test function */
	enum COMPARISON_FUNCTION
	{
		NEVER,
		LESS,
		LEQUAL,
		GREATER,
		GEQUAL,
		EQUAL,
		NOTEQUAL,
		ALWAYS
	};

    /** Operation for stencil */
    struct STENCIL_OPERATION
    {
        OPERATION           stencilFailOp       : 4;
        OPERATION           stencilDepthFailOp  : 4;
        OPERATION           stencilPassOp       : 4;
        COMPARISON_FUNCTION stencilFunc         : 4;

        STENCIL_OPERATION() :
            stencilFailOp(KEEP),
            stencilDepthFailOp(KEEP),
            stencilPassOp(KEEP),
            stencilFunc(NEVER)
        {}
    };

    /** Description of the depth stencil state */
    struct DESC 
    {
        unsigned int            depthEnable         : 1;
        unsigned int            depthWriteMask      : 1;     // true - write, false - do not write
        COMPARISON_FUNCTION     depthFunc           : 8; 
        unsigned int            stencilEnable       : 1;
        unsigned int            stencilReadMask     : 8;
        unsigned int            stencilWriteMask    : 8;
        STENCIL_OPERATION       frontFaceOp;
        STENCIL_OPERATION       backFaceOp;

        DESC() :
            depthEnable(true),
            depthWriteMask(true),
            depthFunc(LEQUAL),
            stencilEnable(false),
            stencilReadMask(0),
            stencilWriteMask(0)
        {}
    };

public:
    // Override state
    virtual TYPE SGL_DLLCALL Type() const { return DEPTH_STENCIL_STATE; }

    /** Get description of the state */
    virtual const DESC& SGL_DLLCALL Desc() const = 0;

    /** Setup state to the device */
    virtual void SGL_DLLCALL Bind() const = 0;

    virtual ~DepthStencilState() {}
};

} // namepsace sgl

#endif // SIMPLE_GL_DEPTH_STENCIL_STATE_H
