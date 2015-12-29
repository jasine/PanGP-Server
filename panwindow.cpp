#include "panwindow.h"
#include "ui_panwindow.h"


panWindow::panWindow(QImage PanCureImageRef, CurveParameterForPanCoreData CurveParameterForPanCoreRef, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::panWindow)
{
    ui->setupUi(this);

    toCaptureCursorPos=false;


    // 默认情况下认为图像数据没有准备好
    IsImagePrepared=true;

    // 接收传过来的图像数据
    PanCureImage=PanCureImageRef;

    // 接收传过来的窗体以及图像参数
    CurveParameterForPanCore=CurveParameterForPanCoreRef;
    CurveParameterForPanCore.IsPanWindowOpen=true;

    // 判断窗口是否打开了
    IsAxisDialogWindowOpen=false;
    ISColorDialogWindowOpen=false;


    //设置窗口大小
    this->resize(QSize(CurveParameterForPanCore.PanWinWidth,CurveParameterForPanCore.PanWinHeight));

    //隐藏最小按钮
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |Qt::WindowMinimizeButtonHint |Qt::WindowCloseButtonHint);

    //固定窗体大小
    this->setFixedSize(this->size());   

    qDebug() << "window open";



}

panWindow::~panWindow()
{
    delete ui;
}

void panWindow::paintEvent(QPaintEvent *)
{
    //qDebug() << "image data was loaded";
    //调整窗口大小
    //this->resize(QSize(CurveParameter.PanWinWidth,CurveParameter.PanWinHeight));

    if(IsImagePrepared==false)
    {
        return;
    }

    //qDebug() << "go to paint";

    //将图像数据画到窗体上
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform,true);//平滑直线
    painter.scale(1.0/(CurveParameterForPanCore.ImageDPI/100.0),1.0/(CurveParameterForPanCore.ImageDPI/100.0));
    painter.drawImage(0,0,PanCureImage);
    //painter.drawImage(0,0,PanCureImage);//加载图像到窗体上
    //qDebug() << " image data is loaded." ;
}


//更新窗体和曲线
void panWindow::slotUpdateImage(QImage PanCureImageRef)//更新图像
{
    PanCureImage=PanCureImageRef; //更新图像数据
    qDebug() << "get updated image information";
    update();//更新窗体上的图像
}


//关闭自己
void panWindow::slotKillMyself()
{
    close();
}

void panWindow::mousePressEvent(QMouseEvent *event)
{
    if(toCaptureCursorPos==false)
    {
        return;
    }
    QPoint mpos =  event->pos();//获取坐标
    qreal x=mpos.x();
    qreal y=mpos.y();
    qDebug() << "mouse press position: x-> " << x << "\ty-> " << y;
    //this->setCursor(Qt::CrossCursor);

    CurveParameterForPanCore.toDrawPanLegend=true;
    CurveParameterForPanCore.LegendTopLeftX=mpos.x();
    CurveParameterForPanCore.LegendTopLeftY=mpos.y();

    //还原鼠标样式
    this->setCursor(Qt::ArrowCursor);

    //关闭再次捕获
    toCaptureCursorPos=false;

    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForPanCore);
}


void panWindow::on_actionAdd_Legend_triggered()
{
    WarningMessage("Press your mouse on the image, the legend would be stamped where the cursor locates.");
    toCaptureCursorPos=true;
    this->setCursor(Qt::CrossCursor);//改变鼠标
}


void panWindow::WarningMessage(QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setWindowTitle("Tips");
    msgBox.exec();
}



void panWindow::on_actionExport_PNG_Image_triggered()
{
    QString ImageFileName=QFileDialog::getSaveFileName(this, tr("Save image with PNG format"),
                                                       "Pan-Genome.png",
                                                       tr("Images (*.png)"));
    if(ImageFileName.isEmpty())
    {
        return;
    }
    PanCureImage.save(ImageFileName,"PNG");
}

void panWindow::on_actionExport_TIFF_Image_triggered()
{
    QString ImageFileName=QFileDialog::getSaveFileName(this, tr("Save image with TIFF format"),
                                                       "Pan-Genome.tiff",
                                                       tr("Images (*.tiff)"));
    QImage tmpImage=PanCureImage;
    QImageWriter writeImage;

    if(ImageFileName.isEmpty())
    {
        return;
    }

    writeImage.setQuality(100);
    writeImage.setFormat("tiff");
    writeImage.setFileName(ImageFileName);
    writeImage.setCompression(1);
    writeImage.write(tmpImage);
}


//接受窗体Edit项目更新图像方面的请求
void panWindow::slotReceiveDialog(CurveParameterForPanCoreData CurveParameterForPanCoreRef)
{
    CurveParameterForPanCore=CurveParameterForPanCoreRef;

    //给主线程发送请求更新信号
    emit sigRequestUpdateImage(CurveParameterForPanCore);

}


void panWindow::on_actionSet_Axis_Titles_triggered()
{
    if(IsAxisDialogWindowOpen==true)
    {
        emit sigToKillAxisDialogWindow();
    }

    PanWindowDialogSetAxis *pwdsa= new PanWindowDialogSetAxis(CurveParameterForPanCore,this);

    qRegisterMetaType<CurveParameterForPanCoreData>("CurveParameterForPanCoreData"); //注册自定义的数据类型


    // 通知子窗口关闭
    connect(this,SIGNAL(sigToKillAxisDialogWindow()),pwdsa,SLOT(slotKillMyself()));

    // 返回参数
    connect(pwdsa,SIGNAL(sigReturnDialogResult(CurveParameterForPanCoreData)),
            this,SLOT(slotReceiveDialog(CurveParameterForPanCoreData)));

    pwdsa->show();

    IsAxisDialogWindowOpen=true;
}


void panWindow::on_actionSet_Curve_Color_triggered()
{
    if(ISColorDialogWindowOpen)
    {
        emit sigToKillColorDialogWindow();
    }

    PanWindowDialogSetColor *pwdsc= new PanWindowDialogSetColor(CurveParameterForPanCore);

    qRegisterMetaType<CurveParameterForPanCoreData>("CurveParameterForPanCoreData"); //注册自定义的数据类型

    // 通知子窗口关闭
    connect(this,SIGNAL(sigToKillColorDialogWindow()),pwdsc,SLOT(slotKillMyself()));

    // 接收返回数据
    connect(pwdsc,SIGNAL(sigReturnDialogResult(CurveParameterForPanCoreData)),
            this,SLOT(slotReceiveDialog(CurveParameterForPanCoreData)));

    pwdsc->show();

    ISColorDialogWindowOpen=true;
}


void panWindow::on_actionSave_Pan_Genome_Data_triggered()
{
    emit sigSavePanGenomeData();
}



void panWindow::on_actionAbout_triggered()
{
    emit sigOpenAboutWindow();//打开about窗口
}

void panWindow::on_actionDraw_New_Gene_Curve_triggered()
{
    emit sigDrawNewGeneCurve(); // 画新基因的数据
    qDebug() << "plot new gene curve";
}

void panWindow::on_actionFit_Curve_Function_triggered()
{
    emit sigFitPanCore(11);//拟合
    qDebug() << "fit pan and core curve model";
}

// 显示boxplot
void panWindow::on_actionBoxplot_triggered()
{
    CurveParameterForPanCore.toShowBoxplot=true;
    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForPanCore);
}


// 显示散点图
void panWindow::on_actionScatter_Plot_triggered()
{
    CurveParameterForPanCore.toShowBoxplot=false;
    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForPanCore);
}

// 重写窗口关闭函数

void panWindow::closeEvent(QCloseEvent *)
{
    // 通知主线程自己即将关闭
    sigTellMaiWindowIAmClosed();

    // 关闭窗体
    close();
}

// 切换图像分辨率
void panWindow::on_action100_DPI_triggered()
{
    CurveParameterForPanCore.ImageDPI=100;
    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForPanCore);
}

void panWindow::on_action300_dpi_triggered()
{
    CurveParameterForPanCore.ImageDPI=300;
    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForPanCore);
}

void panWindow::on_action600_dpi_triggered()
{
    CurveParameterForPanCore.ImageDPI=600;
    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForPanCore);
}
