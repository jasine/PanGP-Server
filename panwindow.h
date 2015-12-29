#ifndef PANWINDOW_H
#define PANWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPoint>
#include <QPainter>
#include <panprofiledata.h>
#include <curveparameterforpancoredata.h>
#include <QAction>
#include <QMouseEvent>
#include <QCursor>
#include <QMessageBox>
#include <QFileDialog>
#include <panwindowdialogsetaxis.h>
#include <panwindowdialogsetcolor.h>
#include <QImageWriter>

namespace Ui {
class panWindow;
}

class panWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit panWindow(QImage PanCureImageRef, CurveParameterForPanCoreData CurveParameterForPanCoreRef
                       , QWidget *parent);
    ~panWindow();

signals:
    void sigRequestUpdateImage(CurveParameterForPanCoreData CurveParameterForPanCoreRef);//请求更新图像
    void sigToKillAxisDialogWindow();// 关闭坐标轴参数修改窗口
    void sigToKillColorDialogWindow();// 关闭坐标轴参数修改窗口
    void sigSavePanGenomeData(); //保存泛基因组数据
    void sigOpenAboutWindow(); // 打开about 窗口
    void sigDrawNewGeneCurve();// 画新基因的曲线
    void sigFitPanCore(qint32 FitType);//拟合泛基因组和核心基因组,参数含义FitType=1,拟合core and pan, =0,拟合New Gene
    void sigTellMaiWindowIAmClosed();//通知主窗口，自己已经被关闭了

public slots:
    void slotKillMyself(); //关闭自己
    void slotUpdateImage(QImage PanCureImageRef);//更新图像
    void slotReceiveDialog(CurveParameterForPanCoreData CurveParameterForPanCoreRef);//接受窗体Edit项目更新图像方面的请求

    
private slots:
    void on_actionAdd_Legend_triggered();

    void on_actionExport_PNG_Image_triggered();

    void on_actionSet_Axis_Titles_triggered();

    void on_actionSet_Curve_Color_triggered();

    void on_actionSave_Pan_Genome_Data_triggered();

    void on_actionExport_TIFF_Image_triggered();

    void on_actionAbout_triggered();

    void on_actionDraw_New_Gene_Curve_triggered();

    void on_actionFit_Curve_Function_triggered();

    void on_actionBoxplot_triggered();

    void on_actionScatter_Plot_triggered();

    void on_action100_DPI_triggered();

    void on_action300_dpi_triggered();

    void on_action600_dpi_triggered();

private:
    Ui::panWindow *ui;


    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *);//对关闭函数进行重写
    void mousePressEvent(QMouseEvent *);
    void WarningMessage(QString message);


    //泛基因组数据
    PanProfileData PanProfile; //接收传入的泛基因组数据

    //泛基因组曲线参数数据
    CurveParameterForPanCoreData CurveParameterForPanCore;

    bool IsImagePrepared; //判断图像数据是否发过来了
    QImage PanCureImage; // 图像数据将画在这个上面

    bool toCaptureCursorPos; //  是否需要捕获鼠标信息
    bool IsAxisDialogWindowOpen;   //  axis设置窗口是否打开了
    bool ISColorDialogWindowOpen; // 颜色设置窗口是否打开了




};

#endif // PANWINDOW_H
