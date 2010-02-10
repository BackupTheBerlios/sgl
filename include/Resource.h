#ifndef SIMPLE_GL_RESOURCE_H
#define SIMPLE_GL_RESOURCE_H

#include "Utility/Error.h"
#include "Utility/Referenced.h"

namespace sgl {

/** Type of the scalar value used for texture formats or
 * VertexBuffer data specification.
 */
enum SCALAR_TYPE
{
    UNKNOWN,
    BIT,
    BYTE,
	UBYTE,
    SHORT,
	USHORT,
    INT,
    UINT,
	FLOAT,
    __NUMBER_OF_SCALAR_TYPES__
};

/** Traits containing information about scalar types for compile-time checks */
template<SCALAR_TYPE type>
struct ScalarTypetTraits
{
    /** Size of the type in bits */
    static inline size_t SizeInBits();
};

/** Structure containing information about scalar type */
struct scalar_type_desc
{
    size_t sizeInBits; /// Size of the type in bits
};

/** Traits containing scalar type information for runtime checks. */
static const scalar_type_desc SCALAR_TYPE_TRAITS[] =
{
    {0},
    {1},
    {8},
    {8},
    {16},
	{16},
    {32},
    {32},
    {32}
};

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

#define SGL_DEFINE_TRAITS(type, size) \
    template<> inline size_t ScalarTypetTraits<type>::SizeInBits() { return size; } \

SGL_DEFINE_TRAITS(BIT,    1)
SGL_DEFINE_TRAITS(BYTE,   8)
SGL_DEFINE_TRAITS(UBYTE,  8)
SGL_DEFINE_TRAITS(SHORT,  16)
SGL_DEFINE_TRAITS(USHORT, 16)
SGL_DEFINE_TRAITS(INT,    32)
SGL_DEFINE_TRAITS(UINT,   32)
SGL_DEFINE_TRAITS(FLOAT,  32)

#undef SGL_DEFINE_TRAITS

}

#endif // SIMPLE_GL_RESOURCE_H
