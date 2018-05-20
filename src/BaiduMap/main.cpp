#include "mainwindow.h"
#include <QApplication>
#include "GraphicsMap.h"
#include "MapSettingWidget.h"
#include "MainWnd.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    //GraphicsMap gmap;
    //gmap.resize(500,500);
    //gmap.show();

    //MapSettingWidget dlg;
    //dlg.show();
    MainWnd w;
    w.showMaximized();

    return a.exec();
}
