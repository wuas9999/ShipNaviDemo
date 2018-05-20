#include "MapSettingWidget.h"
#include "ui_MapSettingWidget.h"
#include <QFileDialog>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>

MapSettingWidget::MapSettingWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MapSettingWidget)
{
    ui->setupUi(this);
    ui->filePath_location->installEventFilter(this);
    ui->filePath_picture->installEventFilter(this);
}

MapSettingWidget::~MapSettingWidget()
{
    delete ui;
}

void MapSettingWidget::on_btn_picture_clicked()
{
    QString  filePath = QFileDialog::getOpenFileName(this,tr("打开图片文件"),QString(),tr("图片文件(*.png *.jpg *.jpeg *.bmp)"));
    if(!filePath.isEmpty())
    {
        ui->filePath_picture->setText(filePath);
    }
}

void MapSettingWidget::on_btn_location_clicked()
{
    QString  filePath = QFileDialog::getOpenFileName(this);
    if(!filePath.isEmpty())
    {
        ui->filePath_location->setText(filePath);
    }
}


bool MapSettingWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched ==  ui->filePath_location)
    {
        if(event->type() == QEvent::DragEnter)
        {
            QDragEnterEvent* dropEvent = (QDragEnterEvent*)event;
            const QMimeData* mineData =  dropEvent->mimeData();
            if(mineData->hasImage())
            {
                mineData->imageData();
            }
            else if(mineData->hasText())
            {
                qDebug()<<mineData->text();
            }
            return true;
        }
        return false;
    }
    else if(watched == ui->filePath_picture)
    {
        //qDebug()<<"type"<<event->type();
        if(event->type() == QEvent::DragEnter)
        {
            QDragEnterEvent* dropEvent = (QDragEnterEvent*)event;
            const QMimeData* mineData =  dropEvent->mimeData();
            qDebug()<<"formats"<<mineData->formats();
            if(mineData->hasImage())
            {
                mineData->imageData();
            }
            else if(mineData->hasText())
            {
                qDebug()<<QUrl(mineData->text()).toLocalFile();
                //ui->filePath_picture->setText(QUrl(mineData->text()).toLocalFile());
            }
            dropEvent->acceptProposedAction();
            return true;
        }
        else if(event->type() == QEvent::Drop)
        {
            QDropEvent* dropEvent = (QDropEvent*)event;
            const QMimeData* mineData =  dropEvent->mimeData();
            if(mineData->hasText())
            {
                //qDebug()<<QUrl(mineData->text()).toLocalFile();
                ui->filePath_picture->setText(QUrl(mineData->text()).toLocalFile());
                event->accept();
            }
            return true;
        }
        return false;
    }

    return QMainWindow::eventFilter(watched,event);
}
