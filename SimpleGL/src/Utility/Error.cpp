#include "Utility/Error.h"
#ifdef __ANDROID__
#   include <android/log.h>
#endif

using namespace sgl;

static ErrorHandler*    errorHandler = 0;
static SGL_HRESULT      result = SGL_OK;
static std::string      msg;

namespace sgl {

extern "C" SGL_DLLEXPORT SGL_HRESULT SGL_DLLCALL sglGetLastError()
{
	SGL_HRESULT tmp = result;
	result = SGL_OK;
	return tmp;
}

extern "C" SGL_DLLEXPORT const char* SGL_DLLCALL sglGetErrorMsg()
{
	return msg.c_str();
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

void PrintErrorHandler::HandleError(SGL_HRESULT type, const char* msg)
{
#ifdef __ANDROID__
    switch(type)
    {
    case SGLERR_INVALID_CALL:
        __android_log_print(ANDROID_LOG_ERROR, "sgl", "Invalid call: %s\n", msg);
        break;

    case SGLERR_OUT_OF_MEMORY:
        __android_log_print(ANDROID_LOG_ERROR, "sgl", "Out of memory: %s\n", msg);
        break;

    case SGLERR_FILE_NOT_FOUND:
        __android_log_print(ANDROID_LOG_ERROR, "sgl", "File not found: %s\n", msg);
        break;

    case SGLERR_UNSUPPORTED:
        __android_log_print(ANDROID_LOG_ERROR, "sgl", "Unsupported function: %s\n", msg);
        break;

    default:
        __android_log_print(ANDROID_LOG_ERROR, "sgl", "Unknown error: %s\n", msg);
        break;
    }
#else
    switch(type)
    {
    case SGLERR_INVALID_CALL:
        std::cerr << "Invalid call: " << msg << std::endl;
        break;

    case SGLERR_OUT_OF_MEMORY:
        std::cerr << "Out of memory: " << msg << std::endl;
        break;

    case SGLERR_FILE_NOT_FOUND:
        std::cerr << "File not found: " << msg << std::endl;
        break;

    case SGLERR_UNSUPPORTED:
        std::cerr << "Unsupported function: " << msg << std::endl;
        break;

    default:
        std::cerr << "Unknown error: " << msg << std::endl;
        break;
    }
#endif
}

} // namespace sgl
