#include "GL/GLCommon.h"
#include "GL/GLDevice.h"
#include "GL/GLVertexLayout.h"
#include <algorithm>
#include <vector>

namespace sgl {
    
    const unsigned sgl::BIND_SCALAR_TYPE[__NUMBER_OF_SCALAR_TYPES__] =
    {
        0,
        GL_BITMAP,
        GL_BOOL,
        GL_BYTE,
	    GL_UNSIGNED_BYTE,
        GL_SHORT,
	    GL_UNSIGNED_SHORT,
        GL_INT,
        GL_UNSIGNED_INT,
	    GL_FLOAT,
        GL_DOUBLE
    };

} // namespace sgl

namespace {

    using namespace sgl;

    /* Vertex declaration */
    template<DEVICE_VERSION DeviceVersion>
    class GLVertexLayoutMixed:
        public ReferencedImpl<VertexLayout>
    {
    public:
        typedef GLVertexLayoutMixed<DeviceVersion>      this_type;

    public:
	    GLVertexLayoutMixed( GLDevice<DeviceVersion>*    device_, 
                             unsigned int                numElements, 
                             const ELEMENT*              elements ) :
	        device(device_),
            elements(elements, elements + numElements) // copy elements
        {
        }

        // Override VertexLayout
        unsigned int    SGL_DLLCALL NumElements() const                 { return elements.size(); }
        ELEMENT         SGL_DLLCALL Element(unsigned int index) const   { return elements[index]; }

        void SGL_DLLCALL Bind() const
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

        void SGL_DLLCALL Unbind() const
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

    private:
        GLDevice<DeviceVersion>*    device;
	    std::vector<ELEMENT>	    elements;
    };

    /* Sort elements by index */
    struct sort_by_index
    {
        bool operator () (const VertexLayout::ELEMENT& a, const VertexLayout::ELEMENT& b)
        {
            return a.index < b.index;
        }
    };

    /* Vertex layout with attributes only */
    template<DEVICE_VERSION DeviceVersion>
    class GLVertexLayoutAttribute :
        public ReferencedImpl<VertexLayout>
    {
    public:
        typedef GLVertexLayoutAttribute<DeviceVersion>  this_type;

    public:
	    GLVertexLayoutAttribute( GLDevice<DeviceVersion>*    device_, 
                                 unsigned int                numElements, 
                                 const ELEMENT*              elements_ ) :
	        device(device_),
            elements(elements_, elements_ + numElements) // copy elements
        {
            std::sort( elements.begin(), elements.end(), sort_by_index() );
        }

        // Override VertexLayout
        unsigned int    SGL_DLLCALL NumElements() const                 { return elements.size(); }
        ELEMENT         SGL_DLLCALL Element(unsigned int index) const   { return elements[index]; }

        void SGL_DLLCALL Bind() const
        {
            if ( const this_type* vertexLayout = static_cast<const this_type*>( device->CurrentVertexLayout() ) ) 
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
                        if (index0 < index1) {
                            glDisableVertexAttribArray(index0);
                        }
                        else if (index0 > index1) {
                            glEnableVertexAttribArray(index1);
                        }

                        glVertexAttribPointer( index1, 
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

        void SGL_DLLCALL Unbind() const
        {
            if ( device->CurrentVertexLayout() == this )
            {
                for (size_t i = 0; i<elements.size(); ++i) {
                    glDisableVertexAttribArray(elements[i].index);
                }

                device->SetVertexLayout(0);
            }
        }

    private:
        GLDevice<DeviceVersion>*    device;
	    std::vector<ELEMENT>	    elements;
    };

} // anonymous namespace 

namespace sgl {

template<DEVICE_VERSION DeviceVersion>
sgl::VertexLayout* sglCreateVertexLayout(GLDevice<DeviceVersion>*       device, 
                                         unsigned int                   numElements, 
                                         const VertexLayout::ELEMENT*   element)
{
    if ( !device_traits<DeviceVersion>::support_fixed_attributes() ) 
    {
#ifndef SGL_NO_STATUS_CHECK
        for (unsigned i = 0; i<numElements; ++i) 
        {
            if (element[i].semantic != VertexLayout::ATTRIBUTE) {
                throw gl_error("Device profile doesn't support fixed pipeline vertex attributes");
            }
        }
#endif

        return new GLVertexLayoutAttribute<DeviceVersion>(device, numElements, element);
    }
#ifndef SGL_NO_STATUS_CHECK
    else if ( !device_traits<DeviceVersion>::support_generic_attributes() )
    {
        for (unsigned i = 0; i<numElements; ++i) 
        {
            if (element[i].semantic == VertexLayout::ATTRIBUTE) {
                throw gl_error("Device profile doesn't support generic vertex attributes");
            }
        }
    }

#endif

    return new GLVertexLayoutMixed<DeviceVersion>(device, numElements, element);
}

// explicit template instantiation
template sgl::VertexLayout* sglCreateVertexLayout<DV_OPENGL_1_3>(GLDevice<DV_OPENGL_1_3>*, unsigned int, const VertexLayout::ELEMENT*);
template sgl::VertexLayout* sglCreateVertexLayout<DV_OPENGL_1_4>(GLDevice<DV_OPENGL_1_4>*, unsigned int, const VertexLayout::ELEMENT*);
template sgl::VertexLayout* sglCreateVertexLayout<DV_OPENGL_1_5>(GLDevice<DV_OPENGL_1_5>*, unsigned int, const VertexLayout::ELEMENT*);
template sgl::VertexLayout* sglCreateVertexLayout<DV_OPENGL_2_0>(GLDevice<DV_OPENGL_2_0>*, unsigned int, const VertexLayout::ELEMENT*);
template sgl::VertexLayout* sglCreateVertexLayout<DV_OPENGL_2_1>(GLDevice<DV_OPENGL_2_1>*, unsigned int, const VertexLayout::ELEMENT*);
template sgl::VertexLayout* sglCreateVertexLayout<DV_OPENGL_3_0>(GLDevice<DV_OPENGL_3_0>*, unsigned int, const VertexLayout::ELEMENT*);
template sgl::VertexLayout* sglCreateVertexLayout<DV_OPENGL_3_1>(GLDevice<DV_OPENGL_3_1>*, unsigned int, const VertexLayout::ELEMENT*);
template sgl::VertexLayout* sglCreateVertexLayout<DV_OPENGL_3_2>(GLDevice<DV_OPENGL_3_2>*, unsigned int, const VertexLayout::ELEMENT*);

} // namespace sgl