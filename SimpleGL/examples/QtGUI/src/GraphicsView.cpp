#include "GraphicsView.h"
#include <QResizeEvent>

GraphicsView::GraphicsView()
{
    setWindowTitle(tr("SGL Qt example"));
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    if ( scene() ) {
        scene()->setSceneRect( QRect(QPoint(0, 0), event->size()) );
    }

    QGraphicsView::resizeEvent(event);
}
