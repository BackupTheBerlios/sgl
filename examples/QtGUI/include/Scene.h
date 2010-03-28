#ifndef SIMPLE_GL_QT_GUI_GRAPHICS_SCENE_H
#define SIMPLE_GL_QT_GUI_GRAPHICS_SCENE_H

#include <QGraphicsScene>
#include <QSharedPointer>
#include "GUICube.h"
#include "SGLWidget.h"

class Scene :
    public QGraphicsScene
{
public:
    Scene(SGLWidget* sglWidget);

    QDialog* createDialog(const QString &windowTitle) const;

private:
    void init();
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void drawBackground(QPainter *painter, const QRectF &rect);

private:
    sgl::Uniform4x4F*               projectionMatrixUniform;
    sgl::Uniform4x4F*               modelviewMatrixUniform;
    SGLWidget*                      sglWidget;
    QSharedPointer<GUICube>         guiCube;
    bool                            initialized;

    // default state set
    sgl::ref_ptr<sgl::RasterizerState>      wireframeState;
};

#endif // SIMPLE_GL_QT_GUI_GRAPHICS_SCENE_H
