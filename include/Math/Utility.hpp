#ifndef GRAPHICS_MATH_UTILITY_HPP
#define GRAPHICS_MATH_UTILITY_HPP

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

const float PI      = 3.1415926535897932384626433832795f;
const float PI2     = 2.0f * PI;
const float HALF_PI = 0.5f * PI;

const float EPS_3f  = 1e-3f;
const float EPS_4f  = 1e-4f;
const float EPS_5f  = 1e-5f;
const float EPS_6f  = 1e-6f;
const float EPS_7f  = 1e-7f;
const float EPSf    = 1e-7f;

const float DEG_TO_RAD = PI / 180.0f;
const float RAD_TO_DEG = 180.0f / PI;

inline float deg_to_rad(float angle) { return DEG_TO_RAD * angle; }
inline float rad_to_deg(float angle) { return RAD_TO_DEG * angle; }

} // namesapce math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // GRAPHICS_MATH_UTILITY_HPP
