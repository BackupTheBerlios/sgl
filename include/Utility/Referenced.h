#ifndef SIMPLE_GL_REFERENCED_H
#define SIMPLE_GL_REFERENCED_H

#include "DLLInterface.h"
#include <algorithm>
#include <cassert>

namespace sgl {

struct ref_counter;

/** Interface for reference counted objects which allows only
 * weak_ptr and scoped_ptr.
 */
class WeakReferenced :
    public DLLInterface
{
public:
    /** Ger ref counter for weak_ptr */
    virtual ref_counter* SGL_DLLCALL get_ref_counter() const = 0;

    virtual ~WeakReferenced() {}
};

/** Interface for reference counted objects */
class Referenced :
    public WeakReferenced
{
public:
    virtual ~Referenced() {}
};

/** Counter for ref and weak references */
struct ref_counter
{
    inline ref_counter( WeakReferenced* _ptr,
                        int             _refCount  = 0,
                        int             _weakCount = 0 ) :
        ptr(_ptr),
        refCount(_refCount),
        weakCount(_weakCount) {}

    inline void add_ref() {
        ++refCount;
    }

    inline void remove_ref()
    {
        if ( --refCount == 0 )
        {
            ptr->Destroy();
            ptr = 0;
            if (weakCount == 0) {
                Destroy();
            }
        }
    }

    inline int ref_count() {
        return refCount;
    }

    inline void add_weak() {
        ++weakCount;
    }

    inline void remove_weak()
    {
        if ( --weakCount == 0 && !ptr ) {
            Destroy();
        }
    }

    inline int weak_count() {
        return weakCount;
    }

    template<typename T>
    inline T* get() {
        return static_cast<T*>(ptr);
    }

    /* Destroys ref_counter inside DLL */
    void SGL_DLLCALL Destroy();

public:
    WeakReferenced* ptr;
    int             refCount;
    int             weakCount;
};

/* Implemenation of the reference counting objects */
template<typename Interface>
class ReferencedImpl :
    public DLLImpl<Interface>
{
public:
    typedef DLLImpl<Interface>          base_type;
    typedef ReferencedImpl<Interface>   this_type;

private:
    // noncopyable
    ReferencedImpl(const this_type& /*impl*/);
    ReferencedImpl& operator = (const this_type& /*rhs*/);

public:
    ReferencedImpl() :
        refCounter(0)
    {}

    template<typename T0>
    ReferencedImpl(T0 _0) :
        DLLImpl<Interface>(_0),
        refCounter(0)
    {}

    template<typename T0, typename T1>
    ReferencedImpl(T0 _0, T1 _1) :
        DLLImpl<Interface>(_0, _1),
        refCounter(0)
    {}

    template<typename T0, typename T1, typename T2>
    ReferencedImpl(T0 _0, T1 _1, T2 _3) :
        DLLImpl<Interface>(_0, _1, _3),
        refCounter(0)
    {}

    ref_counter* SGL_DLLCALL get_ref_counter() const
    {
        if (!refCounter) {
            refCounter = new ref_counter( const_cast<this_type*>(this) );
        }
        return refCounter;
    }

    virtual ~ReferencedImpl() {}

private:
    mutable ref_counter* refCounter;
};

// Forward
template<typename T>
class weak_ptr;

/** Smart pointer for sgl classes */
template<typename T>
class scoped_ptr
{
template<typename Y> friend class scoped_ptr;
template<typename Y> friend class weak_ptr;
public:
    typedef T               value_type;
    typedef scoped_ptr<T>   this_type;

private:
    // noncopyable
    scoped_ptr(const this_type&);
    scoped_ptr& operator = (const this_type&);

public:
    inline scoped_ptr() :
        ptr(0)
    {}

    inline explicit scoped_ptr(T* _ptr) :
        ptr(_ptr)
    {}

    inline ~scoped_ptr()
    {
        if (ptr)
        {
            ref_counter* refCounter = ptr->get_ref_counter();
            ptr->Destroy();
            refCounter->ptr = 0;
            if (refCounter->weakCount == 0) {
                refCounter->Destroy();
            }
        }
    }

    // Compare objects
    template<class Y>
    inline bool operator == (const scoped_ptr<Y>& rhs) const { return ptr == rhs.ptr; }

    template<class Y>
    inline bool operator == (Y* rhs) const { return ptr == rhs; }

    template<class Y>
    inline bool operator != (const scoped_ptr<Y>& rhs) const { return ptr  != rhs.ptr ; }

    template<class Y>
    inline bool operator != (Y* rhs) const { return ptr != rhs; }

    // Set pointer to 0. Copy and swap idiom
    inline void reset() {
        this_type().swap(*this);
    }

    // Set new pointer to ref_ptr. Copy and swap idiom
    inline void reset(T* ptr) {
        this_type(ptr).swap(*this);
    }

    // Swap ref_ptr's
    inline void swap(this_type& sPtr) throw() {
        std::swap(ptr, sPtr.ptr);
    }

    // Get object
    inline T* operator -> () const { return ptr; }
    inline T& operator *() const   { return *ptr; }
    inline T* get() const          { return ptr; }

    // implicit cast to the pointer type
    inline operator T* ()             { return ptr; }
    inline operator const T* () const { return ptr; }

private:
    T* ptr;
};

/** Smart pointer with reference counting for sgl classes */
template<typename T>
class ref_ptr
{
template<typename Y> friend class ref_ptr;
template<typename Y> friend class weak_ptr;
public:
    typedef T           value_type;
    typedef ref_ptr<T>  this_type;

public:
    inline ref_ptr() :
        ptr(0)
    {}

    inline ref_ptr(const ref_ptr<T>& rPtr) :
        ptr(rPtr.ptr)
    {
        if (ptr) {
            ptr->get_ref_counter()->add_ref();
        }
    }

    template<typename Y>
    inline ref_ptr(const ref_ptr<Y>& rPtr) :
        ptr(rPtr.ptr)
    {
        if (ptr) {
            ptr->get_ref_counter()->add_ref();
        }
    }

    inline explicit ref_ptr(T* _ptr) :
        ptr(_ptr)
    {
        // make sure T can be casted to Referenced
        if (ptr)
        {
            if ( ref_counter* ref = static_cast<const Referenced*>(ptr)->get_ref_counter() ) {
                ref->add_ref();
            }
        }
    }

    inline ~ref_ptr()
    {
        if (ptr) {
            ptr->get_ref_counter()->remove_ref();
        }
    }

    // Compare objects
    template<class Y>
    inline bool operator == (const ref_ptr<Y>& rhs) const { return ptr == rhs.ptr; }

    template<class Y>
    inline bool operator == (Y* rhs) const { return ptr == rhs; }

    template<class Y>
    inline bool operator != (const ref_ptr<Y>& rhs) const { return ptr != rhs.ptr; }

    template<class Y>
    inline bool operator != (Y* rhs) const { return ptr != rhs; }

    // Copy ref_ptr. Copy and swap idiom
    inline ref_ptr<T>& operator = (const ref_ptr<T>& rPtr)
    {
        if (ptr != rPtr.ptr) {
            this_type(rPtr).swap(*this);
        }

        return *this;
    }

    // Set pointer to 0. Copy and swap idiom
    inline void reset() {
        this_type().swap(*this);
    }

    // Set new pointer to ref_ptr. Copy and swap idiom
    inline void reset(T* _ptr) {
        this_type(_ptr).swap(*this);
    }

    // Swap ref_ptr's
    inline void swap(this_type& rPtr) throw() {
        std::swap(ptr, rPtr.ptr);
    }

    // Get object
    inline T* operator -> () const { return ptr; }
    inline T& operator *() const   { return *ptr; }
    inline T* get() const          { return ptr; }

    // implicit cast to the pointer type
    inline operator T* ()             { return ptr; }
    inline operator const T* () const { return ptr; }

private:
    T* ptr;
};

/** Smart pointer with intrusive weak reference counting for sgl classes */
template<typename T>
class weak_ptr
{
public:
    typedef T           value_type;
    typedef weak_ptr<T> this_type;

public:
    inline weak_ptr() :
        refCounter(0)
    {}

    inline weak_ptr(const weak_ptr<T>& rPtr) :
        refCounter(rPtr.refCounter)
    {
        if (refCounter) {
            refCounter->add_weak();
        }
    }

    template<class Y>
    inline weak_ptr(const weak_ptr<Y>& rPtr) :
        refCounter( rPtr.refCounter.cast() )
    {
        if (refCounter) {
            refCounter->add_weak();
        }
    }

    inline explicit weak_ptr(Referenced* ptr) :
        refCounter(0)
    {
        if (ptr)
        {
            refCounter = ptr->get_ref_counter();
            refCounter->add_weak();
        }
    }

    inline explicit weak_ptr(WeakReferenced* ptr) :
        refCounter(0)
    {
        if (ptr)
        {
            refCounter = ptr->get_ref_counter();
            refCounter->add_weak();
        }
    }

    inline ~weak_ptr()
    {
        refCounter->remove_weak();
    }

    // Compare objects
    template<class Y>
    inline bool operator == (const weak_ptr<Y>& rhs) const { return refCounter == rhs.refCounter; }

    template<class Y>
    inline bool operator == (Y* rhs) const { return refCounter && refCounter->ptr == rhs; }

    template<class Y>
    inline bool operator != (const weak_ptr<Y>& rhs) const { return refCounter != rhs.refCounter; }

    template<class Y>
    inline bool operator != (Y* rhs) const { return !refCounter || refCounter->ptr != rhs; }

    // Copy weak_ptr. Copy and swap idiom
    inline weak_ptr<T>& operator = (const weak_ptr<T>& rPtr)
    {
        if (refCounter != rPtr.refCounter) {
            this_type(rPtr).swap(*this);
        }

        return *this;
    }

    // Set pointer to 0. Copy and swap idiom
    inline void reset() {
        this_type().swap(*this);
    }

    // Set new pointer to weak_ptr. Copy and swap idiom
    inline void reset(T* _ptr)
    {
        if (refCounter->ptr != _ptr) {
            this_type(_ptr).swap(*this);
        }
    }

    // Swap weak_ptr's
    inline void swap(this_type& rPtr) throw()
    {
        std::swap< ref_counter* >(refCounter, rPtr.refCounter);
    }

    // Get object
    inline ref_ptr<T> lock () const
    {
        if ( refCounter && refCounter->get<T>() ) {
            return ref_ptr<T>( refCounter->get<T>() );
        }
        return ref_ptr<T>();
    }

    // implicit cast to bool type
    inline operator bool() const { return refCounter != 0; }

private:
    ref_counter* refCounter;
};

/** static_cast for ref_ptr */
template<typename T, typename Y>
ref_ptr<T> ref_static_cast(const ref_ptr<Y>& ptr)
{
    return ref_ptr<T>( static_cast<T*>( ptr.get() ) );
}

/** const_cast for ref_ptr */
template<typename T, typename Y>
ref_ptr<T> ref_const_cast(const ref_ptr<Y>& ptr)
{
    return ref_ptr<T>( const_cast<T*>( ptr.get() ) );
}

/** dynamic_cast for ref_ptr */
template<typename T, typename Y>
ref_ptr<T> ref_dynamic_cast(const ref_ptr<Y>& ptr)
{
    return ref_ptr<T>( dynamic_cast<T*>( ptr.get() ) );
}

/** static_cast for weak_ptr */
template<typename T, typename Y>
weak_ptr<T> weak_static_cast(const weak_ptr<Y> rPtr)
{
    return weak_ptr<T>( static_cast<T*>( rPtr.get() ) );
}

/** const_cast for weak_ptr */
template<typename T, typename Y>
weak_ptr<T> weak_const_cast(const weak_ptr<Y> rPtr)
{
    return weak_ptr<T>( const_cast<T*>( rPtr.get() ) );
}

/** dynamic_cast for weak_ptr */
template<typename T, typename Y>
weak_ptr<T> weak_dynamic_cast(const weak_ptr<Y> rPtr)
{
    return weak_ptr<T>( dynamic_cast<T*>( rPtr.get() ) );
}

} // namespace sgl

// for boost::intrusive_ptr
namespace boost {

inline void intrusive_ptr_add_ref(sgl::Referenced* ptr)
{
    ptr->get_ref_counter()->add_ref();
}

inline void intrusive_ptr_release(sgl::Referenced* ptr)
{
    ptr->get_ref_counter()->remove_ref();
}

// to allow boost mem_fn & bind from ref_ptr
template<typename T>
T* get_pointer(const sgl::ref_ptr<T>& ptr)
{
    return ptr.get();
}

// to allow boost mem_fn & bind from ref_ptr
template<typename T>
T* get_pointer(const sgl::weak_ptr<T>& ptr)
{
    return ptr.get();
}

} // namespace boost

#endif // SIMPLE_GL_REFERENCED_H
