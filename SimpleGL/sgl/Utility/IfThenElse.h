#ifndef SIMPLE_GL_IF_THEN_ELSE_H
#define SIMPLE_GL_IF_THEN_ELSE_H

namespace sgl {

// May be replace with some 3rd-party template 
template<bool B, typename T1, typename T2>
struct if_then_else
{
	typedef T1 type;
};

template<typename T1, typename T2>
struct if_then_else<false, T1, T2>
{
	typedef T2 type;
};

} // namespace sgl

#endif // SIMPLE_GL_IF_THEN_ELSE_H