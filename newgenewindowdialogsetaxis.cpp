#include "newgenewindowdialogsetaxis.h"
#include "ui_newgenewindowdialogsetaxis.h"

NewGeneWindowDialogSetAxis::NewGeneWindowDialogSetAxis(CurveParameterForNewGeneData CurveParameterForNewGeneRef, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NewGeneWindowDialogSetAxis)
{
    ui->setupUi(this);

    //隐藏最大、最小按钮
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

// 传递来的参数
    CurveParameterForNewGene=CurveParameterForNewGeneRef;

// 坐标轴
    //默认标题
    ui->lineEditXTitle->setText(CurveParameterForNewGene.NewGeneXTitle);
    ui->lineEditYTitle->setText(CurveParameterForNewGene.NewGeneYTitle);

    //默认字号
    ui->comboBoxXTitleSize->setCurrentIndex(CurveParameterForNewGene.NewGeneXTitleSize-9);
    ui->comboBoxYTitleSize->setCurrentIndex(CurveParameterForNewGene.NewGeneYTitleSize-9);

    //默认字体
    ui->fontComboBoxXTitleFont->setCurrentFont(CurveParameterForNewGene.NewGeneXTitleFont);
    ui->fontComboBoxYTitleFont->setCurrentFont(CurveParameterForNewGene.NewGeneYTitleFont);

//刻度上的
    //设置字号
    ui->comboBoxXScaleSize->setCurrentIndex(CurveParameterForNewGene.NewGeneXScaleValueSize-9);
    ui->comboBoxYScaleSize->setCurrentIndex(CurveParameterForNewGene.NewGeneYScaleValueSize-9);

    //设置默认字体
    ui->fontComboBoxXScaleFont->setCurrentFont(CurveParameterForNewGene.NewGeneXScaleValueFont);
    ui->fontComboBoxYScaleFont->setCurrentFont(CurveParameterForNewGene.NewGeneYScaleValueFont);

}

NewGeneWindowDialogSetAxis::~NewGeneWindowDialogSetAxis()
{
    delete ui;
}

void NewGeneWindowDialogSetAxis::slotKillMyself()
{
    close();
}


void NewGeneWindowDialogSetAxis::on_pushButtonOK_clicked()
{
// 获取X Title
    CurveParameterForNewGene.NewGeneXTitle=ui->lineEditXTitle->text();
    CurveParameterForNewGene.NewGeneXTitleSize=ui->comboBoxXTitleSize->currentIndex()+9;
    CurveParameterForNewGene.NewGeneXTitleFont=ui->fontComboBoxXTitleFont->currentFont();
    CurveParameterForNewGene.NewGeneXScaleValueSize=ui->comboBoxXScaleSize->currentIndex()+9;
    CurveParameterForNewGene.NewGeneXScaleValueFont=ui->fontComboBoxXScaleFont->currentFont();


    qDebug() << "x current font: " << CurveParameterForNewGene.NewGeneXTitleFont;
    qDebug() << "x current size: " << CurveParameterForNewGene.NewGeneXTitleSize;


// 获取Y title
    CurveParameterForNewGene.NewGeneYTitle=ui->lineEditYTitle->text();
    CurveParameterForNewGene.NewGeneYTitleSize=ui->comboBoxYTitleSize->currentIndex()+9;
    CurveParameterForNewGene.NewGeneYTitleFont=ui->fontComboBoxYTitleFont->currentFont();
    CurveParameterForNewGene.NewGeneYScaleValueSize=ui->comboBoxYScaleSize->currentIndex()+9;
    CurveParameterForNewGene.NewGeneYScaleValueFont=ui->fontComboBoxYScaleFont->currentFont();

    emit sigReturnDialogResult(CurveParameterForNewGene);
    close();

}

// 关闭自己
void NewGeneWindowDialogSetAxis::on_pushButtonCancel_clicked()
{
    close();
}
