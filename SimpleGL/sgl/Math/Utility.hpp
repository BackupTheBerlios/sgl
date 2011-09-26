#ifndef GRAPHICS_MATH_UTILITY_HPP
#define GRAPHICS_MATH_UTILITY_HPP

#include "../Config.h"
#include <cmath>

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
#   define SGL_BEGIN_MATH_NAMESPACE namespace sgl { namespace math {
#   define SGL_END_MATH_NAMESPACE   } }
#else
#   define SGL_BEGIN_MATH_NAMESPACE namespace math {
#   define SGL_END_MATH_NAMESPACE   }
#endif

SGL_BEGIN_MATH_NAMESPACE

const float PI      = 3.1415926535897932384626433832795f;
const float PI2     = 2.0f * PI;
const float HALF_PI = 0.5f * PI;

const float DEG_TO_RAD = PI / 180.0f;
const float RAD_TO_DEG = 180.0f / PI;

inline float deg_to_rad(float angle) { return DEG_TO_RAD * angle; }
inline float rad_to_deg(float angle) { return RAD_TO_DEG * angle; }

template<typename T>
inline T abs(T val)
{
    return std::abs(val);
}

template<>
inline float abs(float val)
{
    return std::fabs(val);
}

template<>
inline double abs(double val)
{
    return std::fabs(val);
}

template<>
inline long double abs(long double val)
{
    return std::fabs(val);
}

SGL_END_MATH_NAMESPACE

#endif // GRAPHICS_MATH_UTILITY_HPP
