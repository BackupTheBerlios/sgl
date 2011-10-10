#ifndef SIMPLE_GL_CONFIG_H
#define SIMPLE_GL_CONFIG_H

#ifndef __GNUC__
	#ifndef __INTEL_COMPILER
		#define SGL_DLLCALL __stdcall
	#else
		#define SGL_DLLCALL
	#endif
	#ifdef SGL_EXPORT
		#define SGL_DLLEXPORT __declspec(dllexport)
	#else
		#define SGL_DLLEXPORT __declspec(dllimport)
	#endif
#else
	#define SGL_DLLEXPORT
	#define SGL_DLLCALL
#endif

#cmakedefine SIMPLE_GL_ANDROID
#cmakedefine SIMPLE_GL_USE_SDL_IMAGE
#cmakedefine SIMPLE_GL_USE_DEVIL
#cmakedefine SIMPLE_GL_USE_SSE2
#cmakedefine SIMPLE_GL_USE_SSE3
#cmakedefine SIMPLE_GL_USE_SSE4
#cmakedefine SIMPLEGL_MATH_IN_SGL_NAMESPACE

#if defined(SIMPLE_GL_USE_SSE2) || defined(SIMPLE_GL_USE_SSE3) || defined (SIMPLE_GL_USE_SSE4)
#define SIMPLE_GL_USE_SSE
#endif

#endif // SIMPLE_GL_CONFIG_H
