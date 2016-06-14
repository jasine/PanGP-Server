#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    newGene(false)
{

   QPixmap pixmap(":/image/back.jpg");
    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(pixmap));
    setPalette(palette);
    

    // 设置窗口大小
    ui->setupUi(this);
    MainWindowSize(false); // 最小模式

    // 设置软件名称与版本
    this->setWindowTitle("Pan-Genome Profile Analyze Tool (V1.0.1)");

    //隐藏最小按钮
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |Qt::WindowMinimizeButtonHint |Qt::WindowCloseButtonHint);

    //窗口状态
    MainStatus(false,false,true);

    //切换按钮
    ui->pushButtonHide->setVisible(false);
    ui->pushButtonShow->setVisible(false);
    ui->pushButtonCancel->setVisible(false);

    //进度条
    ui->labelProcessIMG->setVisible(false);

    QMovie *movie =new QMovie(":/image/load.gif");
    ui->labelProcessIMG->setMovie(movie);
    movie->start();

    //一开始设置运行记录框不可见
    ui->groupBoxProcessMsg->setVisible(false);
    ui->groupBoxProcessMsg->setEnabled(false);

    //设置初始抽样方法
    ui->comboBoxSampleMethod->setCurrentIndex(0);
    ui->lineEditSampleSize->setEnabled(false);
    ui->lineEditSampleRepeat->setEnabled(false);
    ui->lineEditAmpCoefficient->setEnabled(false);

    //设置抽样大小 sample size
    ui->lineEditSampleSize->setText("500");

    //设置抽样重复次数 sample repeat
    ui->lineEditSampleRepeat->setText("10");

    //设置放大因子AmpCoefficient
    ui->lineEditAmpCoefficient->setText("10");

    // 是否存在新基因的数据
    PanProfile.IsNew=false;

    //参数初始化
    DataType=0;
    InputDataPath="";
    SamplingMethod=-1;

    processMessage="";//记录运行日志

/*
 *初始化曲线参数信息
 */
    initializePanCoreWindowParameter();
    initializeNewGeneWindowParameter();


    //about窗口是否打开
    IsAboutWindowOpen=false;

}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
###############################################
                 主窗口自己事务处理
###############################################
*/

// 警告提示信息
void MainWindow::WarningMessage(QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setWindowTitle("Tips");
    msgBox.exec();
}


// 窗体状态设置
void MainWindow::MainStatus(bool bIsHeader,bool bIsClusterID, bool bParameter)
{
    ui->checkBoxIsHeader->setVisible(bIsHeader);
    ui->checkBoxIsClusterID->setVisible(bIsClusterID);
    ui->GroupSampleMethod->setEnabled(bParameter);
}

void  MainWindow::MainWindowSize(bool IsMax)
{
    if(IsMax == true)
    {
        this->setMaximumWidth(600);
        this->setMaximumHeight(600);
        this->resize(QSize(600,600));
    }else
    {
        //ui->setupUi(this);
        this->setMaximumWidth(600);
        this->setMaximumHeight(320);
        this->resize(QSize(600,320));
    }
}


// 输入数据类型

void MainWindow::on_radioButtonMatrixFile_clicked() // matrix
{
    MainStatus(false,false,true);
    DataType=1;
}

void MainWindow::on_radioButtonClusterFile_clicked() // clusterFile
{
    MainStatus(true,true,true);
    DataType=2;
}

void MainWindow::on_radioButtonPanData_clicked() // panData
{
    MainStatus(false,false,false);
    DataType=3;
}

void MainWindow::on_pushButtonBrowse_clicked()
{
    QString fileName ="";
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("*"));
    if(fileName!="")
    {
        ui->textBrowserFilePath->setText(fileName);
        InputDataPath=fileName;
    }
}


// 抽样方法切换的时候，选择性disable抽样大小这个控件
void MainWindow::on_comboBoxSampleMethod_currentIndexChanged(int index)
{
    if(index==0) // 遍历所有
    {
        // 如果是选择遍历，那么抽样大小这一项给失活掉
        ui->lineEditSampleSize->setEnabled(false); // sample size
        ui->lineEditSampleRepeat->setEnabled(false); // sample repeat
        ui->lineEditAmpCoefficient->setEnabled(false); // amp coefficient
    }

    if(index==1) // TR algorithm
    {
        ui->lineEditSampleSize->setEnabled(true); // sample size
        ui->lineEditSampleRepeat->setEnabled(true); // sample repeat
        ui->lineEditAmpCoefficient->setEnabled(false); // amp corrficient
    }

    if (index==2) // DG algorithm
    {
        ui->lineEditSampleSize->setEnabled(true); // sample size
        ui->lineEditSampleRepeat->setEnabled(true); // sample repeat
        ui->lineEditAmpCoefficient->setEnabled(true); // amp corrficient
    }
}


void MainWindow::autoExec(QString filename,QString outPath)
{
    this->outpath=outPath;

    Isheader=true;
    IsClusterID=true;
    SamplingMethod=0;
    SampleRepeat=10;
    SampleSize=500;
    AmpCoefficient=10;
    DataType=3;
    InputDataPath=filename;

    // 输入文件类型


    // 检查输入文件
    bool FileValidity = CheckInputFileFormatValidity(DataType,InputDataPath);
    if(FileValidity==false)
    {
        qDebug() << "file checking";
        return;
    }

    // 封装数据类

    // 公共部分
    SampleParameter.DataType=DataType;
    SampleParameter.InputDataPath=InputDataPath;
    SampleParameter.IsClusterID=IsClusterID;
    SampleParameter.Isheader=Isheader;
    SampleParameter.SampleRepeat=SampleRepeat;
    SampleParameter.SampleSize=SampleSize;
    SampleParameter.SamplingMethod=SamplingMethod;
    SampleParameter.AmpCoefficient=AmpCoefficient;



    qRegisterMetaType<SampleParameterData>("SampleParameterData"); //注册自定义的数据类型

    SampleThread * stp = new SampleThread(SampleParameter,this);

    qRegisterMetaType<PanProfileData>("PanProfileData"); //注册自定义的数据类型


    //反馈运行日志的通道
    connect(stp,SIGNAL(sigSampleThreadProcessMsg(QString)),
            this,SLOT(slotScheduleProcessMSG(QString)));

    //监控抽样子线程是否运行结束
    connect(stp,SIGNAL(sigSampleThreadFinished(PanProfileData)),
            this,SLOT( slotDrawThread(PanProfileData)));


    //终止抽样子线程的通信
    connect(this,SIGNAL(sigKillSampleThread()),
            stp,SLOT(slotKillMyself()));

    stp->start();
}


void MainWindow::on_pushButtonOK_clicked()
{
    // 初始化一些参数 避免出错MainWindow
    Isheader=true;
    IsClusterID=true;
    SamplingMethod=0;
    SampleRepeat=10;
    SampleSize=500;
    AmpCoefficient=10;


    // 输入文件类型
    if(DataType==0)
    {
        WarningMessage("Please select input data type");
        return;
    }

    // 输入文件路径
    if(InputDataPath=="")
    {
        WarningMessage("Please assingn input data file");
        return;
    }

    if(DataType==2)
    {
        Isheader=ui->checkBoxIsHeader->isChecked();
        IsClusterID=ui->checkBoxIsClusterID->isChecked();
    }

    if(DataType==1 || DataType==2)

    {
        // 抽样算法
        SamplingMethod=ui->comboBoxSampleMethod->currentIndex();

        if(SamplingMethod>0)
        {
            //抽样样本大小
            SampleSize=ui->lineEditSampleSize->text().toInt();
            if(SampleSize<200 || SampleSize >20000)
            {
                WarningMessage("Sample size is an integer between 200 and 20000");
                return;
            }

            // 抽样重复次数
            SampleRepeat=ui->lineEditSampleRepeat->text().toInt();
            if(SampleRepeat<4 || SampleRepeat > 100)
            {
                WarningMessage("Sample repeat is an integer between 4 and 100");
                return;
            }
        }

        if(SamplingMethod==2)
        {
            // 放大系数
            AmpCoefficient=ui->lineEditAmpCoefficient->text().toInt();
            if(AmpCoefficient<4 || AmpCoefficient>SampleRepeat)
            {
                WarningMessage("Amplification coefficient is an integer between 4 and [the value of Sample repeat]");
                return;
            }
        }
    }

    // 检查输入文件
    bool FileValidity = CheckInputFileFormatValidity(DataType,InputDataPath);
    if(FileValidity==false)
    {
        qDebug() << "file checking";
        return;
    }

    // 封装数据类

    // 公共部分
    SampleParameter.DataType=DataType;
    SampleParameter.InputDataPath=InputDataPath;
    SampleParameter.IsClusterID=IsClusterID;
    SampleParameter.Isheader=Isheader;
    SampleParameter.SampleRepeat=SampleRepeat;
    SampleParameter.SampleSize=SampleSize;
    SampleParameter.SamplingMethod=SamplingMethod;
    SampleParameter.AmpCoefficient=AmpCoefficient;


/*
    if(DataType==2)
    {
        SampleParameter.IsClusterID=IsClusterID;
        SampleParameter.Isheader=Isheader;
    }

    if(DataType==1 || DataType==2)
    {
        SampleParameter.SamplingMethod=SamplingMethod;
        if(SamplingMethod>0)
        {
           SampleParameter.SampleSize=SampleSize;
           SampleParameter.SampleRepeat=SampleRepeat;
        }
        if(SamplingMethod==2)
        {
            SampleParameter.AmpCoefficient=AmpCoefficient;
        }
    }
*/
    //窗体其他部件控制
    ui->GroupDataType->setEnabled(false);
    ui->GroupSampleMethod->setEnabled(false);


    // 设置查看运行状态和日志
    ui->pushButtonShow->setVisible(true);

    // 设置run、quit、cancel按钮
    ui->pushButtonOK->setEnabled(false);
    ui->pushButtonQuit->setVisible(false);
    ui->pushButtonCancel->setVisible(true);

    //进度条
    ui->labelProcessIMG->setVisible(true);

    //默认是可显示进度的状态
    on_pushButtonShow_clicked();
    qRegisterMetaType<SampleParameterData>("SampleParameterData"); //注册自定义的数据类型

    SampleThread * stp = new SampleThread(SampleParameter,this);

    qRegisterMetaType<PanProfileData>("PanProfileData"); //注册自定义的数据类型


    //反馈运行日志的通道
    connect(stp,SIGNAL(sigSampleThreadProcessMsg(QString)),
            this,SLOT(slotScheduleProcessMSG(QString)));

    //监控抽样子线程是否运行结束
    connect(stp,SIGNAL(sigSampleThreadFinished(PanProfileData)),
            this,SLOT( slotDrawThread(PanProfileData)));


    //终止抽样子线程的通信
    connect(this,SIGNAL(sigKillSampleThread()),
            stp,SLOT(slotKillMyself()));

    stp->start();
}

void MainWindow::on_pushButtonQuit_clicked()
{
    qDebug() << "Main window is closing";
    this->close();
}


void MainWindow::on_pushButtonHide_clicked()
{
    // 主窗口最小模式
     MainWindowSize(false); // 最小模式

    // 切换按钮
    ui->pushButtonHide->setVisible(false);
    ui->pushButtonShow->setVisible(true);

    ui->groupBoxProcessMsg->setVisible(false);
    ui->groupBoxProcessMsg->setEnabled(false);
}

void MainWindow::on_pushButtonShow_clicked()
{
    // 主窗口最大模式
     MainWindowSize(true); // 最大模式

    // 切换按钮
    ui->pushButtonHide->setVisible(true);
    ui->pushButtonShow->setVisible(false);

    ui->groupBoxProcessMsg->setVisible(true);
    ui->groupBoxProcessMsg->setEnabled(true);


}

//重写窗口关闭函数
void MainWindow::closeEvent(QCloseEvent *)
{
    // 通知about窗口关闭
    emit sigKillAboutWindow();

    //通知抽样线程终止
    emit sigKillSampleThread();

    //通知画图线程终止
    emit sigKillDrawThread();

    //通知泛基因组曲线窗口关闭
    emit sigKillPanWindow();

    //通知新基因画图线程关闭
    emit sigKillDrawNewGeneThread();

    //通知新基因窗口关闭
    emit sigKillNewGeneWindow();

    //通知拟合线程关闭
    emit sigKillFittingThread();

    //通知泛基因组和核心基因拟合窗口关闭
    emit sigKillPanFittingWindow();

    //通知新基因基因拟合窗口关闭
    emit sigKillNewGeneFittingWindow();


    qDebug() << "close all window and thread";
}

void MainWindow::on_pushButtonCancel_clicked()
{
    //窗体其他部件控制
    ui->GroupDataType->setEnabled(true);
    if(DataType!=3)
    {
        ui->GroupSampleMethod->setEnabled(true);
    }
    ui->groupBoxProcessMsg->setVisible(false);
    ui->groupBoxProcessMsg->setEnabled(false);

    // 主窗口最小模式
    MainWindowSize(false); // 最小模式


    //程序控制按钮
    ui->pushButtonOK->setEnabled(true);
    ui->pushButtonQuit->setVisible(true);
    ui->pushButtonCancel->setVisible(false);

    //运行状态切换按钮
    ui->pushButtonHide->setVisible(false);
    ui->pushButtonShow->setVisible(false);

    //进度条
    ui->labelProcessIMG->setVisible(false);

    //通知抽样线程终止
    emit sigKillSampleThread();

    //通知画图线程终止
    emit sigKillDrawThread();

    //通知泛基因组曲线窗口关闭
    emit sigKillPanWindow();

    //通知新基因画图线程关闭
    emit sigKillDrawNewGeneThread();

    //通知新基因窗口关闭
    emit sigKillNewGeneWindow();

    //通知拟合线程关闭
    emit sigKillFittingThread();

    //通知泛基因组和核心基因拟合窗口关闭
    emit sigKillPanFittingWindow();

    //通知新基因基因拟合窗口关闭
    emit sigKillNewGeneFittingWindow();


// 数据初始化
    //初始化泛基因组窗口的参数信息
    initializePanCoreWindowParameter();

    //初始化新基因窗口的参数信息
    initializeNewGeneWindowParameter();

    //初始化泛基因组数据，默认是没有新基因数据
    PanProfile.IsNew==false;
}



/*
###############################################
             处理子窗口事务 （公共事务）
###############################################
*/

void MainWindow::slotScheduleProcessMSG(QString message)
{
    // 用于调试来自信号的参数
    //qDebug() << message;

    processMessage = processMessage + "\n" + message;

    ui->textEditProcessMsg->setText(processMessage);
    ui->textEditProcessMsg->moveCursor(QTextCursor::End);
}

/*
###############################################
             处理子窗口事务 （泛基因组曲线部分）
###############################################
*/

//第一次调用DrawThread画泛基因组的图
void MainWindow::slotDrawThread(PanProfileData PanProfileRef)
{

// 这里用于测试抽样算法的结果

    //接收从抽样算法线程传来的数据
    PanProfile=PanProfileRef;

    //第一次运行的时候窗体以及画图参数，采用默认

    qDebug() << "======   next thread =============";
    CurveParameterForPanCore.ImageDPI=300;

    DrawPanCoreCurve * dctp = new DrawPanCoreCurve(PanProfile,CurveParameterForPanCore,this);

    //运行日志通道
    connect(dctp,SIGNAL(sigdrawCurveThreadProcessMsg(QString)),
            this, SLOT(slotScheduleProcessMSG(QString)));

    //运行结束通道PanCureImage
    connect(dctp,SIGNAL(sigdrawCurveFinish(QImage)),
            this, SLOT(slotOpenPanWindow(QImage)));

    //终止线程的通道
    connect(this,SIGNAL(sigKillDrawThread()),
            dctp,SLOT(slotKillMyself()));

    dctp->start();
}

//打开pan-genome曲线窗体
void MainWindow::slotOpenPanWindow(QImage PanCurveImageRef)
{
    PanCurveImageRef.save(outpath+"/Pan.png","PNG");
    
    if(this->newGene){
        slotFitData(20);
    }else{
        QCoreApplication::exit(0);
    }

    panWindow *pw =new panWindow(PanCurveImageRef,CurveParameterForPanCore,this);

    qRegisterMetaType<CurveParameterForPanCoreData>("CurveParameterForPanCoreData"); //注册自定义的数据类型

    //来自panWindow的更新请求
    connect(pw,SIGNAL(sigRequestUpdateImage(CurveParameterForPanCoreData)),
            this,SLOT(slotReceivePanWindowRequest(CurveParameterForPanCoreData)));

    //保存panData
    connect(pw,SIGNAL(sigSavePanGenomeData()),this,SLOT(slotSavePanGenomeData()));

    //发往panWindow的更新图像反馈
    connect(this,SIGNAL(sigTellPanWindowUpdate(QImage)),
            pw,SLOT(slotUpdateImage(QImage)));

    //通知panWindow关闭
    connect(this,SIGNAL(sigKillPanWindow()),pw,SLOT(slotKillMyself()));

    //请求拟合曲线方程
    connect(pw,SIGNAL(sigFitPanCore(qint32)),this,SLOT(slotFitData(qint32)));

    //请求画新基因曲线
    connect(pw,SIGNAL(sigDrawNewGeneCurve()),this,SLOT(slotDrawNewGeneThread()));

    // 子窗体通知主窗体，自己被关闭了
    connect(pw,SIGNAL(sigTellMaiWindowIAmClosed()),this,SLOT(slotPanSaysHeWasClosed()));

    //打开about窗口
    connect(pw,SIGNAL(sigOpenAboutWindow()),this,SLOT(slotOpenAboutWindow()));

    //pw->show();

    // 停止进度条
    //进度条
    ui->labelProcessIMG->setVisible(false);

}

//提示窗体更新pan-genome曲线
void  MainWindow::slotUpdatePanWindow(QImage PanCurveImageRef)
{
    qDebug() << "feedback the subWindow";
    emit sigTellPanWindowUpdate(PanCurveImageRef);
}

//重新画图
void MainWindow::slotReceivePanWindowRequest(CurveParameterForPanCoreData CurveParameterForPanCoreRef)
{
    qDebug() << "receive update image request";
    CurveParameterForPanCore=CurveParameterForPanCoreRef; //更新存在main window中的数据
    DrawPanCoreCurve * dctp = new DrawPanCoreCurve(PanProfile,CurveParameterForPanCore,this);

    //运行日志通道
    connect(dctp,SIGNAL(sigdrawCurveThreadProcessMsg(QString)),
            this, SLOT(slotScheduleProcessMSG(QString)));

    //运行结束通道
    connect(dctp,SIGNAL(sigdrawCurveFinishRePaint(QImage)),
            this, SLOT(slotUpdatePanWindow(QImage)));

    //终止线程的通道
    connect(this,SIGNAL(sigKillDrawThread()),
            dctp,SLOT(slotKillMyself()));

    dctp->start();
}

//保存泛基因组数据
void MainWindow::slotSavePanGenomeData()
{
    QString PanDataFileName=QFileDialog::getSaveFileName(this, tr("Save Pan-Genome Data"),
                                                       "PanGenomeData.txt",
                                                       tr("*"));
    if(PanDataFileName.isEmpty())
    {
        return;
    }
    QFile PDF(PanDataFileName);
    QTextStream PDFStream(&PDF);
    PDF.open(QIODevice::WriteOnly);
    PDFStream << "GenomeNum\tPan-Genome Size\tCore Genome Size\n";
    qint32 num=PanProfile.GenomeNumValue.size();
    for(qint32 i=0;i<num;i++)
    {
        PDFStream << PanProfile.GenomeNumValue[i] << "\t" << PanProfile.PanValue[i]
                     << "\t" << PanProfile.CoreValue[i] << "\n";
    }
    PDF.close();
    qDebug() << "File was saved in " << PanDataFileName;
}

void MainWindow::slotOpenAboutWindow()
{
    if(IsAboutWindowOpen==true)
    {
        emit sigKillAboutWindow();
    }

    AboutWindow *aw= new AboutWindow(this);

    connect(this,SIGNAL(sigKillAboutWindow()),aw,SLOT(close()));

    aw->show();
    IsAboutWindowOpen=true;
}

// 初始化泛基因组与核心基因组窗口一些参数
void MainWindow::initializePanCoreWindowParameter()
{
    // 窗口参数
    CurveParameterForPanCore.PanWinWidth=800; //窗口宽度 800
    CurveParameterForPanCore.PanWinHeight=600; //窗口高度 600
    CurveParameterForPanCore.IsPanWindowOpen=false; // panwindow窗体是否打开了

    // 图像参数
    CurveParameterForPanCore.ImageWidth=CurveParameterForPanCore.PanWinWidth; //图像宽度
    CurveParameterForPanCore.ImageHeight=CurveParameterForPanCore.PanWinHeight;//图像高度
    CurveParameterForPanCore.toDrawPanLegend=false;//是否画图例
    CurveParameterForPanCore.toShowBoxplot=true; // 默认显示箱线图
    CurveParameterForPanCore.ImageDPI=300; // 默认分辨率

    // 坐标轴
    CurveParameterForPanCore.PanCoreXTitle="Genome Number";//标题
    CurveParameterForPanCore.PanCoreYTitle="Gene Cluster Number";

    CurveParameterForPanCore.PanCoreXTitleSize=15; // X轴字号
    CurveParameterForPanCore.PanCoreYTitleSize=15; // Y轴字号

    QFont font("Times");
    CurveParameterForPanCore.PanCoreXTitleFont=font;
    CurveParameterForPanCore.PanCoreYTitleFont=font;

    // 刻度
    CurveParameterForPanCore.PanCoreXScaleValueSize=10;
    CurveParameterForPanCore.PanCoreYScaleValueSize=10;
    CurveParameterForPanCore.PanCoreXScaleValueFont=font;
    CurveParameterForPanCore.PanCoreYScaleValueFont=font;

    // 颜色
    CurveParameterForPanCore.PanColorID=4;
    CurveParameterForPanCore.CoreColorID=5;
    CurveParameterForPanCore.PanColor =CurveParameterForPanCore.AllColor[CurveParameterForPanCore.PanColorID];
    CurveParameterForPanCore.CoreColor=CurveParameterForPanCore.AllColor[CurveParameterForPanCore.CoreColorID];
}

// 初始化泛基因组窗口的数据
void MainWindow::slotPanSaysHeWasClosed()
{
    // 调用主窗体取消函数
    on_pushButtonCancel_clicked();

    //初始化所有参数
    initializePanCoreWindowParameter();
}




/*
###############################################
             处理子窗口事务 （新基因曲线部分）
###############################################
*/


// 画新基因的曲线
void MainWindow::slotDrawNewGeneThread()
{
    // 先判断新基因的拟合结果是否存在，如果不存在，先做新基因的结果进行拟合
    if(FittingResult.IsNewFittingFinished ==false)
    {
        qDebug()<< "new gene is not fitted, so the thread go to fit first.";
        slotFitData(20); // 拟合但不打开窗口
        return;
    }

    // 关闭已经存在的窗口
    emit sigKillNewGeneWindow();
    CurveParameterForNewGene.IsNewGeneWindowOpen=false;


    // 启动画新基因的子线程
    DrawNewGeneCurve *dngc=new DrawNewGeneCurve(PanProfile,CurveParameterForNewGene,FittingResult,this);

    qRegisterMetaType<FittingResultData>("FittingResultData"); //注册自定义的数据类型

    // 通知线程关闭自己
    connect(this,SIGNAL(sigKillDrawNewGeneThread()),dngc,SLOT(slotKillMyself()));

    // 线程汇报自己的进度信息
    connect(dngc,SIGNAL(sigdrawNewGeneCurveThreadProcessMsg(QString)),this,SLOT(slotScheduleProcessMSG(QString)));

    // 线程返回数据
    connect(dngc,SIGNAL(sigdrawNewGeneCurveFinish(QImage)),this,SLOT(slotOpenNewGeneWindow(QImage)));

    dngc->start();
}


// 打开新基因曲线窗口

void MainWindow::slotOpenNewGeneWindow(QImage NewGeneCurveImageRef)
{
    NewGeneCurveImageRef.save(outpath+"/NewGene.png","PNG");
    newGeneWindow * ngw =new newGeneWindow(NewGeneCurveImageRef,CurveParameterForNewGene,this);

    qRegisterMetaType<CurveParameterForNewGeneData>("CurveParameterForNewGeneData"); //注册自定义的数据类型

    // 通知新基因窗口关闭
    connect(this,SIGNAL(sigKillNewGeneWindow()),ngw,SLOT(slotKillMyself()));

    // 通知新基因窗口更新图像数据
    connect(this,SIGNAL(sigTellNewGeneWindowUpdate(QImage)),ngw,SLOT(slotUpdateImage(QImage)));

    // 来自新基因窗口的图像更新请求
    connect(ngw,SIGNAL(sigRequestUpdateImage(CurveParameterForNewGeneData)),this,SLOT(slotReceiveNewGeneWindowRequest(CurveParameterForNewGeneData)));

    // 子窗口通知主窗口，自己被关闭了
    connect(ngw,SIGNAL(sigTellMaiWindowIAmClosed()),this,SLOT(slotNewGeneSayHeWasClosed()));

    // 拟合新基因的曲线
    connect(ngw,SIGNAL(sigFitNewGene(qint32)),this,SLOT(slotFitData(qint32)));

    // 请求打开about窗口
    connect(ngw,SIGNAL(sigOpenAboutWindow()),this,SLOT(slotOpenAboutWindow()));

    QCoreApplication::exit(0);


    ngw->show();
}


// 接收新基因窗口更新图像数据的请求，并启动画新基因曲线的线程
void MainWindow::slotReceiveNewGeneWindowRequest(CurveParameterForNewGeneData CurveParameterForNewGeneRef)
{
    //接收参数
    CurveParameterForNewGene=CurveParameterForNewGeneRef;

    // 启动画图线程
    DrawNewGeneCurve * dngc = new DrawNewGeneCurve(PanProfile,CurveParameterForNewGene,FittingResult,this);

    //qRegisterMetaType<CurveParameterForNewGeneData>("CurveParameterForNewGeneData"); //注册自定义的数据类型

    //  通知线程关闭
    connect(this,SIGNAL(sigKillDrawNewGeneThread()),dngc,SLOT(slotKillMyself()));

    // 获取返回图像信息
    connect(dngc,SIGNAL(sigdrawNewGeneCurveFinishRePaint(QImage)),this,SLOT(slotUpdateNewGeneWindow(QImage)));

    //线程汇报当前进度信息
    connect(dngc,SIGNAL(sigdrawNewGeneCurveThreadProcessMsg(QString)),this,SLOT(slotScheduleProcessMSG(QString)));

    dngc->start();
}

// 通知新基因窗口更新图像数据
void MainWindow::slotUpdateNewGeneWindow(QImage NewGeneCurveImageRef)
{
    qDebug() << "Main window tell new window to update";
    emit sigTellNewGeneWindowUpdate(NewGeneCurveImageRef);
}

//曲线拟合
void MainWindow::slotFitData(qint32 FitType)
{
    qDebug() << "go to fit " << FitType;

    /*
     * FitType 的值有两个
     *    1开头：pan core, 11:拟合并打开窗口， 10：拟合不打开窗口
     *    2开头：new gene, 21:拟合并打开窗口， 20：拟合不打开窗口
    */

    // 设置拟合类型
    FittingResult.FitType=FitType;

    DataFittingThread * dft = new DataFittingThread(PanProfile,FittingResult,this);

    //qRegisterMetaType<PanProfileData>("PanProfileData"); //注册自定义的数据类型

    qRegisterMetaType<FittingResultData>("FittingResultData"); //注册自定义的数据类型

    // 接收线程的返回信号
    connect(dft,SIGNAL(sigFittingFinished(FittingResultData)),this,SLOT(slotReceiveFittingResult(FittingResultData)));

    // 给拟合线程发信号
    connect(this,SIGNAL(sigKillFittingThread()),dft,SLOT(slotKillMyself()));

    dft->start();

    qDebug() << "data fitting thread starts" ;
}


void MainWindow::slotReceiveFittingResult(FittingResultData FittingResultRef)
{
    FittingResult=FittingResultRef;

     qDebug() << "receive fitting result";

    if(FittingResult.FitType == 11 ) // 打开泛基因组拟合结果窗口
    {
        qDebug() << "Show pan and core fitting result";

        emit sigKillPanFittingWindow();
        PanFittingWindow * pfw= new PanFittingWindow(FittingResult,this);

        //qRegisterMetaType<FittingResultData>("FittingResultData"); //注册自定义的数据类型

        // 通知泛基因组拟合窗口关闭
        connect(this,SIGNAL(sigKillPanFittingWindow()),pfw, SLOT(slotKillMyself()));

        pfw->show();

    }

    if(FittingResult.FitType == 21 ) // 打开新基因拟合结果拟合结果窗口
    {
        qDebug() << "Show new gene fitting result";

        emit sigKillNewGeneFittingWindow();

        NewFittingWindow * nfw = new NewFittingWindow(FittingResult,this);

        //通知新基因拟合窗口关闭
        connect(this,SIGNAL(sigKillNewGeneFittingWindow()),nfw,SLOT(slotKillMyself()));

        nfw->show();
    }

    if(FittingResult.FitType==20)
    {
        qDebug() << "data for new gene fitting";
        slotDrawNewGeneThread();
    }

}




// 初始化新基因窗口的一些参数
void MainWindow::initializeNewGeneWindowParameter()
{
    //窗口属性   
    CurveParameterForNewGene.NewGeneWinWidth=800;
    CurveParameterForNewGene.NewGeneWinHeight=600;
    CurveParameterForNewGene.IsNewGeneWindowOpen=false;// 新基因的窗口是否打开了

    // 图像参数
    CurveParameterForNewGene.ImageWidth=CurveParameterForNewGene.NewGeneWinWidth;
    CurveParameterForNewGene.ImageHeight=CurveParameterForNewGene.NewGeneWinHeight;
    CurveParameterForNewGene.toShowTrendLine=false; // 默认不显示趋势线
    CurveParameterForNewGene.toDrawNewGeneLegend=false; // 默认不画图例
    CurveParameterForNewGene.ImageDPI=300; //默认图像DPI 300

    //默认不显示趋势线
    CurveParameterForNewGene.toShowTrendLine=false; // 默认显示箱线图

    // 坐标轴
    CurveParameterForNewGene.NewGeneXTitle="Genome Number";//标题
    CurveParameterForNewGene.NewGeneYTitle="New Gene Cluster Number";

    CurveParameterForNewGene.NewGeneXTitleSize=15;
    CurveParameterForNewGene.NewGeneYTitleSize=15;

    QFont font("Times");
    CurveParameterForNewGene.NewGeneXTitleFont=font;
    CurveParameterForNewGene.NewGeneYTitleFont=font;

    // 刻度
    CurveParameterForNewGene.NewGeneXScaleValueSize=10;
    CurveParameterForNewGene.NewGeneYScaleValueSize=10;
    CurveParameterForNewGene.NewGeneXScaleValueFont=font;
    CurveParameterForNewGene.NewGeneYScaleValueFont=font;

    // 初始化颜色
    CurveParameterForNewGene.BarColorID=5;
    CurveParameterForNewGene.BarColor=CurveParameterForNewGene.AllColor[CurveParameterForNewGene.BarColorID];

    CurveParameterForNewGene.TrendLineColorID=1;
    CurveParameterForNewGene.TrendLineColor=CurveParameterForNewGene.AllColor[CurveParameterForNewGene.TrendLineColorID];

    // 初始化新基因你和结果
    FittingResult.IsNewFittingFinished=false;


}

// 初始化新基因画图参数数据
void MainWindow::slotNewGeneSayHeWasClosed()
{
    // 初始化新基因的参数
    initializeNewGeneWindowParameter();

    // 关闭新基因的拟合
    sigKillNewGeneFittingWindow();
}

// 检查输入文件格式与选择的数据类型是否一致
bool MainWindow::CheckInputFileFormatValidity(qint32 DataTypeRef, QString InputDataPath)
{
    qint32 DataType=DataTypeRef;
    QString InputData = InputDataPath;
    QString line;
    QFile DATAHand(InputData);
    DATAHand.open(QIODevice::ReadOnly);


    if(DataType==1) // 矩阵数据
    {

        qDebug() << "check data type 0";

        qint32 MaxCheckLine=200;
        qint32 CurrentLine=0;

        while((!DATAHand.atEnd()) and (CurrentLine < MaxCheckLine))
        {
            CurrentLine++;
            line=DATAHand.readLine();
            line.replace("\n","");
            line.replace("\r","");
            QString tmpLine=line;
            tmpLine.replace("1","");
            tmpLine.replace("0","");
            if(tmpLine != "")
            {
                WarningMessage("The appointed input file does NOT like matrix File or data format error");
                return false;
            }
        }
    }


    if(DataType==2)
    {
        qDebug() << "check data type 1";

        QList <QString> row;
        qint32 MaxCheckLine=1000;
        qint32 CurrentLine=0;
        qint32 genomeNum;

        // 读第一行
        line=DATAHand.readLine();
        row=line.split("\t");
        genomeNum=row.size();

        while((!DATAHand.atEnd()) and (CurrentLine < MaxCheckLine))
        {
            CurrentLine++;
            line=DATAHand.readLine();
            line.replace("\n","");
            line.replace("\r","");
            row=line.split("\t");
            if(row.size()>3)
            {
                if(row.size()!=genomeNum)
                {
                    WarningMessage("The appointed input file does NOT like gene cluster file or data format error");
                    return false;
                }
            }else
            {
                WarningMessage("The appointed input file does NOT like gene cluster file or data format error");
                return false;
            }
        }
    }

    if(DataType==3)
    {
        qDebug() << "check data type 2";

        QList <QString> row;
        qint32 MaxCheckLine=1000;
        qint32 CurrentLine=0;
        // 读第一行
        line=DATAHand.readLine();

        while((!DATAHand.atEnd()) and (CurrentLine < MaxCheckLine))
        {
            CurrentLine++;
            line=DATAHand.readLine();
            line.replace("\n","");
            line.replace("\r","");

            row=line.split("\t");
            if(row.size()!=3 &&row.size()!=4)
            {
                WarningMessage("The appointed input file does NOT like Pan-genome Data File or data format error");
                return false;
            }

            QString tmpLine;
            QRegExp re("[0-9]");
            tmpLine=line;
            tmpLine.replace(re,"");
            row=tmpLine.split("\t");
            if(row[0] == "" && row[1] =="" && row[2] =="")
            {
            }else
            {
                WarningMessage("The appointed input file does NOT like Pan-genome Data File or data format error");
                return false;
            }
        }
    }

    DATAHand.close();

    return true;
}




