#include "GL/GLCommon.h"
#include "GL/GLDevice.h"
#include "GL/GLVertexLayout.h"
#include <algorithm>
#include <vector>

namespace {

    using namespace sgl;

	/* Sort elements by index */
	struct sort_by_index
	{
		bool operator () (const VertexLayout::ELEMENT& a, const VertexLayout::ELEMENT& b)
		{
			return a.index < b.index;
		}
	};

} // anonymous namespace

namespace sgl {

const unsigned BIND_SCALAR_TYPE[__NUMBER_OF_SCALAR_TYPES__] =
{
	0,
#ifdef SIMPLE_GL_ES
    0,
#else
	GL_BITMAP,
#endif
	GL_BOOL,
	GL_BYTE,
	GL_UNSIGNED_BYTE,
	GL_SHORT,
	GL_UNSIGNED_SHORT,
	GL_INT,
	GL_UNSIGNED_INT,
	GL_FLOAT,
#ifdef SIMPLE_GL_ES
    0
#else
    GL_DOUBLE
#endif
};

GLVertexLayoutMixed::GLVertexLayoutMixed( GLDevice*    device_, 
											unsigned int                numElements, 
											const ELEMENT*              elements ) :
	device(device_),
    elements(elements, elements + numElements) // copy elements
{
}

GLVertexLayoutMixed::~GLVertexLayoutMixed()
{
    if (device->Valid()) {
    	Unbind();
    }
}

void GLVertexLayoutMixed::Bind() const
{
    if ( const this_type* vertexLayout = static_cast<const this_type*>( device->CurrentVertexLayout() ) ) 
    {
        if (vertexLayout != this) {
            vertexLayout->Unbind();
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

void GLVertexLayoutMixed::Unbind() const
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

GLVertexLayoutAttribute::GLVertexLayoutAttribute( GLDevice*    device_, 
											  	  unsigned int                numElements, 
												  const ELEMENT*              elements_ ) :
	device(device_),
    elements(elements_, elements_ + numElements) // copy elements
{
    std::sort( elements.begin(), elements.end(), sort_by_index() );
}

GLVertexLayoutAttribute::~GLVertexLayoutAttribute()
{
    Unbind();
}

void GLVertexLayoutAttribute::Bind() const
{
    if ( const GLVertexLayoutAttribute* vertexLayout = static_cast<const GLVertexLayoutAttribute*>( device->CurrentVertexLayout() ) ) 
    {
        if (vertexLayout != this) 
        {
            // perform vertex layout switch
            size_t i = 0;
            size_t j = 0;
            while ( i < vertexLayout->elements.size() && j < elements.size() ) 
            {
                size_t index0 = vertexLayout->elements[i].index;
                size_t index1 = elements[j].index;
                if (index0 < index1)
                {
                    glDisableVertexAttribArray(index0);
                    ++i;
                }
                else
                {
                    if (index0 > index1) {
                        glEnableVertexAttribArray(index1);
                    }

                    glVertexAttribPointer( index1,
                                           elements[j].size,
                                           BIND_SCALAR_TYPE[ elements[j].type ],
                                           false,
                                           elements[j].stride,
                                           (GLvoid*)elements[j].offset );
                    ++i; ++j;
                }
            }

            for (; i<vertexLayout->elements.size(); ++i) {
                glDisableVertexAttribArray(vertexLayout->elements[i].index);
            }

            for (; j<elements.size(); ++j)
            {
                glEnableVertexAttribArray(elements[j].index);

                glVertexAttribPointer( elements[j].index,
                                       elements[j].size,
                                       BIND_SCALAR_TYPE[ elements[j].type ],
                                       false,
                                       elements[j].stride,
                                       (GLvoid*)elements[j].offset );

            }

            device->SetVertexLayout(this);
        }
    }
    else
    {
        // setup vertex layout
        for (size_t i = 0; i<elements.size(); ++i)
        {
            glEnableVertexAttribArray(elements[i].index);
            glVertexAttribPointer( elements[i].index, 
                                   elements[i].size, 
                                   BIND_SCALAR_TYPE[ elements[i].type ], 
                                   false, 
                                   elements[i].stride, 
								   (GLvoid*)elements[i].offset );
		}

		device->SetVertexLayout(this);
	}
}

void GLVertexLayoutAttribute::Unbind() const
{
	if ( device->CurrentVertexLayout() == this )
	{
		for (size_t i = 0; i<elements.size(); ++i) {
			glDisableVertexAttribArray(elements[i].index);
		}

		device->SetVertexLayout(0);
	}
}

} // namespace sgl
