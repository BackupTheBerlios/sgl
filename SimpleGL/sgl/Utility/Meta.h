#ifndef SIMPLE_GL_UTILITY_META_H
#define SIMPLE_GL_UTILITY_META_H

#include "../Config.h"

namespace sgl {

/* Copied from boost */
template <bool B, class T = void>
struct enable_if_c 
{
    typedef T tag;
};

template <class T>
struct enable_if_c<false, T> {};

template <class Cond, class T = void>
struct enable_if : public enable_if_c<Cond::value, T> {};

template<typename T>
inline void do_nothing(T) {}

} // namespace sgl

#endif // SIMPLE_GL_UTILITY_META_H
