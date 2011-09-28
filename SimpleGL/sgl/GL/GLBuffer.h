#ifndef SIMPLE_GL_GL_BUFFER_OBJECT_H
#define SIMPLE_GL_GL_BUFFER_OBJECT_H

#include "GLCommon.h"
#include "GLDevice.h"
#include <cstdlib>
#include <cstring>

namespace sgl {

const GLuint BIND_GL_USAGE[] = 
{
	GL_STREAM_DRAW, 
	GL_STREAM_READ, 
	GL_STREAM_COPY, 
    GL_STATIC_DRAW, 
	GL_STATIC_READ, 
	GL_STATIC_COPY, 
    GL_DYNAMIC_DRAW, 
	GL_DYNAMIC_READ, 
	GL_DYNAMIC_COPY
};

inline GLint GuardedBind(GLuint glTarget, GLuint glBuffer)
{
    GLint oldBuffer;
    if ( glTarget == GL_ELEMENT_ARRAY_BUFFER ) {
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &oldBuffer);
    }
    else if ( glTarget == GL_ARRAY_BUFFER ) {
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
    }
    else {
        oldBuffer = 0; // do nothing
    }

    glBindBufferARB(glTarget, glBuffer);
    return oldBuffer;
}

template<typename Interface>
class GLBuffer :
    public ReferencedImpl<Interface>
{
protected:
    typedef ReferencedImpl<Interface> base_type;

protected:
    GLBuffer( GLDevice*  _device,
              GLuint     _glTarget ) :
        device(_device),
        glTarget(_glTarget),
        usage(Buffer::STATIC_DRAW),
	    glBuffer(0),
        dataSize(0)
    {
        glGenBuffersARB(1, &glBuffer);
    }

    ~GLBuffer()
    {
        glDeleteBuffersARB(1, &glBuffer);
    }

public:
    SGL_HRESULT SGL_DLLCALL Map(int     hint, 
                                void**  data)
    {
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);

        GLuint glHint = 0;
        if (hint == Buffer::MAP_READ_BIT) {
            glHint = GL_READ_ONLY;
        }
        else if (hint == Buffer::MAP_WRITE_BIT) {
            glHint = GL_WRITE_ONLY;
        }
        else if (hint == (Buffer::MAP_READ_BIT | Buffer::MAP_WRITE_BIT)) {
            glHint = GL_READ_WRITE;
        }
        else {
            assert(!"Invalid hint. Can be only MAP_READ_BIT, MAP_WRITE_BIT, MAP_READ_BIT | MAP_WRITE_BIT");
        }

        (*data) = glMapBuffer(glTarget, glHint);
    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return CheckGLError("GLBuffer::Map failed: ", error);
        }
    #endif

        mapped = true;
        return SGL_OK;
    }

    SGL_HRESULT SGL_DLLCALL MapRange(unsigned   offset, 
                                     unsigned   size, 
                                     int        hint, 
                                     void**     data)
    {
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);

        GLuint glHint = (hint & Buffer::MAP_READ_BIT) * GL_MAP_READ_BIT
                      | (hint & Buffer::MAP_WRITE_BIT) * GL_MAP_WRITE_BIT
                      | (hint & Buffer::MAP_INVALIDATE_RANGE_BIT) * GL_MAP_INVALIDATE_RANGE_BIT
                      | (hint & Buffer::MAP_INVALIDATE_BUFFER_BIT) * GL_MAP_INVALIDATE_BUFFER_BIT
                      | (hint & Buffer::MAP_UNSYNCHRONIZED_BIT) * GL_MAP_UNSYNCHRONIZED_BIT;

        (*data) = glMapBufferRange(glTarget, offset, size, glHint);
    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return CheckGLError("GLBuffer::MapRange failed: ", error);
        }
    #endif
        mapped = true;

        // restore
        if (oldBuffer != glBuffer) {
            glBindBufferARB(glTarget, oldBuffer);
        }
        return SGL_OK;
    }

    SGL_HRESULT SGL_DLLCALL FlushMappedRange(unsigned offset, unsigned size)
    {
        assert(mapped);
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);

        glFlushMappedBufferRange(glTarget, offset, size);
    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return CheckGLError("GLBuffer::FlushMappedRange failed: ", error);
        }
    #endif

        // restore
        if (oldBuffer != glBuffer) {
            glBindBufferARB(glTarget, oldBuffer);
        }
        return SGL_OK;
    }

    SGL_HRESULT SGL_DLLCALL Unmap()
    {
        assert(mapped);
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);

        glUnmapBuffer(glTarget);
    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return CheckGLError("GLBuffer::Unmap failed: ", error);
        }
    #endif
        mapped = false;

        // restore
        if (oldBuffer != glBuffer) {
            glBindBufferARB(glTarget, oldBuffer);
        }
        return SGL_OK;
    }

    bool SGL_DLLCALL Mapped() const
    {
        return mapped;
    }

    Buffer::USAGE SGL_DLLCALL Usage() const
    {
        return usage;
    }

    // Override BufferView
    SGL_HRESULT SGL_DLLCALL SetData( unsigned int   _dataSize,
                                     const void*    data,
                                     Buffer::USAGE  usage_ )
    {
        // set buffer data
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);
        glBufferDataARB(glTarget, _dataSize, data, BIND_GL_USAGE[usage = usage_]);
    
    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return CheckGLError("GLBuffer::SetData failed: ", error);
        }
    #endif
	    dataSize = _dataSize;

        // restore
        if (oldBuffer != glBuffer) {
            glBindBufferARB(glTarget, oldBuffer);
        }
	    return SGL_OK;
    }

    SGL_HRESULT SGL_DLLCALL SetSubData( unsigned int    offset,
                                        unsigned int    chunkSize,
                                        const void*     data )
    {
        // set buffer data
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);
        glBufferSubDataARB(glTarget, offset, chunkSize, data);
    
    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return CheckGLError("GLBuffer::SetSubData failed: ", error);
        }
    #endif

        // restore
        if (oldBuffer != glBuffer) {
            glBindBufferARB(glTarget, oldBuffer);
        }
	    return SGL_OK;
    }

    SGL_HRESULT SGL_DLLCALL GetData( void*         data,
                                     unsigned int  offset,
                                     unsigned int  dataSize ) const
    {
        assert(data);

        // get buffer data
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);

        // try to map
        void* mappedData = glMapBufferRange(glTarget, offset, dataSize, GL_MAP_READ_BIT);
    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return CheckGLError("GLBuffer::GetData failed: ", error);
        }
    #endif

        // copy
        memcpy(data, mappedData, dataSize);

        // try to unmap
        glUnmapBuffer(glTarget);
    #ifndef SGL_NO_STATUS_CHECK
        error = glGetError();
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return CheckGLError("GLBuffer::GetData failed: ", error);
        }
    #endif

        // restore
        if (oldBuffer != glBuffer) {
            glBindBufferARB(glTarget, oldBuffer);
        }
        return SGL_OK;
    }

    unsigned int SGL_DLLCALL Size() const
    { 
        return dataSize;
    }

protected:
    GLDevice*      device;

    // data
    GLuint         glTarget;
    Buffer::USAGE  usage;
    GLuint         glBuffer;
    GLuint         oldBuffer;
    size_t         dataSize;
    bool           mapped;
};

template<typename Interface>
class GLBufferModern :
    public GLBuffer<Interface>
{
protected:
    GLBufferModern( GLDevice*  device,
                    GLuint     glTarget ) 
    :   GLBuffer<Interface>(device, glTarget)
    {
    }

public:
    SGL_HRESULT SGL_DLLCALL CopyTo( Buffer* target ) const
    {
        assert(target);
        if (target->Size() != GLBuffer<Interface>::dataSize)
        {
            SGL_HRESULT hr = target->SetData(GLBuffer<Interface>::dataSize, 0);
            if (hr != SGL_OK) {
                return hr;
            }
        }

        return CopyTo(target, 0, 0, GLBuffer<Interface>::dataSize);
    }

    SGL_HRESULT SGL_DLLCALL CopyTo( Buffer*      target,
                                    unsigned int offsetSrc,
                                    unsigned int offsetDst,
                                    unsigned int size ) const
    {
        // hack
        GLBufferModern<Interface>* targetBuf = static_cast<GLBufferModern<Interface>*>(target);

        glBindBuffer(GL_COPY_READ_BUFFER, GLBuffer<Interface>::glBuffer);
        glBindBuffer(GL_COPY_WRITE_BUFFER, targetBuf->glBuffer);
        glCopyBufferSubData( GL_COPY_READ_BUFFER, 
                             GL_COPY_WRITE_BUFFER,
                             offsetSrc,
                             offsetDst,
                             size );
#ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            return CheckGLError("GLBuffer::CopyTo failed: ", error);
        }
#endif

        return SGL_OK;
    }
};

template<typename Interface>
class GLBufferDefault :
    public GLBuffer<Interface>
{
protected:
    GLBufferDefault( GLDevice*  device,
                     GLuint     glTarget ) 
    :   GLBuffer<Interface>(device, glTarget)
    {
    }

public:
    SGL_HRESULT SGL_DLLCALL CopyTo( Buffer* target ) const
    {
        assert(target);

        void* data = 0;
        SGL_HRESULT hr = const_cast<GLBufferDefault*>(this)->Map(Buffer::MAP_READ_BIT, &data);
        {
            if (hr != SGL_OK) {
                return hr;
            }
            hr = target->SetData(GLBuffer<Interface>::dataSize, data);
        }
        const_cast<GLBufferDefault*>(this)->Unmap();

        return hr;
    }

    SGL_HRESULT SGL_DLLCALL CopyTo( Buffer*      target,
                                    unsigned int offsetSrc,
                                    unsigned int offsetDst,
                                    unsigned int size ) const
    {
        assert(target);
        assert( (offsetSrc + size <= GLBuffer<Interface>::dataSize) && (offsetDst + size <= target->Size()) );

        void* data = 0;
        SGL_HRESULT hr = const_cast<GLBufferDefault*>(this)->Map(Buffer::MAP_READ_BIT, &data);
        {
            if (hr != SGL_OK) {
                return hr;
            }
            hr = target->SetData(GLBuffer<Interface>::dataSize, data);
        }
        const_cast<GLBufferDefault*>(this)->Unmap();

        return hr;
    }
};

} // namespace sgl

#endif // SIMPLE_GL_GL_BUFFER_OBJECT_H
