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
    if ( glBuffer == GL_ELEMENT_ARRAY_BUFFER ) {
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &oldBuffer);
    }
    else if ( glBuffer == GL_ARRAY_BUFFER ) {
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
    #ifndef SGL_NO_STATUS_CHECK
        else {
            return EInvalidCall("GLBuffer::Map failed. Invalid call.");
        }
    #endif

        GLenum error = glGetError();
        error = glGetError();
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);
        error = glGetError();

        (*data) = glMapBuffer(glTarget, glHint);

    #ifndef SGL_NO_STATUS_CHECK
        error = glGetError();
        if (error == GL_INVALID_VALUE || error == GL_INVALID_OPERATION || error == GL_INVALID_ENUM) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBuffer::Map failed. Invalid call.");
        }
        else if (error == GL_OUT_OF_MEMORY) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBuffer::Map failed. Can't allocate memory for mapping.");
        }
        else if (error != GL_NO_ERROR) 
        {
            glBindBufferARB(glTarget, oldBuffer);
            return EUnknown("GLBuffer::Map failed. OpenGL error");
        }
    #endif

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
        if (error == GL_INVALID_VALUE || error == GL_INVALID_OPERATION || error == GL_INVALID_ENUM) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBuffer::MapRange failed. Invalid call.");
        }
        else if (error == GL_OUT_OF_MEMORY) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBuffer::MapRange failed. Can't allocate memory for mapping.");
        }
        else if (error != GL_NO_ERROR) 
        {
            glBindBufferARB(glTarget, oldBuffer);
            return EUnknown("GLBuffer::MapRange failed. OpenGL error");
        }
    #endif

        return SGL_OK;
    }

    SGL_HRESULT SGL_DLLCALL FlushMappedRange(unsigned offset, unsigned size)
    {
    #ifndef SGL_NO_STATUS_CHECK
        GLuint currentBuffer;
        glGetIntegerv(glTarget, (GLint*)&currentBuffer);
        if (currentBuffer != glBuffer) {
            return EInvalidCall("GLBuffer::FlushMappedRange failed. Buffer is not mapped.");
        }
    #endif

        glFlushMappedBufferRange(glTarget, offset, size);

    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error == GL_INVALID_VALUE || error == GL_INVALID_OPERATION) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBuffer::MapRange failed. Invalid call.");
        }
        else if (error != GL_NO_ERROR) 
        {
            glBindBufferARB(glTarget, oldBuffer);
            return EUnknown("GLBuffer::MapRange failed. OpenGL error");
        }
    #endif

        return SGL_OK;
    }

    SGL_HRESULT SGL_DLLCALL Unmap()
    {
    #ifndef SGL_NO_STATUS_CHECK
        GLuint currentBuffer;
        if ( glTarget == GL_ELEMENT_ARRAY_BUFFER ) {
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&currentBuffer);
        }
        else if ( glTarget == GL_ARRAY_BUFFER ) {
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&currentBuffer);
        }

        if (currentBuffer != glBuffer) {
            return EInvalidCall("GLBuffer::Unmap failed. Buffer is not mapped.");
        }
    #endif

        glUnmapBuffer(glTarget);

    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error == GL_INVALID_OPERATION) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBuffer::Unmap failed. Buffer is not mapped.");
        }
    #endif

        return SGL_OK;
    }

    bool SGL_DLLCALL Mapped() const
    {
        GLuint currentBuffer;
        glGetIntegerv(glTarget, (GLint*)&currentBuffer);
        if (currentBuffer != glBuffer) {
            return false;
        }

        GLint bufferMapped;
        glGetBufferParameteriv(glTarget, GL_BUFFER_MAPPED, &bufferMapped);
        return bufferMapped != 0;
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
        if (error == GL_OUT_OF_MEMORY) 
        {
            glBindBufferARB(glTarget, oldBuffer);
            return EOutOfMemory("GLBuffer::SetData failed. Out of memory");
        }
        else if (error != GL_NO_ERROR) 
        {
            glBindBufferARB(glTarget, oldBuffer);
            return EUnknown("GLBuffer::SetData failed. OpenGL error");
        }
    #endif

        // set properties
	    dataSize = _dataSize;
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
        if (error == GL_INVALID_VALUE || error == GL_INVALID_OPERATION) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBuffer::SetSubData failed. Out of bounds.");
        }
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EUnknown("GLBuffer::SetSubData failed. OpenGL error");
        }
    #endif

	    return SGL_OK;
    }

    SGL_HRESULT SGL_DLLCALL GetData( void*         data,
                                     unsigned int  offset,
                                     unsigned int  dataSize ) const
    {
        // get buffer data
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);
        void* mapped = glMapBufferRange(glTarget, offset, dataSize, GL_MAP_READ_BIT);

    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error == GL_INVALID_VALUE || error == GL_INVALID_OPERATION || error == GL_INVALID_ENUM) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBuffer::GetData failed. Invalid call.");
        }
        else if (error == GL_OUT_OF_MEMORY) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBuffer::GetData failed. Can't allocate memory for mapping.");
        }
        else if (error != GL_NO_ERROR) 
        {
            glBindBufferARB(glTarget, oldBuffer);
            return EUnknown("GLBuffer::GetData failed. OpenGL error");
        }
    #endif

        // copy
        memcpy(data, mapped, dataSize);

        // restore binding
        glUnmapBuffer(glTarget);
        glBindBufferARB(glTarget, oldBuffer);
        return SGL_OK;
    }

    unsigned int SGL_DLLCALL Size() const { return dataSize; }

protected:
    GLDevice*      device;

    // data
    GLuint         glTarget;
    Buffer::USAGE  usage;
    GLuint         glBuffer;
    GLuint         oldBuffer;
    size_t         dataSize;
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
        if (target->Size() != dataSize) 
        {
            SGL_HRESULT hr = target->SetData(dataSize, 0);
            if (hr != S_OK) {
                return hr;
            }
        }

        return CopyTo(target, 0, 0, dataSize);
    }

    SGL_HRESULT SGL_DLLCALL CopyTo( Buffer*      target,
                                    unsigned int offsetSrc,
                                    unsigned int offsetDst,
                                    unsigned int size ) const
    {
        // hack
        GLBufferModern<Interface>* targetBuf = static_cast<GLBufferModern<Interface>*>(target);

        glBindBuffer(GL_COPY_READ_BUFFER, glBuffer);
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
            hr = target->SetData(dataSize, data);
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
    #ifndef SGL_NO_STATUS_CHECK
        if ( offsetSrc + size > dataSize || offsetDst + size > target->Size() ) {
            return EInvalidCall("GLBuffer::CopyTo failed: trying to access outside buffer bounds");
        }
    #endif

        void* data = 0;
        SGL_HRESULT hr = const_cast<GLBufferDefault*>(this)->Map(Buffer::MAP_READ_BIT, &data);
        {
            if (hr != SGL_OK) {
                return hr;
            }
            hr = target->SetData(dataSize, data);
        }
        const_cast<GLBufferDefault*>(this)->Unmap();

        return hr;
    }
};

} // namespace sgl

#endif // SIMPLE_GL_GL_BUFFER_OBJECT_H
