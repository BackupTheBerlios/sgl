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

    glBindBuffer(glTarget, glBuffer);
    return oldBuffer;
}

template<typename Interface>
class GLBuffer :
    public ReferencedImpl<Interface>
{
protected:
    GLBuffer( Device*   _device,
              GLuint    _glTarget ) :
        device(_device),
        glTarget(_glTarget),
        glUsage(GL_STATIC_DRAW),
	    glBuffer(0)
    {
        glGenBuffers(1, &glBuffer);
    }

    ~GLBuffer()
    {
        glDeleteBuffers(1, &glBuffer);
    }

public:
    // Override BufferView
    SGL_HRESULT SGL_DLLCALL SetData( unsigned int   _dataSize,
                                     const void*    data )
    {
        // set buffer data
        GLuint oldBuffer = GuardedBind(glTarget, glBuffer);
        glBufferData(glTarget, _dataSize, data, glUsage);
    
    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error == GL_OUT_OF_MEMORY) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EOutOfMemory("GLBufferView::SetData failed. Out of memory");
        }
        else if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EUnknown("GLBufferView::SetData failed. OpenGL error");
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
        glBufferSubData(glTarget, offset, chunkSize, data);
    
    #ifndef SGL_NO_STATUS_CHECK
        GLenum error = glGetError();
        if (error == GL_INVALID_VALUE) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBufferView::SetSubData failed. Out of bounds.");
        }
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EUnknown("GLBufferView::SetSubData failed. OpenGL error");
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
        if (error == GL_INVALID_VALUE) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EInvalidCall("GLBufferView::GetData failed. Out of bounds.");
        }
        if (error != GL_NO_ERROR) 
        {
            glBindBuffer(glTarget, oldBuffer);
            return EUnknown("GLBufferView::GetData failed. OpenGL error");
        }
    #endif

        // copy
        memcpy(data, mapped, dataSize);

        // restore binding
        glUnmapBuffer(glTarget);
        glBindBuffer(glTarget, oldBuffer);
        return SGL_OK;
    }

    unsigned int SGL_DLLCALL Size() const { return dataSize; }

protected:
    ref_ptr<Device>     device;

    // data
    GLuint              glTarget;
    GLuint              glUsage;
    GLuint              glBuffer;
    size_t              dataSize;
};

} // namespace sgl

#endif // SIMPLE_GL_GL_BUFFER_OBJECT_H
