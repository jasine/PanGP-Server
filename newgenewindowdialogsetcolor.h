#ifndef NEWGENEWINDOWDIALOGSETCOLOR_H
#define NEWGENEWINDOWDIALOGSETCOLOR_H

#include <QMainWindow>
#include <curveparameterfornewgenedata.h>
#include <QVector>
#include <QDebug>

namespace Ui {
class NewGeneWindowDialogSetColor;
}

class NewGeneWindowDialogSetColor : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit NewGeneWindowDialogSetColor(CurveParameterForNewGeneData CurveParameterForNewGeneRef,
                                         QWidget *parent = 0);
    ~NewGeneWindowDialogSetColor();

signals:
   void sigReturnDialogResult(CurveParameterForNewGeneData CurveParameterForNewGeneRef);// 返回处理结果
    
private slots:
    void on_pushButtonCancel_clicked();

    void on_pushButtonOK_clicked();

    void slotKillMyself();

private:
    Ui::NewGeneWindowDialogSetColor *ui;

    CurveParameterForNewGeneData CurveParameterForNewGene;

};

#endif // NEWGENEWINDOWDIALOGSETCOLOR_H
