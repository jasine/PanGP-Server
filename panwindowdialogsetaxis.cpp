#include "panwindowdialogsetaxis.h"
#include "ui_panwindowdialogsetaxis.h"

PanWindowDialogSetAxis::PanWindowDialogSetAxis(CurveParameterForPanCoreData CurveParameterForPanCoreRef, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PanWindowDialogSetAxis)
{
    ui->setupUi(this);

    //隐藏最大、最小按钮
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    CurveParameterForPanCore=CurveParameterForPanCoreRef;

//坐标轴
    // 设置默认标题
    ui->lineEditXTitle->setText(CurveParameterForPanCore.PanCoreXTitle);
    ui->lineEditYTitle->setText(CurveParameterForPanCore.PanCoreYTitle);
    // 设置默认字号
    ui->comboBoxXTitleSize->setCurrentIndex(CurveParameterForPanCore.PanCoreXTitleSize-9);
    ui->comboBoxYTitleSize->setCurrentIndex(CurveParameterForPanCore.PanCoreYTitleSize-9);
    // 设置默认字体
    ui->fontComboBoxXTitleFont->setCurrentFont(CurveParameterForPanCore.PanCoreXTitleFont);
    ui->fontComboBoxYTitleFont->setCurrentFont(CurveParameterForPanCore.PanCoreYTitleFont);

//刻度上的
    // 设置字号
    ui->comboBoxXScaleSize->setCurrentIndex(CurveParameterForPanCore.PanCoreXScaleValueSize-9);
    ui->comboBoxYScaleSize->setCurrentIndex(CurveParameterForPanCore.PanCoreYScaleValueSize-9);
    //设置默认字体
    ui->fontComboBoxXScaleFont->setCurrentFont(CurveParameterForPanCore.PanCoreXScaleValueFont);
    ui->fontComboBoxYScaleFont->setCurrentFont(CurveParameterForPanCore.PanCoreYScaleValueFont);
}




PanWindowDialogSetAxis::~PanWindowDialogSetAxis()
{
    delete ui;
}

//关闭自己
void PanWindowDialogSetAxis::slotKillMyself()
{
    close();
}


void PanWindowDialogSetAxis::on_pushButtonCancel_clicked()
{
    close();
}


void PanWindowDialogSetAxis::on_pushButtonOK_clicked()
{
// 获取X Title
    CurveParameterForPanCore.PanCoreXTitle=ui->lineEditXTitle->text();
    CurveParameterForPanCore.PanCoreXTitleSize=ui->comboBoxXTitleSize->currentIndex()+9;
    CurveParameterForPanCore.PanCoreXTitleFont=ui->fontComboBoxXTitleFont->currentFont();
    CurveParameterForPanCore.PanCoreXScaleValueSize=ui->comboBoxXScaleSize->currentIndex()+9;
    CurveParameterForPanCore.PanCoreXScaleValueFont=ui->fontComboBoxXScaleFont->currentFont();


    qDebug() << "x current font: " << CurveParameterForPanCore.PanCoreXTitleFont;
    qDebug() << "x current size: " << CurveParameterForPanCore.PanCoreXTitleSize;


// 获取Y title
    CurveParameterForPanCore.PanCoreYTitle=ui->lineEditYTitle->text();
    CurveParameterForPanCore.PanCoreYTitleSize=ui->comboBoxYTitleSize->currentIndex()+9;
    CurveParameterForPanCore.PanCoreYTitleFont=ui->fontComboBoxYTitleFont->currentFont();
    CurveParameterForPanCore.PanCoreYScaleValueSize=ui->comboBoxYScaleSize->currentIndex()+9;
    CurveParameterForPanCore.PanCoreYScaleValueFont=ui->fontComboBoxYScaleFont->currentFont();



    emit sigReturnDialogResult(CurveParameterForPanCore);
    close();
}











