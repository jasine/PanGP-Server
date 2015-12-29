#ifndef PANFITTINGWINDOW_H
#define PANFITTINGWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QMovie>
#include <fittingresultdata.h>


namespace Ui {
class PanFittingWindow;
}

class PanFittingWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit PanFittingWindow(FittingResultData FittingResultRef, QWidget *parent = 0);
    ~PanFittingWindow();



public slots:
    //关闭自己
    void slotKillMyself();
    
private:
    Ui::PanFittingWindow *ui;

    FittingResultData FittingResult; // 传递过来的参数


};

#endif // PANFITTINGWINDOW_H
