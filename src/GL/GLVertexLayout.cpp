#include "GL/GLVertexLayout.h"

using namespace sgl;

const GLenum sgl::BIND_SCALAR_TYPE[__NUMBER_OF_SCALAR_TYPES__] =
{
    0,
    GL_BITMAP,
    GL_BYTE,
	GL_UNSIGNED_BYTE,
    GL_SHORT,
	GL_UNSIGNED_SHORT,
    GL_INT,
    GL_UNSIGNED_INT,
	GL_FLOAT
};


GLVertexLayout::GLVertexLayout( GLDevice*       _device, 
                                unsigned int    numElements, 
                                const ELEMENT*  elements ) :
	device(_device),
    elements(elements, elements + numElements) // copy elements
{
}

GLVertexLayout::~GLVertexLayout()
{
}

void GLVertexLayout::Bind() const
{
    if ( const VertexLayout* vertexLayout = device->CurrentVertexLayout() ) 
    {
        if (vertexLayout != this) {
            static_cast<const GLVertexLayout*>(vertexLayout)->Unbind();
        }
    }

    for (size_t i = 0; i<elements.size(); ++i)
    {
        switch (elements[i].semantic)
        {
        case VERTEX:
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer( elements[i].size, 
                             BIND_SCALAR_TYPE[ elements[i].type ], 
                             elements[i].stride, 
                             (GLvoid*)elements[i].offset );
            break;

        case NORMAL:
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer( BIND_SCALAR_TYPE[ elements[i].type ], 
                             elements[i].stride, 
                             (GLvoid*)elements[i].offset );
            break;

        case COLOR:
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer( elements[i].size, 
                            BIND_SCALAR_TYPE[ elements[i].type ], 
                            elements[i].stride, 
                            (GLvoid*)elements[i].offset );
            break;

        case TEXCOORD:
            glClientActiveTexture(GL_TEXTURE0 + elements[i].index);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer( elements[i].size, 
                               BIND_SCALAR_TYPE[ elements[i].type ], 
                               elements[i].stride, 
                               (GLvoid*)elements[i].offset );
            break;

        case ATTRIBUTE:
            glEnableVertexAttribArray(elements[i].index);
            glVertexAttribPointer( elements[i].index, 
                                   elements[i].size, 
                                   BIND_SCALAR_TYPE[ elements[i].type ], 
                                   false, 
                                   elements[i].stride, 
                                   (GLvoid*)elements[i].offset );
            break;

        default:
            assert(!"Can't get here.");
        }
    }

    device->SetVertexLayout(this);
}

void GLVertexLayout::Unbind() const
{
    if ( device->CurrentVertexLayout() == this )
    {
        for (size_t i = 0; i<elements.size(); ++i)
        {
            switch (elements[i].semantic)
            {
            case VERTEX:
                glDisableClientState(GL_VERTEX_ARRAY);
                break;

            case NORMAL:
                glDisableClientState(GL_NORMAL_ARRAY);
                break;

            case COLOR:
                glDisableClientState(GL_COLOR_ARRAY);
                break;

            case TEXCOORD:
                glClientActiveTexture(GL_TEXTURE0 + elements[i].index);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                break;

            case ATTRIBUTE:
                glDisableVertexAttribArray(elements[i].index);
                break;

            default:
                assert(!"Can't get here.");
            }
        }

        device->SetVertexLayout(0);
    }
}
