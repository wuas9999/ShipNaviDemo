#ifndef MAINWND_H
#define MAINWND_H

#include <QMainWindow>

namespace Ui {
class MainWnd;
}

class MainWnd : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWnd(QWidget *parent = 0);
    ~MainWnd();
private slots:
    void OnButtonToggled(int id, bool checked );
private:
    Ui::MainWnd *ui;
};

#endif // MAINWND_H
