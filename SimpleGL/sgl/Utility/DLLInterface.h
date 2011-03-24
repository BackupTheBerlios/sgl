#ifndef SIMPLE_GL_DLL_INTERFACE_H
#define SIMPLE_GL_DLL_INTERFACE_H

#include "../Config.h"

#ifdef SIMPLE_GL_USE_SSE
#include "Aligned.h"
#endif // SIMPLE_GL_USE_SSE

namespace sgl {

/**
 * Interface for any classes within dll to simplify
 * the creation of the binary-compatible dll
 */
class DLLInterface
{
public:
	/** Handles the destruction of this class. This is essentially a destructor
	*  that works across DLL boundaries.
	*/
	virtual void SGL_DLLCALL Destroy() = 0;

    void operator delete(void* ptr)
    {
        if (ptr)
        {
            DLLInterface* pInterface = static_cast<DLLInterface*>(ptr);
            pInterface->Destroy();
        }
    }

    virtual ~DLLInterface() {}
};

template<typename Interface>
class DLLImpl :
	public Interface
{
protected:
	DLLImpl() {}

    template<typename T0>
	DLLImpl(T0 _0) :
        Interface(_0)
    {}

    template<typename T0, typename T1>
	DLLImpl(T0 _0, T1 _1) :
        Interface(_0, _1)
    {}

	/** This allows the implementation to simply define a destructor.  It
	 *  will automatically be called at the right time.
	 */
	virtual ~DLLImpl() {}

public:
#ifdef SIMPLE_GL_USE_SSE
	void* operator new(size_t size)
    {
	    void* pData = align_alloc(size, 0x10);
        if (!pData) {
            throw std::bad_alloc();
        }

        return pData;
    }
#endif // SIMPLE_GL_USE_SSE

    /** From this point will be used default delete */
    void operator delete(void* p)
    {
    #ifdef SIMPLE_GL_USE_SSE
        if (p) {
            align_free(p);
        }
    #else // SIMPLE_GL_USE_SSE
        ::operator delete(p);
    #endif // SIMPLE_GL_USE_SSE
    }

	/**
 	 * Implementation of DLLInterface's destroy method. This function will
	 * make sure this object gets deleted in a polymorphic manner. You
	 * should NOT reimplement this method.
	 */
	virtual void SGL_DLLCALL Destroy()
	{
        delete this;
	}
};

} // namespace sgl

#endif // SIMPLE_GL_DLL_INTERFACE_H
