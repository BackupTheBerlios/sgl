#ifndef SIMPLE_GL_TYPES_H
#define SIMPLE_GL_TYPES_H

// sgl - simple graphics library
namespace sgl {

/// Device type according to the library used for rendering
enum DEVICE_VERSION
{
    DV_OPENGL_1_3,
    DV_OPENGL_1_4,
    DV_OPENGL_1_5,
    DV_OPENGL_2_0,
    DV_OPENGL_2_1,
    DV_OPENGL_3_0,
    DV_OPENGL_3_1,
    DV_OPENGL_3_2
};

/** Type of the scalar value used for texture formats or
 * VertexBuffer data specification.
 */
enum SCALAR_TYPE
{
    UNKNOWN,
    BIT,
    BOOL,
    BYTE,
    UBYTE,
    SHORT,
    USHORT,
    INT,
    UINT,
    FLOAT,
    DOUBLE,
    __NUMBER_OF_SCALAR_TYPES__
};

/** Traits containing information about scalar types for compile-time checks */
template<SCALAR_TYPE type>
struct ScalarTypetTraits
{
    /** Size of the type in bits */
    static inline unsigned SizeInBits();
};

/** Structure containing information about scalar type */
struct scalar_type_desc
{
    unsigned sizeInBits; /// Size of the type in bits
};

/** Traits containing scalar type information for runtime checks. */
static const scalar_type_desc SCALAR_TYPE_TRAITS[] =
{
    {0},
    {1},
    {8},
    {8},
    {8},
    {16},
    {16},
    {32},
    {32},
    {32}
};

} // namespace sgl

#endif // SIMPLE_GL_TYPES_H
