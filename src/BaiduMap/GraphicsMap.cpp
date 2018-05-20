#include "GraphicsMap.h"
#include <QLineF>
#include <QVector>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QDebug>
#include <GraphicsSceneMap.h>
#include <QGraphicsTextItem>

#include <QWheelEvent>
#include <QMouseEvent>

#include <QLabel>
#include <QGraphicsPixmapItem>

GraphicsMap::GraphicsMap(QWidget *parent )
    :QGraphicsView(parent)
{
    //this->setForegroundBrush(Qt::green);
    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setAcceptDrops(true);
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    m_scene = new GraphicsSceneMap(this);
    setScene(m_scene);
    this->centerOn(m_scene->getPicItem()->scenePos());
    QGraphicsRectItem* rectItem = m_scene->addRect(0,0,100,3,QPen(Qt::black),QBrush(QColor(255,0,0,255)));

    int mask =QGraphicsItem::ItemIsMovable
            |QGraphicsItem::ItemIsSelectable
            |QGraphicsItem::ItemIsFocusable
            |QGraphicsItem::ItemClipsToShape
            |QGraphicsItem::ItemClipsChildrenToShape;
    QGraphicsItem::GraphicsItemFlags allFlag;
    allFlag &=mask;
    //rectItem->setFlags(allFlag);
    rectItem->setFlag(QGraphicsItem::ItemIsMovable);
    rectItem->setFlag(QGraphicsItem::ItemIsSelectable);

    rectItem->setFocus(Qt::MouseFocusReason);

    m_xyPosition =  scene()->addText(QString());


    QLabel* pLabel = new QLabel("helloworld",this);
    pLabel->setStyleSheet("border:none;border-bottom:2px solid red;");
    pLabel->setGeometry(0,0,100,40);

}


void GraphicsMap::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void GraphicsMap::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}


void GraphicsMap::mouseDoubleClickEvent(QMouseEvent *event)
{
     QGraphicsView::mouseDoubleClickEvent(event);
}

void GraphicsMap::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsMap::wheelEvent(QWheelEvent *event)
{
    qDebug()<<"..de"<<event->delta();
    static int level = 18;
    int oldLevel = level;
    if(event->delta()>0)
    {
        level++;
    }
    else{
        level--;
    }
    level = qMin(18,qMax(0,level));

    if(oldLevel != level)
    {
        m_scene->setLevel(level);
        qreal dScale = 0.5;
        if(level>oldLevel){
             dScale = 2.0;
        }
        this->scale(dScale,dScale);
        QGraphicsView::wheelEvent(event);
    }




}

void GraphicsMap::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);



}

void GraphicsMap::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    //fitInView(sceneRect(), Qt::KeepAspectRatio);
}

void GraphicsMap::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug()<<".......dragEnterEvent";
    QGraphicsView::dragEnterEvent(event);

}

void GraphicsMap::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug()<<".......dragMoveEvent";
     QGraphicsView::dragMoveEvent(event);
        event->acceptProposedAction();
}

void GraphicsMap::dragLeaveEvent(QDragLeaveEvent *event)
{
     QGraphicsView::dragLeaveEvent(event);
}

void GraphicsMap::dropEvent(QDropEvent *event)
{
     qDebug()<<".......dropEvent";
     QGraphicsView::dropEvent(event);
}

void GraphicsMap::setupViewport(QWidget *viewport)
{
     QGraphicsView::setupViewport(viewport);
}

bool GraphicsMap::viewportEvent(QEvent *event)
{
     return QGraphicsView::viewportEvent(event);
     qDebug()<<"viewport size"<<this->viewport()->size();
}

void GraphicsMap::scrollContentsBy(int dx, int dy)
{
     QGraphicsView::scrollContentsBy(dx,dy);
     qDebug()<<"dx:"<<dx<<",dy:"<<dy;
}

void GraphicsMap::drawBackground(QPainter *painter, const QRectF &rect)
{
     QGraphicsView::drawBackground(painter,rect);

}

void GraphicsMap::drawForeground(QPainter *painter, const QRectF &rect)
{
     QGraphicsView::drawForeground(painter,rect);
}
