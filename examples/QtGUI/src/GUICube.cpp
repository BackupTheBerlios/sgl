#include "GUICube.h"
#include "Math/Containers.hpp"
#include <gl/glew.h>

GUICube::GUICube(sgl::Device* device)
{
    math::vector_of_vector3f    vertices;
    std::vector<unsigned short> indices;

    // Create vertices for box
    vertices.push_back( math::Vector3f(-1.0, -1.0,  1.0) );
    vertices.push_back( math::Vector3f(-1.0,  1.0,  1.0) );
    vertices.push_back( math::Vector3f( 1.0,  1.0,  1.0) );
    vertices.push_back( math::Vector3f( 1.0, -1.0,  1.0) );
    vertices.push_back( math::Vector3f(-1.0, -1.0, -1.0) );
    vertices.push_back( math::Vector3f(-1.0,  1.0, -1.0) );
    vertices.push_back( math::Vector3f( 1.0,  1.0, -1.0) );
    vertices.push_back( math::Vector3f( 1.0, -1.0, -1.0) );

    // Back face
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

    // Top face
    indices.push_back(1);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(2);

    // Front face
    indices.push_back(5);
    indices.push_back(4);
    indices.push_back(7);
    indices.push_back(6);

    // Bottom face
    indices.push_back(4);
    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(7);

    // Left face
    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(6);
    indices.push_back(7);

    // Right face
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(1);
    indices.push_back(0);

    vbo.reset( device->CreateVertexBuffer() );
    vbo->SetData(vertices.size() * sizeof(math::Vector3f), &vertices[0]);

    ibo.reset( device->CreateIndexBuffer() );
    ibo->SetData(indices.size() * sizeof(unsigned short), &indices[0]);

    // create vertex layout
    {
        sgl::VertexLayout::ELEMENT elements[] =
        {
            {0, 3, 0, 12, sgl::FLOAT, sgl::VertexLayout::VERTEX},
        };

        vlayout.reset( device->CreateVertexLayout(1, elements) );
    }
}

void GUICube::render(sgl::Device* device) const
{
    vbo->Bind( vlayout.get() );
    ibo->Bind(sgl::IndexBuffer::UINT_16);
    device->DrawIndexed(sgl::QUADS, 0, 24);
    ibo->Unbind();
    vbo->Unbind();
    vlayout->Unbind();
}
