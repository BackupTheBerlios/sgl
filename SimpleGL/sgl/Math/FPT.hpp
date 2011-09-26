#ifndef SIMPLE_GL_MATH_FPT_HPP
#define SIMPLE_GL_MATH_FPT_HPP

#include "Utility.hpp"
#include <limits>

SGL_BEGIN_MATH_NAMESPACE

const float EPS_1f  = 1e-1f;
const float EPS_2f  = 1e-2f;
const float EPS_3f  = 1e-3f;
const float EPS_4f  = 1e-4f;
const float EPS_5f  = 1e-5f;

const double EPS_1d  = 1e-1;
const double EPS_2d  = 1e-2;
const double EPS_3d  = 1e-3;
const double EPS_4d  = 1e-4;
const double EPS_5d  = 1e-5;
const double EPS_6d  = 1e-6;
const double EPS_7d  = 1e-7;
const double EPS_8d  = 1e-8;
const double EPS_9d  = 1e-9;

/** Compare absolute difference of floating point value with zero with tolerance e */
inline bool fpt_small(float x, 
                      float eps = std::numeric_limits<float>::epsilon())
{
    return abs(x) <= eps;
}

/** Compare absolute difference of floating point values with tolerance e */
inline bool fpt_close_abs(float x,
                          float y,
                          float eps = std::numeric_limits<float>::epsilon())
{
    return abs(x - y) <= eps;
}

/** Compare relative difference of floating point values with tolerance e */
inline bool fpt_close_rel(float x,
                          float y,
                          float eps = std::numeric_limits<float>::epsilon())
{
    return abs(x - y) <= eps * std::max(std::max(abs(x), abs(y)), 1.0f);
}

/** Compare both relative and absolute difference of floating point values with tolerance e */
inline bool fpt_close(float x,
                      float y,
                      float epsAbs = std::numeric_limits<float>::epsilon(),
                      float epsRel = std::numeric_limits<float>::epsilon())
{
    return abs(x - y) <= std::max(epsAbs, epsRel * std::max(abs(x), abs(y)));
}

/** Compare absolute difference of floating point value with zero with tolerance e */
inline bool fpt_small(double x, 
                      double eps = std::numeric_limits<double>::epsilon())
{
    return abs(x) <= eps;
}

/** Compare absolute difference of floating point values with tolerance e */
inline bool fpt_close_abs(double x,
                          double y,
                          double eps = std::numeric_limits<double>::epsilon())
{
    return abs(x - y) <= eps;
}

/** Compare relative difference of floating point values with tolerance e */
inline bool fpt_close_rel(double x,
                          double y,
                          double eps = std::numeric_limits<double>::epsilon())
{
    return abs(x - y) <= eps * std::max(std::max(abs(x), abs(y)), 1.0);
}

/** Compare both relative and absolute difference of floating point values with tolerance e */
inline bool fpt_close(double x,
                      double y,
                      double epsAbs = std::numeric_limits<double>::epsilon(),
                      double epsRel = std::numeric_limits<double>::epsilon())
{
    return abs(x - y) <= std::max(epsAbs, epsRel * std::max(abs(x), abs(y)));
}

SGL_END_MATH_NAMESPACE

#endif // SIMPLE_GL_MATH_FPT_HPP
