#ifndef SIMPLE_GL_QT_GUI_GRAPHICS_GUI_CUBE_H
#define SIMPLE_GL_QT_GUI_GRAPHICS_GUI_CUBE_H

#include "Device.h"

class GUICube
{
public:
    GUICube(sgl::Device* device);

    void render(sgl::Device* device) const;

private:
    sgl::ref_ptr<sgl::VertexBuffer> vbo;
    sgl::ref_ptr<sgl::IndexBuffer>  ibo;
    sgl::ref_ptr<sgl::VertexLayout> vlayout;
};

#endif // SIMPLE_GL_QT_GUI_GRAPHICS_GUI_CUBE_H
