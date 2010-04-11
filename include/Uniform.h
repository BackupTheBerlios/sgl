#ifndef SIMPLE_GL_UNIFORM_H
#define SIMPLE_GL_UNIFORM_H

#include "Math/Matrix.hpp"
#include "Texture1D.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "TextureCube.h"

namespace sgl {

// forward decl
class Program;

/** Uniform with the undefined type */
class AbstractUniform :
    public WeakReferenced
{
public:
    enum TYPE
    {
        INT,
        VEC2I,
        VEC3I,
        VEC4I,
        FLOAT,
        VEC2F,
        VEC3F,
        VEC4F,
        MAT2x2F,
        MAT2x3F,
        MAT2x4F,
        MAT3x2F,
        MAT3x3F,
        MAT3x4F,
        MAT4x2F,
        MAT4x3F,
        MAT4x4F,
        SAMPLER_1D,
        SAMPLER_2D,
        SAMPLER_3D,
        SAMPLER_CUBE
    };

public:
    /** Get type of the uniform. */
    virtual TYPE SGL_DLLCALL Type() const = 0;

    /** Get name of the uniform. */
    virtual const char* SGL_DLLCALL Name() const = 0;

    /** Get program of the uniform. */
    virtual const Program* SGL_DLLCALL MasterProgram() const = 0;

    virtual ~AbstractUniform() {}
};

/** Scalar shader uniform  */
template<typename T>
class Uniform :
	public AbstractUniform
{
public:
    /** Set value of the uniform. Master program must be binded. */
    virtual void SGL_DLLCALL Set(const T& value) = 0;

    /** Set array of the uniforms. Master program must be binded.
     * Doesn't perform any checks.
     * @param values - array with uniform values.
     * @param count - number of the values to setup.
     */
    virtual void SGL_DLLCALL Set( const T*       values,
                                  unsigned int   count ) = 0;

    /** Get value of the uniform. if there is arrya of the uniforms - retrieve first.
     * Master program must be binded.
     */
    virtual T SGL_DLLCALL Value() const = 0;

    /** Get number of values in the uniform array. */
    virtual unsigned int SGL_DLLCALL Size() const = 0;

    /** Retrieve stored uniforms. Master program must be binded.
     * Doesn't perform any checks.
     * @param values [out] - array for the values, array must have enouth size to store values
     */
    virtual void SGL_DLLCALL QueryValues(T* values) const = 0;

    virtual ~Uniform() {}
};

// typedefs
typedef Uniform<int>              UniformI;
typedef Uniform<math::Vector2i>   Uniform2I;
typedef Uniform<math::Vector3i>   Uniform3I;
typedef Uniform<math::Vector4i>   Uniform4I;

typedef Uniform<float>            UniformF;
typedef Uniform<math::Vector2f>   Uniform2F;
typedef Uniform<math::Vector3f>   Uniform3F;
typedef Uniform<math::Vector4f>   Uniform4F;

typedef Uniform<math::Matrix2x2f> Uniform2x2F;
typedef Uniform<math::Matrix2x3f> Uniform2x3F;
typedef Uniform<math::Matrix2x4f> Uniform2x4F;
typedef Uniform<math::Matrix3x2f> Uniform3x2F;
typedef Uniform<math::Matrix3x3f> Uniform3x3F;
typedef Uniform<math::Matrix3x4f> Uniform3x4F;
typedef Uniform<math::Matrix4x2f> Uniform4x2F;
typedef Uniform<math::Matrix4x3f> Uniform4x3F;
typedef Uniform<math::Matrix4x4f> Uniform4x4F;

/** Sampler shader uniform used to setup texture samplers to the shader.
 * It setups texture to the specified stage and passes stage to the corresponding uniform.
 * You also can use Uniform<int> to setup sampler stage and explicitly setup
 * texture to that stage. 
 */
template<typename T>
class SamplerUniform :
	public AbstractUniform
{
public:
    /** Setup texture to the uniform.
     * @param stage - stage where to set the texture
     * @param texture - texture to setup. Can be 0.
     */
    virtual void SGL_DLLCALL Set(unsigned int stage, const T* texture) = 0;

    /** Get stage value of the uniform. */
    virtual unsigned int SGL_DLLCALL Value() const = 0;

    virtual ~SamplerUniform() {}
};

typedef SamplerUniform<Texture1D>       SamplerUniform1D;
typedef SamplerUniform<Texture2D>       SamplerUniform2D;
typedef SamplerUniform<Texture3D>       SamplerUniform3D;
typedef SamplerUniform<TextureCube>     SamplerUniformCube;

} // namespace sgl

#endif // SIMPLE_GL_UNIFORM_H
