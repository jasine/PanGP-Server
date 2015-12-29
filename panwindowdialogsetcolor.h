#ifndef PANWINDOWDIALOGSETCOLOR_H
#define PANWINDOWDIALOGSETCOLOR_H

#include <QMainWindow>
#include <curveparameterforpancoredata.h>
#include <QVector>
#include <QDebug>

namespace Ui {
class PanWindowDialogSetColor;
}

class PanWindowDialogSetColor : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit PanWindowDialogSetColor(CurveParameterForPanCoreData CurveParameterForPanCoreRef, QWidget *parent = 0);
    ~PanWindowDialogSetColor();

signals:
   void sigReturnDialogResult(CurveParameterForPanCoreData CurveParameterForPanCoreRef);// 返回处理结果
    
private slots:
    void on_pushButtonCancel_clicked();

    void on_pushButtonOK_clicked();

    void slotKillMyself();

private:
    Ui::PanWindowDialogSetColor *ui;

    CurveParameterForPanCoreData CurveParameterForPanCore;


};

#endif // PANWINDOWDIALOGSETCOLOR_H
