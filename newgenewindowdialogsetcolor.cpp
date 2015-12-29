#include "newgenewindowdialogsetcolor.h"
#include "ui_newgenewindowdialogsetcolor.h"

NewGeneWindowDialogSetColor::NewGeneWindowDialogSetColor(CurveParameterForNewGeneData CurveParameterForNewGeneRef, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NewGeneWindowDialogSetColor)
{
    ui->setupUi(this);

    //隐藏最大、最小按钮
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

// 获取参数
    CurveParameterForNewGene=CurveParameterForNewGeneRef;


// 初始化颜色
    QVector <QColor> AllColor=CurveParameterForNewGene.AllColor;

 // 把颜色加到下拉菜单
    for(qint32 i=0;i<AllColor.size();i++)
    {
        QPixmap pixmap(20,20);
        pixmap.fill(AllColor[i]);
        ui->comboBoxBarColor->addItem(pixmap,"");
        ui->comboBoxTrendLineColor->addItem(pixmap,"");
    }

// 初始化窗口
    // 直方图
    ui->comboBoxBarColor->setCurrentIndex(CurveParameterForNewGene.BarColorID);

    // 趋势线
    ui->comboBoxTrendLineColor->setCurrentIndex(CurveParameterForNewGene.TrendLineColorID);

}

NewGeneWindowDialogSetColor::~NewGeneWindowDialogSetColor()
{
    delete ui;
}

void NewGeneWindowDialogSetColor::on_pushButtonCancel_clicked()
{
    close();
}

void NewGeneWindowDialogSetColor::slotKillMyself()
{
    close();
}

void NewGeneWindowDialogSetColor::on_pushButtonOK_clicked()
{
    // 获取直方图颜色
    CurveParameterForNewGene.BarColorID=ui->comboBoxBarColor->currentIndex();
    CurveParameterForNewGene.BarColor=CurveParameterForNewGene.AllColor[CurveParameterForNewGene.BarColorID];

    // 获取趋势线颜色
    CurveParameterForNewGene.TrendLineColorID=ui->comboBoxTrendLineColor->currentIndex();
    CurveParameterForNewGene.TrendLineColor=CurveParameterForNewGene.AllColor[CurveParameterForNewGene.TrendLineColorID];

    // 发信号
    emit sigReturnDialogResult(CurveParameterForNewGene);//颜色结束

    close();

}
