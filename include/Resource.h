#ifndef SIMPLE_GL_RESOURCE_H
#define SIMPLE_GL_RESOURCE_H

#include "Types.h"
#include "Utility/Error.h"
#include "Utility/Referenced.h"

namespace sgl {

/// Viewport or texture region
struct rectangle
{
    unsigned    x;
    unsigned    y;
    unsigned    width;
    unsigned    height;

    rectangle() {}
    rectangle(unsigned _x, unsigned _y, unsigned _w, unsigned _h) :
        x(_x), y(_y), width(_w), height(_h) {}
    rectangle(const rectangle& vp) :
        x(vp.x), y(vp.y), width(vp.width), height(vp.height) {}

    bool operator == (const rectangle& rhs) const
    {
        return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
    }

    bool operator != (const rectangle& rhs) const
    {
        return x != rhs.x || y != rhs.y || width != rhs.width || height != rhs.height;
    }
};

/** Interface for the resource object. Resources are all objcets
 * that create any objects at the device side.
 */
class Resource :
	public Referenced
{
public:
    virtual SGL_DLLCALL ~Resource() {}
};

template<typename Interface>
class ResourceImpl :
    public ReferencedImpl<Interface>
{
public:
	ResourceImpl()
    {}

    template<typename T0>
	ResourceImpl(T0 _0) :
        ReferencedImpl<Interface>(_0)
    {}

    template<typename T0, typename T1>
	ResourceImpl(T0 _0, T1 _1) :
        ReferencedImpl<Interface>(_0, _1)
    {}

    virtual SGL_DLLCALL ~ResourceImpl() {}
};

#undef SGL_DEFINE_TRAITS

}

#endif // SIMPLE_GL_RESOURCE_H
