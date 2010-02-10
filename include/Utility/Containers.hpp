/**
 * \author DikobrAz
 * \date 08.09.09
 * File contains aligned arrays to store sse optimized vectors.
 * Actually used STL containers with aligned allocator if the
 * SSE optimization is enabled and without it otherwise. For the
 * MSVS there is own vector implementation because of compilation
 * bug using vector of aligned elements.
 */

#ifndef SIMPLE_GL_MATH_CONTAINERS_HPP
#define SIMPLE_GL_MATH_CONTAINERS_HPP

#include "../Utility/Aligned.h"
#include "Matrix.hpp"
#include <algorithm>
#include <vector>

#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
namespace sgl {
#endif
namespace math {

#ifdef SIMPLE_GL_USE_SSE

    #ifdef __GNUC__

    /// Aligned(0x10) std::vector of Vector4f
    typedef std::vector< Vector4f, sgl::aligned_allocator<Vector4f> > vector_of_vector4f;
    /// Aligned(0x10) std::vector of Matrix4f
    typedef std::vector< Matrix4f, sgl::aligned_allocator<Matrix4f> > vector_of_matrix4f;

    #else // MSVS

    template< typename T, typename Allocator = std::allocator<T> >
    class vector
    {
    public:
        typedef vector<T, Allocator>                    this_type;

        typedef typename Allocator::reference           reference;
        typedef typename Allocator::const_reference     const_reference;

        typedef typename T*                             iterator;
        typedef typename const T*                       const_iterator;
        typedef std::reverse_iterator<iterator>         reverse_iterator;
        typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

        typedef size_t                                  size_type;
        typedef ptrdiff_t                               difference_type;

        typedef T                                       value_type;
        typedef typename Allocator                      allocator_type;
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
            return *this;
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
        iterator end()   { return data + numElements; }

        const_iterator begin() const { return data; }
        const_iterator end() const   { return data + numElements; }

        reverse_iterator rbegin() { return reverse_iterator(data + numElements - 1); }
        reverse_iterator rend()   { return reverse_iterator(data - 1); }

        const_reverse_iterator rbegin() const { return reverse_iterator(data + numElements - 1); }
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

            // fill
            T val;
            for (T* p = data + dataSize; p != data + size; ++p) {
                allocator.construct(p, val);
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

    /// Aligned(0x10) std::vector of Vector4f
    typedef math::vector< Vector4f, sgl::aligned_allocator<Vector4f> > vector_of_vector4f;
    /// Aligned(0x10) std::vector of Matrix4f
    typedef math::vector< Matrix4f, sgl::aligned_allocator<Matrix4f> > vector_of_matrix4f;

    #endif // MSVS

#else // SIMPLE_GL_USE_SSE

/// std::vector of Vector4f
typedef std::vector<Vector4f> vector_of_vector4f;
/// std::vector of Matrix4f
typedef std::vector<Matrix4f> vector_of_matrix4f;

#endif // SIMPLE_GL_USE_SSE

} // namespace math
#ifdef SIMPLEGL_MATH_IN_SGL_NAMESPACE
} // namespace sgl
#endif

#endif // SIMPLE_GL_MATH_CONTAINERS_HPP
