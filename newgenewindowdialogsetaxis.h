#ifndef NEWGENEWINDOWDIALOGSETAXIS_H
#define NEWGENEWINDOWDIALOGSETAXIS_H

#include <QMainWindow>
#include <curveparameterfornewgenedata.h>
#include <QDebug>

namespace Ui {
class NewGeneWindowDialogSetAxis;
}

class NewGeneWindowDialogSetAxis : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit NewGeneWindowDialogSetAxis(CurveParameterForNewGeneData CurveParameterForNewGeneRef,
                                        QWidget *parent = 0);
    ~NewGeneWindowDialogSetAxis();

signals:
   void sigReturnDialogResult(CurveParameterForNewGeneData CurveParameterForNewGeneRef);// 返回处理结果

    
private slots:
    void on_pushButtonOK_clicked();

    void on_pushButtonCancel_clicked();

    void slotKillMyself();

private:
    Ui::NewGeneWindowDialogSetAxis *ui;

    CurveParameterForNewGeneData CurveParameterForNewGene;//传过来的参数

};

#endif // NEWGENEWINDOWDIALOGSETAXIS_H
