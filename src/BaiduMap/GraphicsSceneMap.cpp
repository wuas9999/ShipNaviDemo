#include "GraphicsSceneMap.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QGraphicsView>
#include <QMenu>
#include <QAction>


#include "cProjectionMercator.h"
using namespace QTVOSM;

GraphicsSceneMap::GraphicsSceneMap(QObject *parent)
    :QGraphicsScene(parent),m_line(0),m_lastModifyGroup(0),m_nLevel(19)
{
    this->m_dCenterX = this->m_dCenterY = 0;
    setBackgroundBrush(QBrush(QPixmap(":/res/timg.jpg")));
    connect(this,SIGNAL(sceneRectChanged(const QRectF&)) ,SLOT(slotSceneRectChange(const QRectF&)));
    //setForegroundBrush(QColor(255, 255, 255, 50));

    // a grid foreground
    //setForegroundBrush(QBrush(Qt::lightGray, Qt::CrossPattern));
    QRect rectScene = QRect(0,0,256*(1<<19),256*(1<<19));
    this->setSceneRect(rectScene);



    //左下角:118.704528808594,32.258104107131
    //左上角:118.704528808594,32.263910555201
    //右上角:118.712081909180,32.263910555201
    //右下角:118.712081909180,32.258104107131
    QRectF rectPix;

    qint32 x,y;
    double la,lo;
    //CV_MK2LLA(118.704528808594,32.263910555201,&la, &lo);
    this->CV_LLA2DP(32.263910555201,118.704528808594,&x,&y);
    rectPix.setTopLeft(QPointF(x,y));
    //CV_MK2LLA(118.712081909180,32.258104107131,&la, &lo);
    this->CV_LLA2DP(32.258104107131,118.712081909180,&x,&y);
    rectPix.setBottomRight(QPointF(x,y));
    QPixmap pix(tr(":/res/水库1_180124120328.png"));
    m_mapPic = this->addPixmap(pix);
    m_mapPic->setPos(rectPix.topLeft());
    qDebug()<<"rectPix"<<rectPix;
}


void GraphicsSceneMap::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    if(mimeData->hasImage())
    {
        qDebug()<<"hasImage";
        //QImage image = qvariant_cast<QImage>(mimeData->imageData());

    }
    else if(mimeData->hasText())
    {
        qDebug()<<"hasText"<<mimeData->text();

    }
    event->acceptProposedAction();
    //QGraphicsScene::dragEnterEvent(event);


}

void GraphicsSceneMap::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsScene::dragMoveEvent(event);

}

void GraphicsSceneMap::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsScene::dragLeaveEvent(event);
}

void GraphicsSceneMap::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsScene::dropEvent(event);
    qDebug()<<"dropEvent........";


    const QMimeData* mimeData = event->mimeData();
    if(mimeData->hasText())
    {
        qDebug()<<"hasText"<<mimeData->text();

        QString filePath = QUrl(mimeData->text()).toLocalFile();
        QPixmap pix(filePath);
        QGraphicsPixmapItem * pixmapItem = this->addPixmap(pix);
        pixmapItem->setFlag(QGraphicsItem::ItemIsMovable);
        pixmapItem->setFlag(QGraphicsItem::ItemIsSelectable);
        pixmapItem->setPos(event->pos());
        this->views().first()->centerOn(pixmapItem);
    }
}

void GraphicsSceneMap::focusInEvent(QFocusEvent *event)
{
    QGraphicsScene::focusInEvent(event);
}

void GraphicsSceneMap::focusOutEvent(QFocusEvent *event)
{
    QGraphicsScene::focusOutEvent(event);
}

void GraphicsSceneMap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    qDebug()<<"last pos"<<event->lastPos();
    qDebug()<<"lastScenePos"<<event->lastScenePos();
    qDebug()<<"lastScreenPos"<<event->lastScreenPos();
    //    qDebug()<<"buttonDownPos"<<event->buttonDownPos(Qt::LeftButton);
    //    qDebug()<<"buttonDownScenePos"<<event->buttonDownScenePos(Qt::LeftButton);
    //    qDebug()<<"buttonDownScreenPos"<<event->buttonDownScreenPos(Qt::LeftButton);


    double tlat, tlon;
    //CV_DP2LLA(event->pos().x(),event->pos().y(),&tlat,&tlon);
    //CV_DP2LLA(event->lastScenePos().x(),event->lastScenePos().y(),&tlat,&tlon);
    //qDebug()<<tlat<<tlon;


    if(event->modifiers().testFlag(Qt::ControlModifier))
    {
        QPointF newPos = event->lastScenePos();
        if(event->button() == Qt::LeftButton)
        {
            QRectF pointRect = getPointRect();
            pointRect.moveCenter(newPos);

            if(m_Points.size())
            {
                QLineF lineF(m_Points.last(),newPos );
                QGraphicsLineItem* lineitem = this->addLine(lineF,QPen(Qt::green));
                m_tempItems.push_back(lineitem);
            }
            m_Points.push_back(newPos);
            QGraphicsEllipseItem* item =  this->addEllipse(pointRect,QPen(Qt::green),QBrush(Qt::transparent));
            m_tempItems.push_back(item);
        }

    }
    else if(event->modifiers().testFlag(Qt::NoModifier))
    {
        if(event->button() == Qt::RightButton)
        {
           if(m_Points.size()>2)
                {
                    QLineF lineF(m_Points.last(),m_Points.first());
                    QGraphicsLineItem* lineitem = this->addLine(lineF,QPen(Qt::green));
                    m_tempItems.push_back(lineitem);
                    QPolygonF polygonF(m_Points.toVector());
                    QGraphicsPolygonItem* polygonItem =  this->addPolygon(polygonF,QPen(Qt::green),QBrush(QColor(0,0,255,50)));
                    polygonItem->setFlag(QGraphicsItem::ItemIsMovable);
                    polygonItem->setFlag(QGraphicsItem::ItemIsSelectable);
                    polygonItem->setFocus(Qt::MouseFocusReason);
                    m_listPloygons.push_back(polygonItem);
                    //qDebug()<<"......polygonItem scenePos:"<<polygonItem->scenePos();
                    foreach(QGraphicsItem* item,m_tempItems)
                    this->removeItem(item);
                    m_tempItems.clear();
                    m_Points.clear();
                }
        }
        else if(event->button() == Qt::LeftButton)
        {

            if(m_lastModifyGroup)
            {
                QPointF newPos = event->lastScenePos();
                QList<QGraphicsItem*> items =  this->items(newPos);
                if(items.size()&&m_modifyItems.contains(items.first()))
                {
                    QGraphicsEllipseItem* pointItem = (QGraphicsEllipseItem*)(items.first());
                    m_lastModifyGroup->removeFromGroup(pointItem);
                    pointItem->setBrush(Qt::red);
                }
                else
                {
                    foreach(QGraphicsItem* item,m_modifyItems)
                    {
                        this->removeItem(item);
                    }
                    m_modifyItems.clear();
                    this->destroyItemGroup(m_lastModifyGroup);
                    m_lastModifyGroup = 0;
                }



            }
        }

    }
}

void GraphicsSceneMap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);

    double tlat, tlon;
    //CV_DP2LLA(event->pos().x(),event->pos().y(),&tlat,&tlon);
    CV_DP2LLA(event->lastScenePos().x(),event->lastScenePos().y(),&tlat,&tlon);
    qDebug()<<"scenePos:"<<event->lastScenePos()<<"lat,lon:"<<tlat<<tlon;


    this->views().first();
    //QPainter painter;
    //QRectF rect(0,0,60,60);
    //rect.moveCenter(event->lastScenePos());
    //painter.fillRect(rect,QBrush(Qt::red));
    if(event->modifiers().testFlag(Qt::ControlModifier))
    {
        if(m_Points.size())
        {

            QPointF lastPos = m_Points.last();
            QPointF newPos = event->lastScenePos();
            QLineF lineF(lastPos,newPos);
            if(!m_line)
            {
                m_line = this->addLine(lineF,QPen(Qt::yellow));
            }
            else
            {
                m_line->setLine(lineF);
            }
            m_line->setVisible(true);
        }

    }
    else if(event->modifiers().testFlag(Qt::NoModifier))
    {
        if(m_line)    m_line->setVisible(false);

        if(m_lastModifyGroup)
        {
            QList<QGraphicsItem *> items  = this->selectedItems();
            bool bAdjust = false;
            foreach (QGraphicsItem* item, items) {
                if(m_modifyItems.contains(item))
                {
                    bAdjust = true;
                    QVector<QPointF> ptList;
                    QGraphicsPolygonItem* polyGenItem = (QGraphicsPolygonItem*)(m_lastModifyGroup->parentItem());
                    foreach(QGraphicsItem* itemPoint,m_modifyItems)
                    {
                        QGraphicsEllipseItem* elipItem = (QGraphicsEllipseItem*)itemPoint;
                        QPointF pt = elipItem->rect().center();
                        pt = elipItem->mapToScene(pt);
                        ptList.push_back(pt);
                    }

                    QPolygonF polygonF =  polyGenItem->mapFromScene(ptList);
                    polyGenItem->setPolygon(polygonF);
                    break;
                }
            }

            //??
            if(!bAdjust)
            {
                QGraphicsPolygonItem* polyGenItem = (QGraphicsPolygonItem*)(m_lastModifyGroup->parentItem());

            }




        }


    }

}

void GraphicsSceneMap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);

}

void GraphicsSceneMap::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);

    QPointF newPos = event->lastScenePos();
    if(event->button() == Qt::LeftButton)
    {
        QList<QGraphicsItem *> items =  this->items(newPos,Qt::IntersectsItemShape);
        if(items.size()&&items.first()->type() ==QGraphicsPolygonItem::Type)
        {
            QGraphicsPolygonItem* polygenItem = (QGraphicsPolygonItem*)(items.first());
            qDebug()<<"ori"<<polygenItem->polygon();
            QPolygonF polygenF =  polygenItem->mapToScene(polygenItem->polygon());
            qDebug()<<"map"<<polygenF;
            QGraphicsItemGroup* groupItem = new QGraphicsItemGroup(polygenItem);
             m_lastModifyGroup = groupItem;
            foreach(QPointF pt,polygenF)
            {
                QRectF rectF = getPointRect();
                rectF.moveCenter(pt);
                QGraphicsEllipseItem* pointItem = this->addEllipse(rectF,QPen(Qt::red),QBrush(QColor(0,255,255,60)));
                pointItem->setFlag(QGraphicsItem::ItemIsMovable);
                pointItem->setFlag(QGraphicsItem::ItemIsSelectable);
                pointItem->setFocus(Qt::MouseFocusReason);
                m_modifyItems.push_back(pointItem);
                groupItem->addToGroup(pointItem);
            }
        }

    }

}

void GraphicsSceneMap::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QGraphicsScene::wheelEvent(event);
}

void GraphicsSceneMap::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter,rect);
}

void GraphicsSceneMap::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawForeground(painter,rect);




    //qDebug()<<rect;
//    painter->save();
//    QVector<QLineF> lines;

//    const double xpadding =100.0f;
//    const double ypadding =100.0f;
//    for(double dx = 3.0f;dx<rect.width();dx+=xpadding)
//    {
//       double ddx =  rect.left()+dx;
//       lines<<QLineF(ddx,rect.top(),ddx,rect.bottom());
//    }
//    for(double dy =3.0f;dy<rect.height();dy+=ypadding)
//    {
//       double ddy =  rect.top()+dy;
//       lines<<QLineF(rect.left(),ddy,rect.right(),ddy);
//    }

//    painter->setPen(Qt::gray);

//    painter->drawLines(lines);
//    painter->restore();


}

void GraphicsSceneMap::slotSceneRectChange(const QRectF &rect)
{
    qDebug()<<"slotSceneRectChange:"<<rect;
}

int GraphicsSceneMap::level() const
{
    return m_nLevel;
}

void GraphicsSceneMap::setLevel(int level)
{
    //m_nLevel = level;
    //QRect rectScene = QRect(0,0,256*(1<<level),256*(1<<level));
    qDebug()<<"level"<<level;
    //setSceneRect(rectScene);

}

QRectF GraphicsSceneMap::getPointRect()
{
    int scaled = (1<<m_nLevel);
    QRectF rectF(0,0,20*scaled,20*scaled);
    return rectF;
}

QGraphicsPixmapItem *GraphicsSceneMap::getPicItem()
{
    return m_mapPic;
}




void GraphicsSceneMap::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete)
    {
        if(m_lastModifyGroup && m_modifyItems.size()>3)
        {
            QGraphicsPolygonItem* polyGenItem = (QGraphicsPolygonItem*)(m_lastModifyGroup->parentItem());
            QVector<QPointF> ptList;
            auto itr = m_modifyItems.begin();
            for(;itr != m_modifyItems.end();)
            {
                if((*itr)->isSelected())
                {
                    this->removeItem((*itr));
                    itr = m_modifyItems.erase(itr);
                }
                else
                {
                    QGraphicsEllipseItem* elipItem = (QGraphicsEllipseItem*)(*itr);
                    QPointF pt = elipItem->rect().center();
                    pt = elipItem->mapToScene(pt);
                    ptList.push_back(pt);
                    itr++;
                }
            }

            QPolygonF polygonF =  polyGenItem->mapFromScene(ptList);
            polyGenItem->setPolygon(polygonF);


        }

        QList<QGraphicsItem*> items = this->selectedItems();
        foreach(QGraphicsItem* item,items)
        {
            if(item->isSelected())
            {
                int index = m_listPloygons.indexOf(item);
                if(index != -1)
                {
                    this->removeItem(m_listPloygons.at(index));
                    m_listPloygons.removeAt(index);

                }
            }
        }



    }

}

enum MapAction{
    Z_Action,
    J_Action
};

void GraphicsSceneMap::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QPointF pt = event->scenePos();
    QGraphicsItem* item =  this->itemAt(pt,QTransform());
    if(item&&item->type() == QGraphicsPolygonItem::Type)
    {
        QGraphicsPolygonItem* polyItemn = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
        QMenu menu;
        QAction * pMenu =menu.addAction(tr("生成z线"));
        pMenu->setData(Z_Action);
        pMenu = menu.addAction(tr("生成#线"));
        pMenu->setData(J_Action);
        QAction* action = menu.exec(QCursor::pos());
        if(action)
        {
            int insType = action->data().toInt();
            if(insType == Z_Action
                    ||insType == J_Action)
            {
                const qreal offset =10;
                QPolygonF ploygon = polyItemn->polygon();
                QRectF rect = ploygon.boundingRect();
                QList<QLineF> listInstect;
                for(int y = rect.top()+offset;y<rect.bottom()-offset;y+=50)
                {
                    QLineF lf(QPointF(rect.left(),y),QPointF(rect.right(),y));
                    //lf.intersect()
                    //QLineF insLine;
                    QPointF ptLine;
                    int iInterTime =0;
                    for(int i =0;i<ploygon.size();i++)
                    {

                        QLineF elide = QLineF(ploygon.at(i),ploygon.at((i+1)%ploygon.size()));
                        QPointF insPt;
                        if(QLineF::BoundedIntersection  == lf.intersect(elide,&insPt))
                        {
                            iInterTime++;
                            if(iInterTime ==1)
                            {
                                ptLine = insPt;
                                //insPt.rx()+=offset;
                                //insLine.setP1(insPt);
                            }
                            else if(iInterTime ==2)
                            {

                                if(insPt.x()>ptLine.x())
                                {
                                    insPt.rx()-=offset;
                                    ptLine.rx()+=offset;
                                    listInstect.push_back(QLineF(ptLine,insPt));
                                }
                                else
                                {
                                    insPt.rx()+=offset;
                                    ptLine.rx()-=offset;
                                    listInstect.push_back(QLineF(insPt,ptLine));
                                }


                                break;
                            }
                        }
                    }
                }


                if(listInstect.size()>1)
                {
                    QPainterPath path;
                    for(int i =0;i<listInstect.size();i++)
                    {
                        QLineF lftmp =listInstect.at(i);
                        if(i==0)
                        {
                            path.moveTo(lftmp.p1());
                        }
                        else
                        {
                            if(insType == Z_Action)
                            {
                                path.lineTo(lftmp.p1());
                            }
                            else
                            {
                                if(i%2 == 0)
                                {
                                    path.lineTo(lftmp.p1());
                                }
                                else
                                {
                                    path.lineTo(lftmp.p2());
                                }
                            }

                        }
                        if(insType == Z_Action)
                        {
                            path.lineTo(lftmp.p2());
                        }
                        else
                        {
                            if(i%2 == 0)
                            {
                                path.lineTo(lftmp.p2());
                            }
                            else
                            {
                                path.lineTo(lftmp.p1());
                            }
                        }

                    }
                    this->addPath(path);


                }



            }
        }

    }

}


/*!
 \brief convert LLA to Device Points. Device Points is according to current viewport,
 point(0,0) stay at the top-left, point (width-1,height-1) in bottom-right
 This approach is devided into several steps, and it is LEVEL RELATED!

 \fn tilesviewer::CV_LLA2DP
 \param lat	latitude in degree
 \param lon	longitude in degree
 \param pX	the pointer to hold X cood of DP
 \param pY	the pointer to hold Y cood of DP
 \return bool	will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_LLA2DP(double lat,double lon,qint32 * pX,qint32 *pY)
{
    if (!pX||!pY)			return false;

    //!1.To Mercator Projection
    double dMx = cProjectionMercator(lat,lon).ToMercator().m_x;
    double dMy = cProjectionMercator(lat,lon).ToMercator().m_y;

    //!2.Calculat the percentage pos (-0.5-0.5) of the Mercator point
    double dperx = dMx/(cProjectionMercator::pi*cProjectionMercator::R*2);
    double dpery = -dMy/(cProjectionMercator::pi*cProjectionMercator::R*2);

    int nCurrImgSize = (1<<m_nLevel)*256;
    //!3.Calculat the World pixel coordinats
    double dTarX = dperx * nCurrImgSize + nCurrImgSize/2;
    double dTarY = dpery * nCurrImgSize + nCurrImgSize/2;
    //!4.Calculat the World pixel coordinats of current view-center point
    double dCurrX = nCurrImgSize*m_dCenterX+nCurrImgSize/2;
    double dCurrY = nCurrImgSize*m_dCenterY+nCurrImgSize/2;
    //!5.Calculat the World pixel coordinats of current view-left-top point
    double nOffsetLT_x = (dCurrX-width()/2.0);
    double nOffsetLT_y = (dCurrY-height()/2.0);

    //!6.Turn world coord to DP coord
    *pX = dTarX - nOffsetLT_x+.5;
    *pY = dTarY - nOffsetLT_y+.5;
    return true;
}
/*!
 \brief convert  Device Points to LLA. Device Points is according to current viewport,
 point(0,0) stay at the top-left, point (width-1,height-1) in bottom-right
 This approach is devided into several steps, and it is LEVEL RELATED!

 \fn tilesviewer::CV_DP2LLA
 \param X	 X cood of DP
 \param Y	 Y cood of DP
 \param plat	the pointer to hold lat in degree
 \param plon	the pointer to hold lon in degree
 \return bool	will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_DP2LLA(qint32 X,qint32 Y,double  * plat,double * plon)
{
    if (!plat||!plon)
        return false;

    //!1.Current World Pixel Size, connected to nLevel
    int nCurrImgSize = (1<<m_nLevel)*256;
    //!2.current DP according to center
    double dx = X-(width()/2.0);
    double dy = Y-(height()/2.0);
    //!3.Percentage -0.5 ~ 0.5 coord
    double dImgX = dx/nCurrImgSize+m_dCenterX;
    double dImgY = dy/nCurrImgSize+m_dCenterY;
    //!4.to Mercator
    double Mercator_x = cProjectionMercator::pi*cProjectionMercator::R*2*dImgX;
    double Mercator_y = -cProjectionMercator::pi*cProjectionMercator::R*2*dImgY;
    *plat = cProjectionMercator(Mercator_y,Mercator_x).ToLatLon().m_lat;
    *plon = cProjectionMercator(Mercator_y,Mercator_x).ToLatLon().m_lon;
    return true;
}

/*!
 \brief convert Mercator to LLA.Mercator coord is a projection with ID 900913.
 this method is NOT Level releated.

 \fn tilesviewer::CV_MK2LLA
 \param mx	Mercator x coord in meters
 \param my	Mercator y coord in meters
 \param plat	the pointer to hold lat in degree
 \param plon	the pointer to hold lon in degree
 \return bool	will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_MK2LLA(double mx, double my, double * plat, double * plon)
{
    if (!plat||!plon)			return false;
    *plat  = cProjectionMercator(my,mx).ToLatLon().m_lat;
    *plon  = cProjectionMercator(my,mx).ToLatLon().m_lon;
    return true;
}

/*!
 \brief convert LLA to Mercator .Mercator coord is a projection with ID 900913.
 this method is NOT Level releated.

 \fn tilesviewer::CV_LLA2MK
 \param lat	latitude in degree
 \param lon	longitude in degree
 \param pmx	the pointer to hold Mercator x coord in meters
 \param pmy	the pointer to hold Mercator y coord in meters
 \return bool	will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_LLA2MK(double lat,double lon, double * pmx, double * pmy)
{
    if (!pmx||!pmy)			return false;
    //To Mercator Projection
    *pmx = cProjectionMercator(lat,lon).ToMercator().m_x;
    *pmy = cProjectionMercator(lat,lon).ToMercator().m_y;
    return true;
}


/*!
 \brief convert Mercator to World. World Points is according to current level,
 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right
 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
 This approach is devided into several steps, and it is LEVEL RELATED!

 \fn tilesviewer::CV_MK2World
 \param mx	Mercator x in meters
 \param my	Mercator y in meters
 \param px	pointer to hold world x in pixel
 \param py	pointer to hold world y in pixel
 \return bool will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_MK2World(double mx, double my, double * px, double * py)
{
    if (!px||!py)			return false;
    //!1.Calculat the percentage pos (-0.5-0.5) of the Mercator point
    double dperx = mx/(cProjectionMercator::pi*cProjectionMercator::R*2);
    double dpery = -my/(cProjectionMercator::pi*cProjectionMercator::R*2);

    int nCurrImgSize = (1<<m_nLevel)*256;

    //!2.Calculat the World pixel coordinats
    *px = dperx * nCurrImgSize + nCurrImgSize/2;
    *py = dpery * nCurrImgSize + nCurrImgSize/2;

    return true;
}

/*!
 \brief convert World to  Mercator. World Points is according to current level,
 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right
 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
 This approach is devided into several steps, and it is LEVEL RELATED!

 \fn tilesviewer::CV_World2MK
 \param x world x in pixel
 \param y world y in pixel
 \param pmx pointer to hold Mercator x in meters
 \param pmy pointer to hold Mercator y in meters
 \return bool  will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_World2MK(double x,double y, double * pmx, double * pmy)
{
    if (!pmx||!pmy)			return false;
    //!1.Current World Pixel Size, connected to nLevel
    int nCurrImgSize = (1<<m_nLevel)*256;
    //!2.Percentage -0.5 ~ 0.5 coord
    double dImgX = x/nCurrImgSize - .5;
    double dImgY = y/nCurrImgSize - .5;
    //!3.to Mercator
    *pmx = cProjectionMercator::pi*cProjectionMercator::R*2*dImgX;
    *pmy = -cProjectionMercator::pi*cProjectionMercator::R*2*dImgY;
    return true;
}


/*!
 \brief  convert LLA to world. World Points is according to current level,
 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right
 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
 This approach is devided into several steps, and it is LEVEL RELATED!

 \fn tilesviewer::CV_LLA2World
 \param lat		the latitude in meter
 \param lon		the longitude in meter
 \param px	pointer to hold world x in pixel
 \param py	pointer to hold world y in pixel
 \return bool	 will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_LLA2World(double lat, double lon, double * px, double * py)
{
    if (!px||!py)			return false;
    //!1.To Mercator Projection
    double dMx = cProjectionMercator(lat,lon).ToMercator().m_x;
    double dMy = cProjectionMercator(lat,lon).ToMercator().m_y;

    //!2.Calculat the percentage pos (-0.5-0.5) of the Mercator point
    double dperx = dMx/(cProjectionMercator::pi*cProjectionMercator::R*2);
    double dpery = -dMy/(cProjectionMercator::pi*cProjectionMercator::R*2);

    int nCurrImgSize = (1<<m_nLevel)*256;
    //!3.Calculat the World pixel coordinats
    *px = dperx * nCurrImgSize + nCurrImgSize/2;
    *py = dpery * nCurrImgSize + nCurrImgSize/2;
    return true;
}

/*!
 \brief  convert world to LLA. World Points is according to current level,
 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right
 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
 This approach is devided into several steps, and it is LEVEL RELATED!

 \fn tilesviewer::CV_World2LLA
 \param x	world x coord in pixels
 \param y	world y coord in pixels
 \param plat	pointer to hold latitude in degree
 \param plon	pointer to hold longitude in degree
 \return bool	will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_World2LLA(double x,double y, double * plat, double * plon)
{
    if (!plat||!plon)			return false;
    //!1.Current World Pixel Size, connected to nLevel
    int nCurrImgSize = (1<<m_nLevel)*256;
    //!2.Percentage -0.5 ~ 0.5 coord
    double dImgX = x/nCurrImgSize - .5;
    double dImgY = y/nCurrImgSize - .5;
    //!3.to Mercator
    double mkx = cProjectionMercator::pi*cProjectionMercator::R*2*dImgX;
    double mky = -cProjectionMercator::pi*cProjectionMercator::R*2*dImgY;
    //!4.to LLA
    *plat = cProjectionMercator(mky,mkx).ToLatLon().m_lat;
    *plon = cProjectionMercator(mky,mkx).ToLatLon().m_lon;

    return true;
}

/*!
 \brief	 convert  Device Points to World. Device Points is according to current viewport,
 point(0,0) stay at the top-left, point (width-1,height-1) in bottom-right. World Points is according
 to current level,	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right,
 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
 This approach is devided into several steps, and it is LEVEL RELATED!

 \fn tilesviewer::CV_DP2World
 \param dX	Device point X in pixel
 \param dY	Device point Y in pixel
 \param px	pointer to hold world x in pixel
 \param py	pointer to hold world y in pixel
 \return bool	will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_DP2World(qint32 dX, qint32 dY, double * px, double * py)
{
    if (!px||!py)			return false;
    //!1.Current World Pixel Size, connected to nLevel
    int nCurrImgSize = (1<<m_nLevel)*256;
    //!2.current DP according to center
    double dx = dX-(width()/2.0);
    double dy = dY-(height()/2.0);
    //!3.Percentage -0.5 ~ 0.5 coord
    double dImgX = dx/nCurrImgSize+m_dCenterX;
    double dImgY = dy/nCurrImgSize+m_dCenterY;
    //!4.Calculat the World pixel coordinats
    *px = dImgX * nCurrImgSize + nCurrImgSize/2;
    *py = dImgY * nCurrImgSize + nCurrImgSize/2;
    return true;

}

/*!
 \brief	 convert  World to Device Points. Device Points is according to current viewport,
 point(0,0) stay at the top-left, point (width-1,height-1) in bottom-right. World Points is according
 to current level,	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right,
 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
 This approach is devided into several steps, and it is LEVEL RELATED!

 \fn tilesviewer::CV_World2DP
 \param x	world x in pixel
 \param y	world y in pixel
 \param pdX	pointer to hold Device point X in pixel
 \param pdY	pointer to hold Device point Y in pixel
 \return bool	will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_World2DP(double x,double y, qint32 * pdX, qint32 * pdY)
{
    if (!pdX||!pdY)			return false;

    //!1.Current World Pixel Size, connected to nLevel
    int nCurrImgSize = (1<<m_nLevel)*256;

    //!2.Calculat the World pixel coordinats of current view-center point
    double dCurrX = nCurrImgSize*m_dCenterX+nCurrImgSize/2;
    double dCurrY = nCurrImgSize*m_dCenterY+nCurrImgSize/2;
    //!3.Calculat the World pixel coordinats of current view-left-top point
    double nOffsetLT_x = (dCurrX-width()/2.0);
    double nOffsetLT_y = (dCurrY-height()/2.0);

    //!4.Turn world coord to DP coord
    *pdX = x - nOffsetLT_x+.5;
    *pdY = y - nOffsetLT_y+.5;
    return true;

}
/*!
 \brief	 convert  percentage coord to world. World Points is according
 to current level,	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right,
 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18. Percentage coord is a
 level-unretated coord, take a range -0.5~0.5, the world center in 0,0, -0.5.-0.5
 at top-left, 0.5,0.5 at bottom-right
 This approach is LEVEL RELATED!

 \fn tilesviewer::CV_Pct2World
 \param px	percentage coord x
 \param py	percentage coord y
 \param nx	pointer to hold world x in pixel
 \param ny	pointer to hold world y in pixel
 \return bool	will always return true except for the pointer is NULL
*/
bool GraphicsSceneMap::CV_Pct2World(double px,double py,double * nx,double * ny)
{
    if (!nx || !ny)
        return false;
    //first, determine whole map size in current level
    int sz_whole_idx = 1<<m_nLevel;
    int sz_whole_size = sz_whole_idx*256;
    //calculate pix coodinats
    *nx = px * sz_whole_size+sz_whole_size/2;
    *ny = py * sz_whole_size+sz_whole_size/2;
    return true;
}
/*!
 \brief	 convert world to   percentage coord. World Points is according
 to current level,	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right,
 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18. Percentage coord is a
 level-unretated coord, take a range -0.5~0.5, the world center in 0,0, -0.5.-0.5
 at top-left, 0.5,0.5 at bottom-right
 This approach is LEVEL RELATED!

 \fn tilesviewer::CV_World2Pct
 \param nx	world x in pixel
 \param ny	world y in pixel
 \param px	pointer to hold percentage coord x
 \param py	pointer to hold percentage coord y
 \return bool	will always return true except for the pointer is NULL
*/
bool  GraphicsSceneMap::CV_World2Pct(double nx,double ny,double * px,double * py)
{
    if (!px || !py)
        return false;
    //Current World Pixel Size, connected to nLevel
    int nCurrImgSize = (1<<m_nLevel)*256;
    //Percentage -0.5 ~ 0.5 coord
    *px = nx/nCurrImgSize - .5;
    *py = ny/nCurrImgSize - .5;

    return true;
}








