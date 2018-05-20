#ifndef GRAPHICSSCENEMAP_H
#define GRAPHICSSCENEMAP_H

#include <QGraphicsScene>
class QGraphicsItem;
class GraphicsSceneMap:public QGraphicsScene
{
    Q_OBJECT
public:
    GraphicsSceneMap(QObject *parent = Q_NULLPTR);

    // QGraphicsScene interface
    int level() const;
    void setLevel(int level);
    QRectF getPointRect();
    QGraphicsPixmapItem* getPicItem();
protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);
private slots:
    void slotSceneRectChange(const QRectF& rect);
private:

    QList<QPointF> m_Points;
    QList<QGraphicsItem*> m_tempItems;
    QGraphicsLineItem* m_line;
    QList<QGraphicsItem*> m_modifyItems;
    QList<QGraphicsItem*> m_listPloygons;
    QGraphicsItemGroup* m_lastModifyGroup;
    double m_dCenterX;   //percentage, -0.5~0.5
    double m_dCenterY;   //percentage, -0.5~0.5
    int m_nLevel;        //0-18
    QGraphicsPixmapItem* m_mapPic;
    // QGraphicsScene interface
protected:
    void keyPressEvent(QKeyEvent *event);

    bool CV_LLA2DP(double lat,double lon,qint32 * pX,qint32 *pY);
    bool CV_DP2LLA(qint32 X,qint32 Y,double  * plat,double * plon);
    //Convert Merkator and LLA
    bool CV_MK2LLA(double mx, double my, double * plat, double * plon);
    bool CV_LLA2MK(double lat,double lon, double * pmx, double * pmy);
    //Convert Merkator and WorldPixel
    bool CV_MK2World(double mx, double my, double * px, double * py);
    bool CV_World2MK(double x,double y, double * pmx, double * pmy);
    //Convert LLA and World
    bool CV_LLA2World(double lat, double lon, double * px, double * py);
    bool CV_World2LLA(double x,double y, double * plat, double * plon);
    //Convert World and DP
    bool CV_DP2World(qint32 dX, qint32 dY, double * px, double * py);
    bool CV_World2DP(double x,double y, qint32 * dX, qint32 * dY);
    //cood convertion
    bool CV_Pct2World(double px,double py,double * nx,double * ny);
    bool CV_World2Pct(double nx,double ny,double * px,double * py) ;

    // QGraphicsScene interface
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
};

#endif // GRAPHICSSCENEMAP_H
