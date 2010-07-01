#ifndef SIMPLE_GL_GL_BUFFER_OBJECT_H
#define SIMPLE_GL_GL_BUFFER_OBJECT_H

#include "GLDevice.h"
#include <cstdlib>
#include <cstring>

namespace sgl {

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

template<DEVICE_VERSION DeviceVersion, typename Interface>
class GLBuffer :
    public ReferencedImpl<Interface>
{
protected:
    GLBuffer( GLDevice<DeviceVersion>*  _device,
              GLuint                    _glTarget ) :
        device(_device),
        glTarget(_glTarget),
        glUsage(GL_STATIC_DRAW),
	    glBuffer(0)
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
        if (hint == LOCK_READ_BIT) {
            glHint = GL_READ_ONLY;
        }
        else if (hint == LOCK_WRITE_BIT) {
            glHint = GL_WRITE_ONLY;
        }
        else if (hint == (LOCK_READ_BIT | LOCK_WRITE_BIT)) {
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

        GLuint glHint = (hint & LOCK_READ_BIT) * GL_MAP_READ_BIT
                      | (hint & LOCK_WRITE_BIT) * GL_MAP_WRITE_BIT
                      | (hint & LOCK_INVALIDATE_RANGE_BIT) * GL_MAP_INVALIDATE_RANGE_BIT
                      | (hint & LOCK_INVALIDATE_BUFFER_BIT) * GL_MAP_INVALIDATE_BUFFER_BIT
                      | (hint & LOCK_UNSYNCHRONIZED_BIT) * GL_MAP_UNSYNCHRONIZED_BIT;

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
        GLint currentBuffer;
        glGetIntegerv(glTarget, &currentBuffer);
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
        GLint currentBuffer;
        if ( glTarget == GL_ELEMENT_ARRAY_BUFFER ) {
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentBuffer);
        }
        else if ( glTarget == GL_ARRAY_BUFFER ) {
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentBuffer);
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
        GLint currentBuffer;
        glGetIntegerv(glTarget, &currentBuffer);
        if (currentBuffer != glBuffer) {
            return false;
        }

        GLint bufferMapped;
        glGetBufferParameteriv(glTarget, GL_BUFFER_MAPPED, &bufferMapped);
        return bufferMapped != 0;
    }

    // Override BufferView
    SGL_HRESULT SGL_DLLCALL SetData( unsigned int   _dataSize,
                                     const void*    data )
    {
        // set buffer data
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);
        glBufferDataARB(glTarget, _dataSize, data, glUsage);
    
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
                                     unsigned int  dataSize )
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
    GLDevice<DeviceVersion>*    device;

    // data
    GLuint  glTarget;
    GLuint  glUsage;
    GLuint  glBuffer;
    GLuint  oldBuffer;
    size_t  dataSize;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_BUFFER_OBJECT_H
