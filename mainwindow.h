#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QMovie>

#include <samplethread.h> // sampling
#include <sampleparameterdata.h> // sample parameter data

#include <panprofiledata.h> // pan profile

#include <aboutwindow.h> // about window

#include <datafittingthread.h> // data fitting
#include <fittingresultdata.h> // data fitting result

#include <panfittingwindow.h> // pan fitting windows
#include <newfittingwindow.h> // new fitting windows


#include <panwindow.h> // pancore
#include <drawpancorecurve.h>
#include <curveparameterforpancoredata.h>

#include <newgenewindow.h> // new gene
#include <drawnewgenecurve.h>
#include <curveparameterfornewgenedata.h>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void autoExec(QString filename,QString outPath);
    QString outpath;



signals:

    //发向抽样线程的信号
    void sigKillSampleThread(); // 通知抽样thread终止自己

    //发向画图线程的信号
    void sigKillDrawThread(); // 通知pan-core画图thread终止自己
    void sigKillDrawNewGeneThread(); // 通知画新基因的thread终止自己

    //发向泛基因组窗口的信号
    void sigKillPanWindow(); //通知泛基因组窗口关闭
    void sigTellPanWindowUpdate(QImage PanCurveImageRef); //通知泛基因组窗口更新

    //关闭about窗口
    void sigKillAboutWindow();

    //发向新基因窗口的信号
    void sigKillNewGeneWindow();// 通知新基因窗口关闭
    void sigTellNewGeneWindowUpdate(QImage NewGeneCurveImageRef); // 通知新基因窗口更新图像

    //发向拟合线程的信号
    void sigKillFittingThread(); // 通知拟合线程终止

    //发向泛基因组拟合窗口的信号
    void sigKillPanFittingWindow();//通知泛基因组拟合窗口关闭

    //发向新基因拟合窗口的信号
    void sigKillNewGeneFittingWindow();//通知新基因集合窗口关闭


    
private slots:

    void on_radioButtonMatrixFile_clicked();

    void on_radioButtonClusterFile_clicked();

    void on_pushButtonBrowse_clicked();

    void on_pushButtonOK_clicked();

    void on_pushButtonQuit_clicked();

    void on_radioButtonPanData_clicked();

    void on_pushButtonHide_clicked();

    void on_pushButtonShow_clicked();

    void on_pushButtonCancel_clicked();

// 公共窗体槽函数
    // 接收程序运行日志信息
    void slotScheduleProcessMSG(QString message);

    /*
     * 曲线拟合 FitType 的值有两个
     *    1开头：pan core, 11:拟合并打开窗口， 10：拟合不打开窗口
     *    2开头：new gene, 21:拟合并打开窗口， 20：拟合不打开窗口
    */
    void slotFitData(qint32 FitType);

    void slotOpenAboutWindow(); // 打开about窗口


//pan-genome曲线窗体
    //启动画图程序
    void slotDrawThread(PanProfileData PanProfileRef); // 需要修改，后续还需要传入图形的各项参数

    void slotOpenPanWindow(QImage PanCurveImageRef); // 打开泛基因组窗体

    void slotUpdatePanWindow(QImage PanCurveImageRef); //画图线程让主窗体给panwindow 发信号，让其更新窗体

    void slotReceivePanWindowRequest(CurveParameterForPanCoreData CurveParameterForPanCoreRef); //接收panwindow信号

    void slotSavePanGenomeData();// 保存Pan-genome Data

    void slotPanSaysHeWasClosed(); // 泛基因组窗口通知自己被关闭了


//新基因的曲线窗体
    void slotDrawNewGeneThread();//画新基因的曲线的线程

    void slotOpenNewGeneWindow(QImage NewGeneCurveImageRef);// 打开新基因曲线窗口

    void slotUpdateNewGeneWindow(QImage NewGeneCurveImageRef);//画图线程让主窗体给newgenewindow 发信号，让其更新窗体

    void slotReceiveNewGeneWindowRequest(CurveParameterForNewGeneData CurveParameterForNewGeneRef); // 接收新基因窗体的更新请求信号

    void slotNewGeneSayHeWasClosed();// 新基因窗口通知自己被关闭了
    void on_comboBoxSampleMethod_currentIndexChanged(int index);


//打开拟合结果的窗体
    void slotReceiveFittingResult(FittingResultData FittingResultRef); // 接收拟合结果

private:
    Ui::MainWindow *ui;


    void closeEvent(QCloseEvent *);//对关闭函数进行重写

    void MainStatus(bool bIsHeader,bool bIsClusterID, bool bParameter);
    void MainWindowSize(bool IsMax);// 窗口大小切换
    void WarningMessage(QString message);
    bool CheckInputFileFormatValidity(qint32 DataTypeRef, QString InputDataPath);

    // 初始化泛基因组曲线窗体参数
    void initializePanCoreWindowParameter();
    // 初始化新基因曲线窗体参数
    void initializeNewGeneWindowParameter();

    // 拟合结果数据
    FittingResultData FittingResult; // 记录泛基因组、核心基因、新基因的拟合结果

    //泛基因组数据结构体以及数据
    PanProfileData PanProfile;

    //pan-genome曲线窗体以及图形曲线
    CurveParameterForPanCoreData CurveParameterForPanCore;

    //New Gene 曲线窗体参数
    CurveParameterForNewGeneData CurveParameterForNewGene;

    //泛基因组、新基因数据拟合的结果
    FittingResultData CurveFittingResult;

    //全局参数
    QString processMessage; //运行日志

    // 抽样参数
    SampleParameterData SampleParameter; //抽样参数

    // 捕获的窗体输入参数
    qint32 DataType; // 数据类型
    QString InputDataPath; // 输入数据路径
    qint32 SamplingMethod; // 抽样方法
    qint64 SampleSize; //抽样大小
    qint32 SampleRepeat; // 抽样重复次数
    qint32 AmpCoefficient; // 放大系数
    bool Isheader; // 是否包含文件头
    bool IsClusterID; // 是否包含cluster ID

    // about窗体是否打开
    bool IsAboutWindowOpen;





};

#endif // MAINWINDOW_H
