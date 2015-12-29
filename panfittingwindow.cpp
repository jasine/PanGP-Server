#include "panfittingwindow.h"
#include "ui_panfittingwindow.h"

PanFittingWindow::PanFittingWindow(FittingResultData FittingResultRef, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PanFittingWindow)
{
    ui->setupUi(this);

    FittingResult=FittingResultRef;

    //固定窗体大小
    this->setFixedSize(this->size());

    //隐藏最大、最小按钮
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |Qt::WindowCloseButtonHint);


    qDebug() << "entering pan fitting window";
    //qDebug() << "fittype:" << FittingResult.FitType;

/*
 *     ++++++++++++++++++++++++++++++++++++++++++++++++++
 *     +++++++++++++++   pan model A   ++++++++++++++++++
 *     ++++++++++++++++++++++++++++++++++++++++++++++++++
 */

    //model
    ui->labelPanModelA->setVisible(true);
    QMovie *PanMovie =new QMovie(":/image/panModel-1.gif");
    PanMovie->setScaledSize(QSize(100,20));
    ui->labelPanModelA->setMovie(PanMovie);
    PanMovie->start();

    //descciption
    ui->labelPanModelADescription->setVisible(true);
    QMovie *PanDesMovie = new QMovie(":/image/model_description1.gif");
    PanDesMovie->setScaledSize(QSize(140,50));
    ui->labelPanModelADescription->setMovie(PanDesMovie);
    PanDesMovie->start();


    QString textinfo="";
    //parameters
    textinfo = "A: " + QString::number(FittingResult.panM1A) +
            "   B: " + QString::number(FittingResult.panM1B) +
            "   C: " + QString::number(FittingResult.panM1C);
    ui->labelPanModelA_Parameters->setText(textinfo);

    //A interval
    textinfo = "A 95% Confidence Interval: " + QString::number(FittingResult.panM1A) + " ± " + QString::number(FittingResult.panM1A95Interval);
    ui->labelPanModelA_AValue95Interval->setText(textinfo);

    //C interval
    textinfo = "C 95% Confidence Interval: " + QString::number(FittingResult.panM1C) + " ± " + QString::number(FittingResult.panM1C95Interval);
    ui->labelPanModelA_CValue95Interval->setText(textinfo);

    //R squared
    textinfo = "R Squared: " + QString::number(FittingResult.panM1RSquared);
    ui->labelPanModelA_RSquared->setText(textinfo);


/*
 *     ++++++++++++++++++++++++++++++++++++++++++++++++++
 *     +++++++++++++++   core model A   ++++++++++++++++++
 *     ++++++++++++++++++++++++++++++++++++++++++++++++++
 */

// 显示core model
    ui->labelCoreModelA->setVisible(true);
    QMovie *CoreMovie =new QMovie(":/image/coreModel-1.gif");
    CoreMovie->setScaledSize(QSize(111,20));
    ui->labelCoreModelA->setMovie(CoreMovie);
    CoreMovie->start();


    //descciption
    ui->labelCoreModelADescription->setVisible(true);
    QMovie *CoreDesMovie = new QMovie(":/image/model_description2.gif");
    CoreDesMovie->setScaledSize(QSize(140,50));
    ui->labelCoreModelADescription->setMovie(CoreDesMovie);
    CoreDesMovie->start();


    //parameters
    textinfo = "A: " + QString::number(FittingResult.coreM1A) +
            "   B: " + QString::number(FittingResult.coreM1B) +
            "   C: " + QString::number(FittingResult.coreM1C);
    ui->labelCoreModelA_Parameters->setText(textinfo);

    //A interval
    textinfo = "A 95% Confidence Interval: " + QString::number(FittingResult.coreM1A) + " ± " + QString::number(FittingResult.coreM1A95Interval);
    ui->labelCoreModelA_AValue95Interval->setText(textinfo);

    //C interval
    textinfo = "C 95% Confidence Interval: " + QString::number(FittingResult.coreM1C) + " ± " + QString::number(FittingResult.coreM1C95Interval);
    ui->labelCoreModelA_CValue95Interval->setText(textinfo);

    //R squared
    textinfo = "R Squared: " + QString::number(FittingResult.coreM1RSquared);
    ui->labelCoreModelA_RSquared->setText(textinfo);




//label->setPixmap(QPixmap("./pic.jpg"));




}

PanFittingWindow::~PanFittingWindow()
{
    delete ui;
}


void PanFittingWindow::slotKillMyself()
{
    close();
}


