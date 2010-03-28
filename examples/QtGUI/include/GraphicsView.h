#ifndef SIMPLE_GL_QT_GUI_GRAPHICS_VIEW_H
#define SIMPLE_GL_QT_GUI_GRAPHICS_VIEW_H

#include <QGraphicsView>

class GraphicsView :
    public QGraphicsView
{
public:
    GraphicsView();

private:
    void resizeEvent(QResizeEvent *event);
};

#endif // SIMPLE_GL_QT_GUI_GRAPHICS_VIEW_H
