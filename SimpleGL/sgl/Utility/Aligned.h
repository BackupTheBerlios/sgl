#ifndef SIMPLE_GL_UTILITY_ALIGNED_HPP
#define SIMPLE_GL_UTILITY_ALIGNED_HPP

#include <cassert>
#include <limits>
#include <malloc.h>
#ifdef __GNUC__
#include <mm_malloc.h>
#endif
#include <stdexcept>
#include "Meta.h"

namespace sgl {

/** Allocate aligned data.
 * @size - size of the data forr allocaation in bytes.
 * @alignment - data alignment in bytes.
 */
inline void* align_alloc(size_t size, size_t alignment = 0x10)
{
#ifdef __GNUC__
    return _mm_malloc(size, alignment);
#else // MSVS
    return _aligned_malloc(size, alignment);
#endif
}

/** Free aligned data */
inline void align_free(void* data)
{
#ifdef __GNUC__
    _mm_free(data);
#else // MSVS
    _aligned_free(data);
#endif
}

/** Overloads operator new and delete for aligning class. */
template<int align>
class Aligned
{
public:
    void* operator new(size_t size)
    {
        void* pData = align_alloc(size, align);
        if (!pData) {
            throw std::bad_alloc();
        }

        return pData;
    }

    void* operator new [](size_t size)
    {
        void* pData = align_alloc(size, align);
        if (!pData) {
            throw std::bad_alloc();
        }

        return pData;
    }

    void* operator new(size_t /*size*/, void* where)
    {
        assert( reinterpret_cast<size_t>(where) % align == 0 && "Data is not properly aligned!" );
        return where;
    }

    void operator delete(void* pData)
    {
        if (pData) {
            align_free(pData);
        }
    }

    void operator delete [](void* pData)
    {
        if (pData) {
            align_free(pData);
        }
    }

    // actually we don't need storing delete, but MSVS produces warning
    void operator delete(void* /*pData*/, void* /*where*/) {}

protected:
    // Do not delete using ptr to Aligned
    inline ~Aligned() {}
};

/// Aligned 16 bytes
typedef Aligned<0x10> Aligned16;

/** Aligned allocator for STL containers.
 * Developed by Gregory Bonik.
 */
template<typename T>
class aligned_allocator
{
public:
    typedef size_t      ptr_uint_type;

public:
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef T           value_type;

    template<class U>
    struct rebind
    {
        typedef aligned_allocator<U> other;
    };

    enum { packing = sizeof(value_type) };

    aligned_allocator ()                         throw() { }
    aligned_allocator (const aligned_allocator&) throw() { }

    template<class U>
    aligned_allocator (const aligned_allocator<U>&) throw()
    { }

    pointer address(reference x) const
    {
        return &x;
    }

    const_pointer address(const_reference x) const
    {
        return &x;
    }

    pointer allocate( size_type sz, const void* hint = NULL)
    {
        do_nothing(hint);

        size_type to_alloc  = sz * sizeof(value_type);
        unsigned char *p    = new unsigned char[to_alloc + packing];
        ptr_uint_type u     = reinterpret_cast<ptr_uint_type> (p);

        u = (packing - u % packing) % packing;
        p += u;

        p[to_alloc] = static_cast<unsigned char> (u);

        return reinterpret_cast<pointer> (p);
    }

    void deallocate(pointer p, size_type n)
    {
        unsigned char *c = reinterpret_cast<unsigned char*> (p);
        c -= c[sizeof(value_type) * n];
        delete []c;
    }

    size_type max_size() const throw()
    {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    void construct(pointer p, const T& val)
    {
        new(static_cast<void*>(p)) T(val);
    }

    void destroy(pointer p)
    {
        p->~T();
    }
};

/** Vector compatible with the aligned types. MSVS stl vector won't compile */
template< typename T, typename Allocator = std::allocator<T> >
class vector
{
public:
    typedef vector<T, Allocator>                    this_type;

    typedef typename Allocator::reference           reference;
    typedef typename Allocator::const_reference     const_reference;

    typedef T*                                      iterator;
    typedef const T*                                const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;

    typedef T                                       value_type;
    typedef Allocator                               allocator_type;
    typedef typename Allocator::pointer             pointer;
    typedef typename Allocator::const_pointer       const_pointer;

private:
    template<class Iterator>
    void construct(Iterator first, Iterator last)
    {
        iterator dataIter = begin();
        Iterator iter     = first;

        // copy
        for ( iter = first;
              iter != last && dataIter != end();
              ++iter, ++dataIter )
        {
            *dataIter = *iter;
        }

        // append
        for (; iter != last; ++iter) {
            push_back(*iter);
        }
    }

    void construct_default(T* first, T* last)
    {
        T val;
        for (T* p = first; p != last; ++p) {
            allocator.construct(p, val);
        }
    }

    size_type calc_capacity(size_type size)
    {
        static const size_type min_size = 5;

        size_type capacity = static_cast<size_type>(size * size_modifier);
        if (capacity < min_size) {
            capacity = min_size;
        }
        else if ( capacity > max_size() ) {
            capacity = max_size();
        }

        return capacity;
    }

    void destroy(T* first, T* last)
    {
        for (T* p = first; p != last; ++p) {
            allocator.destroy(p);
        }
    }

    void destroy_and_deallocate()
    {
        if (data)
        {
            destroy(data, data + dataSize);
            allocator.deallocate(data, dataCapacity);
        }
    }

public:
    vector(size_type size = 0) :
        data(0),
        dataSize(0),
        dataCapacity(0),
        size_modifier(1.5f)
    {
        resize(size);
    }

    explicit vector(const allocator_type& _allocator) :
        data(0),
        dataSize(0),
        dataCapacity(0),
        size_modifier(1.5f),
        allocator(_allocator)
    {
        resize(size);
    }

    vector( size_type         size,
            const value_type& val) :
        data(0),
        dataSize(0),
        dataCapacity(0),
        size_modifier(1.5f)
    {
        resize(size, val);
    }

    explicit vector( size_type             size,
                     const value_type&     val,
                     const allocator_type& _allocator) :
        data(0),
        dataSize(0),
        dataCapacity(0),
        size_modifier(1.5f),
        allocator(_allocator)
    {
        resize(size, val);
    }

    vector(const this_type& vec) :
        data(0),
        dataSize(0),
        dataCapacity(0),
        size_modifier(1.5f)
    {
        resize(vec.dataSize);
        if (vec.data) {
            std::copy(vec.data, vec.data + vec.dataSize, data);
        }
    }

    template<class Iterator>
    vector( Iterator first,
            Iterator last ) :
        data(0),
        dataSize(0),
        size_modifier(1.5f)
    {
        construct(first, last);
    }

    template<class Iterator>
    vector( Iterator              first,
            Iterator              last,
            const allocator_type& _allocator ) :
        data(0),
        dataSize(0),
        size_modifier(1.5f),
        allocator(_allocator)
    {
        construct(first, last);
    }

    this_type& operator = (const this_type& vec)
    {
        if (vec.dataSize > dataCapacity)
        {
            destroy_and_deallocate();

            // allocate
            dataCapacity = calc_capacity(vec.dataSize);
            data         = allocator.allocate(dataCapacity);
        }

        if (vec.data) {
            std::copy(vec.data, vec.data + vec.dataSize, data);
        }
        dataSize = vec.dataSize;

        return *this;
    }

    ~vector()
    {
        destroy_and_deallocate();
    }

    iterator begin() { return data; }
    iterator end()   { return data + dataSize; }

    const_iterator begin() const { return data; }
    const_iterator end() const   { return data + dataSize; }

    reverse_iterator rbegin() { return reverse_iterator(data + dataSize - 1); }
    reverse_iterator rend()   { return reverse_iterator(data - 1); }

    const_reverse_iterator rbegin() const { return reverse_iterator(data + dataSize - 1); }
    const_reverse_iterator rend() const   { return reverse_iterator(data - 1); }

    size_type size() const      { return dataSize; }
    size_type max_size() const  { return allocator.max_size(); }
    size_type capacity() const  { return dataCapacity; }
    bool      empty() const     { return (dataSize == 0); }

    void resize(size_type size)
    {
        assert( size < max_size() );
        if (size > dataCapacity)
        {
            // allocate
            size_type newCapacity = calc_capacity(size);
            T*        newData     = allocator.allocate(newCapacity);

            // fill with default values
            construct_default(newData, newData + dataSize);

            // copy & store
            if (data)
            {
                std::copy(data, data + dataSize, newData);
                destroy(data, data + dataSize);
                allocator.deallocate(data, dataCapacity);
            }
            data         = newData;
            dataCapacity = newCapacity;
        }

        // fill with default values
        if (dataSize < size) {
            construct_default(data + dataSize, data + size);
        }
        dataSize = size;
    }

    void reserve(size_type capacity)
    {
        assert( capacity < max_size() );
        if (capacity > dataCapacity)
        {
            size_type newCapacity = capacity;
            T*        newData     = allocator.allocate(newCapacity);

            // fill with default values
            construct_default(newData, newData + dataSize);

            // copy
            if (data)
            {
                std::copy(data, data + dataSize, newData);
                destroy(data, data + dataSize);
                allocator.deallocate(data, dataCapacity);
            }
            data         = newData;
            dataCapacity = newCapacity;
        }
    }

    reference operator[] (size_type i)
    {
        assert(i < dataSize);
        return data[i];
    }

    const_reference operator[] (size_type i) const
    {
        assert(i < dataSize);
        return data[i];
    }

    reference at(size_type i)
    {
        if (i >= dataSize) {
            throw std::out_of_range("index out of range");
        }
        return data[i];
    }

    const_reference at(size_type i) const
    {
        if (i >= dataSize) {
            throw std::out_of_range("index out of range");
        }
        return data[i];
    }

    reference front()
    {
        assert( !empty() );
        return data[0];
    }

    const_reference front() const
    {
        assert( !empty() );
        return data[0];
    }

    reference back()
    {
        assert( !empty() );
        return data[dataSize - 1];
    }

    const_reference back() const
    {
        assert( !empty() );
        return data[dataSize - 1];
    }

    template <class Iterator>
    void assign(Iterator first, Iterator last)
    {
        construct(first, last);
    }

    void assign(size_type size, const T& val)
    {
        assert( size < max_size() );
        if (size > dataCapacity)
        {
            // deallocate
            destroy_and_deallocate();

            // allocate
            dataCapacity = calc_capacity(size);
            data         = allocator.allocate(dataCapacity);

            // copy & store
            std::fill(data, data + size, val);
        }
        else {
            std::fill(data, data + size, val);
        }

        dataSize = size;
    }

    void push_back(const_reference val)
    {
        resize(dataSize + 1);
        back() = val;
    }

    void pop_back()
    {
        assert( !empty() );
        --dataSize;
        allocator.destroy(data + dataSize);
    }

    iterator erase(iterator pos)
    {
        assert(pos > data && pos < data + dataSize);

        iterator nextPos = pos;
        iterator prevPos = pos;
        for (++nextPos; nextPos != end(); ++nextPos, ++prevPos) {
            *prevPos = *nextPos;
        }

        --dataSize;
        return pos;
    }

    iterator erase(iterator first, iterator last)
    {
        assert( first > data && first < data + dataSize
                && last > data && last < data + dataSize );

        iterator nextPos = last;
        iterator prevPos = first;
        for ( ; nextPos != end(); ++nextPos, ++prevPos) {
            *prevPos = *nextPos;
        }

        --dataSize;
        return first;
    }

    void swap(this_type& vec)
    {
        std::swap(data, vec.data);
        std::swap(dataSize, vec.dataSize);
        std::swap(dataCapacity, vec.dataCapacity);
    }

    void clear()
    {
        if (data)
        {
            destroy(data, data + dataSize);
            allocator.deallocate(data, dataCapacity);
            data         = 0;
            dataSize     = 0;
            dataCapacity = 0;
        }
    }

    allocator_type get_allocator() const { return allocator; }

private:
    T*              data;
    size_t          dataSize;
    size_t          dataCapacity;
    float           size_modifier;
    allocator_type  allocator;
};

} // namespace sgl

#endif // SIMPLE_GL_UTILITY_ALIGNED_HPP
