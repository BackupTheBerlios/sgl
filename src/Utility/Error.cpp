#include "Utility/Error.h"

using namespace sgl;

static ErrorHandler*	errorHandler = 0;
static SGL_HRESULT		result = SGL_OK;
static const char*		msg;

extern "C" SGL_DLLEXPORT SGL_HRESULT SGL_DLLCALL sglGetLastError()
{
	SGL_HRESULT tmp = result;
	result = SGL_OK;
	msg = 0;
	return tmp;
}

extern "C" SGL_DLLEXPORT const char* SGL_DLLCALL sglGetErrorMsg()
{
	return msg;
}

extern "C" SGL_DLLEXPORT void SGL_DLLCALL sglSetError(SGL_HRESULT _type, const char* _msg)
{
	result = _type;
	msg = _msg;
	if (errorHandler) {
		errorHandler->HandleError(_type, _msg);
	}
}

extern "C" SGL_DLLEXPORT void SGL_DLLCALL sglSetErrorHandler(ErrorHandler* _handler)
{
	errorHandler = _handler;
}

extern "C" SGL_DLLEXPORT ErrorHandler* SGL_DLLCALL sglGetErrorHandler()
{
	return errorHandler;
}
