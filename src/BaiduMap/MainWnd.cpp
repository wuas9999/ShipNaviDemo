#include "MainWnd.h"
#include "ui_MainWnd.h"

MainWnd::MainWnd(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWnd)
{
    ui->setupUi(this);
    ui->buttonGroup->setId(ui->toolButton,QGraphicsView::ScrollHandDrag);
    ui->buttonGroup->setId(ui->toolButton_2,QGraphicsView::RubberBandDrag);
    connect(ui->buttonGroup,SIGNAL(buttonToggled(int , bool )),SLOT(OnButtonToggled(int , bool)));
    ui->graphicsView->setDragMode((QGraphicsView::DragMode) ui->buttonGroup->checkedId());


    //ui->graphicsView->setAcceptDrops();
}

MainWnd::~MainWnd()
{
    delete ui;
}

void MainWnd::OnButtonToggled(int id, bool checked)
{
    ui->graphicsView->setDragMode((QGraphicsView::DragMode)ui->buttonGroup->checkedId());
}
