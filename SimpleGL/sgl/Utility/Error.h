#ifndef SIMPLE_GL_UTILITY_ERROR_H
#define SIMPLE_GL_UTILITY_ERROR_H

#include "../Config.h"
#include "Referenced.h"
#include <iostream>

namespace sgl {

/** Used to determine the result or status of the executed operation */
enum SGL_HRESULT
{
	SGL_OK,					/// operation succeeded
	SGLERR_INVALID_CALL,	/// function call with invalid arguments
	SGLERR_OUT_OF_MEMORY,	/// couldn't allocate memory
	SGLERR_FILE_NOT_FOUND,	/// couldn't find specified file
	SGLERR_UNSUPPORTED,		/// operation not supported by the hardware
    SGLERR_UANAILABLE,      /// resource is unavailable at the moment
	SGLERR_UNKNOWN			/// unknow error
};

/** Handler to handle simple gl errors. For example log it into the file */
class SGL_DLLEXPORT ErrorHandler
{
public:
	/** Handle sgl error
	 * @param result - type of the error
	 * @param msg - error message
	 */
    virtual void HandleError(SGL_HRESULT result, const char* msg) = 0;

	virtual ~ErrorHandler() {}
};

} // namespace sgl

/** Get last occured error */
extern "C" SGL_DLLEXPORT sgl::SGL_HRESULT SGL_DLLCALL sglGetLastError();

/** Get last error message */
extern "C" SGL_DLLEXPORT const char* SGL_DLLCALL sglGetErrorMsg();

/** Set error handler to the sgl */
extern "C" SGL_DLLEXPORT void SGL_DLLCALL sglSetErrorHandler(sgl::ErrorHandler* _handler);

/** Get error handler of the sgl */
extern "C" SGL_DLLEXPORT sgl::ErrorHandler* SGL_DLLCALL sglGetErrorHandler();

/* Set sgl error */
extern "C" SGL_DLLEXPORT void SGL_DLLCALL sglSetError(sgl::SGL_HRESULT _type, const char* _msg);

namespace sgl {

/** Error handler that logs errors to the platform specific log (cerr, android log, ...). */
class SGL_DLLEXPORT PrintErrorHandler :
	public ErrorHandler
{
public:
    // Override ErrorHandler
    void HandleError(SGL_HRESULT type, const char* msg);

    virtual ~PrintErrorHandler() {}
};

/** Error occurs when calling function with invalid arguments */
inline SGL_HRESULT EInvalidCall()
{
	sglSetError(SGLERR_INVALID_CALL, "Invalid function call");
	return SGLERR_INVALID_CALL;
}

/** Error occurs when calling function with invalid arguments */
inline SGL_HRESULT EInvalidCall(const char* msg)
{
	sglSetError(SGLERR_INVALID_CALL, msg);
	return SGLERR_INVALID_CALL;
}

/** Error occurs when we are out of memory */
inline SGL_HRESULT EOutOfMemory()
{
	sglSetError(SGLERR_OUT_OF_MEMORY, "Out of memory");
	return SGLERR_OUT_OF_MEMORY;
}

/** Error occurs when we are out of memory */
inline SGL_HRESULT EOutOfMemory(const char* msg)
{
	sglSetError(SGLERR_OUT_OF_MEMORY, msg);
	return SGLERR_OUT_OF_MEMORY;
}

/** Error occurs when we are trying to open unexisted file for reading */
inline SGL_HRESULT EFileNotFound()
{
	sglSetError(SGLERR_FILE_NOT_FOUND, "File not found");
	return SGLERR_FILE_NOT_FOUND;
}

/** Error occurs when we are trying to open unexisted file for reading */
inline SGL_HRESULT EFileNotFound(const char* msg)
{
	sglSetError(SGLERR_FILE_NOT_FOUND, msg);
	return SGLERR_FILE_NOT_FOUND;
}

/** Error occurs when we are trying to perform operation unsupported by the hardware */
inline SGL_HRESULT EUnsupported()
{
	sglSetError(SGLERR_UNSUPPORTED, "Operation unsupported by the hardware");
	return SGLERR_UNSUPPORTED;
}

/** Error occurs when we are trying to perform operation unsupported by the hardware */
inline SGL_HRESULT EUnsupported(const char* msg)
{
	sglSetError(SGLERR_UNSUPPORTED, msg);
	return SGLERR_UNSUPPORTED;
}

/** Mystic error */
inline SGL_HRESULT EUnknown()
{
	sglSetError(SGLERR_UNKNOWN, "Unknown error");
	return SGLERR_UNKNOWN;
}

/** Mystic error */
inline SGL_HRESULT EUnknown(const char* msg)
{
	sglSetError(SGLERR_UNKNOWN, msg);
	return SGLERR_UNKNOWN;
}

} // namespace sgl

#endif // SIMPLE_GL_UTILITY_ERROR_H
