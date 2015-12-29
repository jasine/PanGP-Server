#include "newgenewindow.h"
#include "ui_newgenewindow.h"

newGeneWindow::newGeneWindow(QImage NewGeneCurveImageRef, CurveParameterForNewGeneData CurveParameterForNewGeneRef, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::newGeneWindow)
{
    ui->setupUi(this);

// 接收参数
    newGeneCurveImage=NewGeneCurveImageRef; // 图像
    CurveParameterForNewGene=CurveParameterForNewGeneRef; //  参数
    CurveParameterForNewGene.IsNewGeneWindowOpen=true;// 新基因的窗口已经打开了
    IsImagePrepared=true;


    //隐藏最小按钮
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |Qt::WindowMinimizeButtonHint |Qt::WindowCloseButtonHint);



    // 判断窗口是否打开了
    IsAxisDialogWindowOpen=false;
    ISColorDialogWindowOpen=false;
}

newGeneWindow::~newGeneWindow()
{
    delete ui;
}

// 关掉自己
void newGeneWindow::slotKillMyself()
{
    close();
}


//  窗口绘图
void newGeneWindow::paintEvent(QPaintEvent *)
{
    if(IsImagePrepared==false)
    {
        return;
    }

    //qDebug() << "go to paint";

    //将图像数据画到窗体上
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform,true);//平滑直线
    painter.scale(1.0/(CurveParameterForNewGene.ImageDPI/100.0),1.0/(CurveParameterForNewGene.ImageDPI/100.0));
    painter.drawImage(0,0,newGeneCurveImage);    

}


// 更新窗体上的图像
void newGeneWindow::slotUpdateImage(QImage NewGeneCurveImageRef)
{
    newGeneCurveImage=NewGeneCurveImageRef;
    qDebug() << "get update information";
    update();//更新窗体上的图像
}

// 提示窗口
void newGeneWindow::WarningMessage(QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setWindowTitle("Tips");
    msgBox.exec();
}


// 接受对话框修改信息，并往主程序发出修改图像的数据请求
void newGeneWindow::slotReceiveDialog(CurveParameterForNewGeneData CurveParameterForNewGeneRef)
{
    CurveParameterForNewGene=CurveParameterForNewGeneRef;
    //给主线程发送请求更新信号
    emit sigRequestUpdateImage(CurveParameterForNewGene);
}


void newGeneWindow::on_actionSet_Axis_Feature_triggered()
{
    if(IsAxisDialogWindowOpen==true)
    {
        emit sigToKillAxisDialogWindow();
    }

    NewGeneWindowDialogSetAxis *ngwdsa = new NewGeneWindowDialogSetAxis(CurveParameterForNewGene,this);

    qRegisterMetaType<CurveParameterForNewGeneData>("CurveParameterForNewGeneData"); //注册自定义的数据类型
    // 建立通道

    // 通知关闭
    connect(this,SIGNAL(sigToKillAxisDialogWindow()),ngwdsa,SLOT(slotKillMyself()));

    // 接受返回参数
    connect(ngwdsa,SIGNAL(sigReturnDialogResult(CurveParameterForNewGeneData)),this,SLOT(slotReceiveDialog(CurveParameterForNewGeneData)));

    ngwdsa->show();

    IsAxisDialogWindowOpen=true;
}




void newGeneWindow::on_actionSet_Curve_Color_triggered()
{
    if(ISColorDialogWindowOpen==true)
    {
        emit sigToKillColorDialogWindow();
    }

    NewGeneWindowDialogSetColor * ngwdsc = new NewGeneWindowDialogSetColor(CurveParameterForNewGene,this);

    qRegisterMetaType<CurveParameterForNewGeneData>("CurveParameterForNewGeneData"); //注册自定义的数据类型

    //建立通道

    //通知窗口关闭
    connect(this,SIGNAL(sigToKillColorDialogWindow()),ngwdsc,SLOT(slotKillMyself()));

    //接受返回参数
    connect(ngwdsc,SIGNAL(sigReturnDialogResult(CurveParameterForNewGeneData)),this,SLOT(slotReceiveDialog(CurveParameterForNewGeneData)));

    ngwdsc->show();
}



void newGeneWindow::on_actionExport_PNG_Image_triggered()
{
    QString ImageFileName=QFileDialog::getSaveFileName(this, tr("Save image with PNG format"),
                                                       "NewGene.png",
                                                       tr("Images (*.png)"));
    if(ImageFileName.isEmpty())
    {
        return;
    }
    newGeneCurveImage.save(ImageFileName,"PNG");
}

void newGeneWindow::on_actionExport_TIFF_Image_triggered()
{
    QString ImageFileName=QFileDialog::getSaveFileName(this, tr("Save image with TIFF format"),
                                                       "NewGene.tiff",
                                                       tr("Images (*.tiff)"));
    QImage tmpImage=newGeneCurveImage;
    QImageWriter writeImage;

    if(ImageFileName.isEmpty())
    {
        return;
    }

    writeImage.setFormat("tiff");
    writeImage.setFileName(ImageFileName);
    writeImage.setCompression(1);
    writeImage.write(tmpImage);
}



/*
// 捕获鼠标位置
void newGeneWindow::mousePressEvent(QMouseEvent *event)
{

    if(toCaptureCursorPos==false)
    {
        return;
    }

    QPoint mpos =event->pos();//获取坐标
    qreal x=mpos.x();
    qreal y=mpos.y();
    qDebug() << "mouse press position: x-> " << x << "\ty-> " << y;
    //this->setCursor(Qt::CrossCursor);

    CurveParameterForNewGene.toDrawNewGeneLegend=true;
    CurveParameterForNewGene.NewGeneLegendTopLeftX=mpos.x();
    CurveParameterForNewGene.NewGeneLegendTopLeftY=mpos.y();

    //还原鼠标样式
    this->setCursor(Qt::ArrowCursor);

    //关闭再次捕获
    toCaptureCursorPos=false;

    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForNewGene);
}*/

// 打开about窗口
void newGeneWindow::on_actionAbout_triggered()
{
    emit sigOpenAboutWindow();
}



void newGeneWindow::on_actionFit_Curve_Function_triggered()
{
    // 发送拟合曲线的信号
    emit sigFitNewGene(21);
}

//显示趋势线
void newGeneWindow::on_actionShowTrendLine_triggered()
{
    qDebug() << "show trend line";
    CurveParameterForNewGene.toShowTrendLine=true;
    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForNewGene);
}

//隐藏趋势线
void newGeneWindow::on_actionHideTrendLine_triggered()
{
    qDebug() << "hide trend line";
    CurveParameterForNewGene.toShowTrendLine=false;
    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForNewGene);
}

// 重写关闭函数

void newGeneWindow::closeEvent(QCloseEvent *)
{
    // 通知主窗体自己被关闭了
    sigTellMaiWindowIAmClosed();

    close();
}

// 切换分辨率
void newGeneWindow::on_action100_dpi_triggered()
{
    CurveParameterForNewGene.ImageDPI=100;
    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForNewGene);
}

void newGeneWindow::on_action300_dpi_triggered()
{
    CurveParameterForNewGene.ImageDPI=300;
    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForNewGene);
}

void newGeneWindow::on_action600_dpi_triggered()
{
    CurveParameterForNewGene.ImageDPI=600;
    //发更新图片请求信号
    emit sigRequestUpdateImage(CurveParameterForNewGene);
}
