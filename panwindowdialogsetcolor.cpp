#include "panwindowdialogsetcolor.h"
#include "ui_panwindowdialogsetcolor.h"

PanWindowDialogSetColor::PanWindowDialogSetColor(CurveParameterForPanCoreData CurveParameterForPanCoreRef, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PanWindowDialogSetColor)
{
    ui->setupUi(this);

    //隐藏最大、最小按钮
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);


// 获取参数
    CurveParameterForPanCore=CurveParameterForPanCoreRef;


// 初始化颜色
    QVector <QColor> AllColor=CurveParameterForPanCore.AllColor;

 // 把颜色加到下拉菜单
    for(qint32 i=0;i<AllColor.size();i++)
    {
        QPixmap pixmap(20,20);
        pixmap.fill(AllColor[i]);
        ui->comboBoxPanColor->addItem(pixmap,"");
        ui->comboBoxCoreColor->addItem(pixmap,"");
    }

// 初始化窗口
    ui->comboBoxPanColor->setCurrentIndex(CurveParameterForPanCore.PanColorID);
    ui->comboBoxCoreColor->setCurrentIndex(CurveParameterForPanCore.CoreColorID);
}

PanWindowDialogSetColor::~PanWindowDialogSetColor()
{
    delete ui;
}

//关闭自己
void PanWindowDialogSetColor::slotKillMyself()
{
    close();
}


void PanWindowDialogSetColor::on_pushButtonCancel_clicked()
{
    close();
}


void PanWindowDialogSetColor::on_pushButtonOK_clicked()
{
    CurveParameterForPanCore.PanColorID=ui->comboBoxPanColor->currentIndex();
    CurveParameterForPanCore.CoreColorID=ui->comboBoxCoreColor->currentIndex();
    CurveParameterForPanCore.PanColor =CurveParameterForPanCore.AllColor[CurveParameterForPanCore.PanColorID];
    CurveParameterForPanCore.CoreColor=CurveParameterForPanCore.AllColor[CurveParameterForPanCore.CoreColorID];

    qDebug() << "Pan color:" <<CurveParameterForPanCore.PanColor;

    // 发信号

    emit sigReturnDialogResult(CurveParameterForPanCore);//颜色结束

    close();

}
