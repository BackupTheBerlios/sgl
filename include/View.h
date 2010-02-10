#ifndef SIMPLE_GL_VIEW_H
#define SIMPLE_GL_VIEW_H

#include "Resource.h"

namespace sgl {

/** View on some GPU resource */
class View :
    public Referenced
{
public:
    /** Get resource of this view */
    virtual Resource* SGL_DLLCALL Source() = 0;

    virtual ~View() {} 
};

}

#endif // SIMPLE_GL_VIEW_H