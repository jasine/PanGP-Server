#ifndef NEWGENEWINDOW_H
#define NEWGENEWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPoint>
#include <QPainter>
#include <panprofiledata.h>
#include <curveparameterfornewgenedata.h>
#include <QAction>
#include <QMouseEvent>
#include <QCursor>
#include <QMessageBox>
#include <QFileDialog>
#include <newgenewindowdialogsetaxis.h>
#include <newgenewindowdialogsetcolor.h>
#include <QImageWriter>

namespace Ui {
class newGeneWindow;
}

class newGeneWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit newGeneWindow(QImage NewGeneCurveImageRef, CurveParameterForNewGeneData CurveParameterForNewGeneRef,
                           QWidget *parent = 0);
    ~newGeneWindow();

signals:
    void sigRequestUpdateImage(CurveParameterForNewGeneData CurveParameterForNewGeneRef);//请求更新图像
    void sigToKillAxisDialogWindow();// 关闭坐标轴参数修改窗口
    void sigToKillColorDialogWindow();// 关闭坐标轴参数修改窗口
    void sigSavePanGenomeData(); //保存新基因的数据
    void sigOpenAboutWindow(); // 打开about 窗口
    void sigDrawNewGeneCurve();// 画新基因的曲线
    void sigFitNewGene(qint32 FitType);//拟合泛基因组和核心基因组,参数含义FitType=1,拟合core and pan, =0,拟合New Gene
    void sigTellMaiWindowIAmClosed();//通知主窗口，自己已经被关闭了

public slots:
    void slotKillMyself(); //关闭自己
    void slotUpdateImage(QImage NewGeneCurveImageRef);//更新图像
    void slotReceiveDialog(CurveParameterForNewGeneData CurveParameterForNewGeneRef);//接受窗体Edit项目更新图像方面的请求



    
private slots:
    void on_actionSet_Axis_Feature_triggered();

    void on_actionExport_PNG_Image_triggered();

    void on_actionExport_TIFF_Image_triggered();

    void on_actionSet_Curve_Color_triggered();

    void on_actionAbout_triggered();

    void on_actionFit_Curve_Function_triggered();

    void on_actionShowTrendLine_triggered();

    void on_actionHideTrendLine_triggered();

    void on_action100_dpi_triggered();

    void on_action300_dpi_triggered();

    void on_action600_dpi_triggered();

private:
    Ui::newGeneWindow *ui;

//函数
    void paintEvent(QPaintEvent *);

    void WarningMessage(QString message);

    void closeEvent(QCloseEvent *);

//变量
    QImage newGeneCurveImage; // 新基因图像数据
    CurveParameterForNewGeneData CurveParameterForNewGene; //图像参数
    bool IsImagePrepared; // 图像数据是否准备好了
    bool toCaptureCursorPos; //  是否需要捕获鼠标信息
    bool IsAxisDialogWindowOpen;   //  axis设置窗口是否打开了
    bool ISColorDialogWindowOpen; // 颜色设置窗口是否打开了



};

#endif // NEWGENEWINDOW_H
