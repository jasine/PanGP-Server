#ifndef NEWFITTINGWINDOW_H
#define NEWFITTINGWINDOW_H

#include <QMainWindow>

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QMovie>
#include <fittingresultdata.h>


namespace Ui {
class NewFittingWindow;
}

class NewFittingWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit NewFittingWindow(FittingResultData FittingResultRef, QWidget *parent = 0);
    ~NewFittingWindow();


public slots:
    //关闭自己
    void slotKillMyself();
    
private:
    Ui::NewFittingWindow *ui;

    FittingResultData FittingResult; // 传递过来的参数

};

#endif // NEWFITTINGWINDOW_H
