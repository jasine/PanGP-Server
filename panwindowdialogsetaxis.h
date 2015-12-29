#ifndef PANWINDOWDIALOGSETAXIS_H
#define PANWINDOWDIALOGSETAXIS_H

#include <QMainWindow>
#include <curveparameterforpancoredata.h>
#include <QDebug>

namespace Ui {
class PanWindowDialogSetAxis;
}

class PanWindowDialogSetAxis : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit PanWindowDialogSetAxis(CurveParameterForPanCoreData CurveParameterForPanCoreRef
                                    , QWidget *parent = 0);
    ~PanWindowDialogSetAxis();

signals:
   void sigReturnDialogResult(CurveParameterForPanCoreData CurveParameterForPanCoreRef);// 返回处理结果


private slots:
    void slotKillMyself();//关掉自己
    
    void on_pushButtonCancel_clicked();

    void on_pushButtonOK_clicked();

private:
    Ui::PanWindowDialogSetAxis *ui;

    CurveParameterForPanCoreData CurveParameterForPanCore;//传过来的参数




};

#endif // PANWINDOWDIALOGSETAXIS_H
