#include "newfittingwindow.h"
#include "ui_newfittingwindow.h"

NewFittingWindow::NewFittingWindow(FittingResultData FittingResultRef, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NewFittingWindow)
{
    ui->setupUi(this);

    //固定窗体大小
    this->setFixedSize(this->size());

    //隐藏最大、最小按钮
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |Qt::WindowCloseButtonHint);


    FittingResult=FittingResultRef;


    /*
     *     ++++++++++++++++++++++++++++++++++++++++++++++++++
     *     +++++++++++++++   new model A   ++++++++++++++++++
     *     ++++++++++++++++++++++++++++++++++++++++++++++++++
     */

        //model
        ui->labelNewModelA->setVisible(true);
        QMovie *NewMovie =new QMovie(":/image/newModel-1.gif");
        NewMovie->setScaledSize(QSize(68,20));
        ui->labelNewModelA->setMovie(NewMovie);
        NewMovie->start();

        //descciption
        ui->labelNewModelADescription->setVisible(true);
        QMovie *NewDesMovie = new QMovie(":/image/model_description3.gif");
        NewDesMovie->setScaledSize(QSize(127,50));
        ui->labelNewModelADescription->setMovie(NewDesMovie);
        NewDesMovie->start();


        QString textinfo="";
        //parameters
        textinfo = "A: " + QString::number(FittingResult.newM1A) +
                "   B: " + QString::number(FittingResult.newM1B);
        ui->labelNewModelA_Parameters->setText(textinfo);

        //A interval
        textinfo = "A 95% Confidence Interval: " + QString::number(FittingResult.newM1A95IntervalMin) + " ≤ A ≤ " + QString::number(FittingResult.newM1A95IntervalMax);
        ui->labelNewModelA_AValue95Interval->setText(textinfo);

        //B interval
        textinfo = "B 95% Confidence Interval: " + QString::number(FittingResult.newM1B) + " ± " + QString::number(FittingResult.newM1B95Interval);
        ui->labelNewModelA_BValue95Interval->setText(textinfo);

        //R squared
        textinfo = "R Squared: " + QString::number(FittingResult.newM1RSquared);
        ui->labelNewModelA_RSquared->setText(textinfo);



}

NewFittingWindow::~NewFittingWindow()
{
    delete ui;
}


void NewFittingWindow::slotKillMyself()
{
    qDebug() << "New Gene fitting windows is going to close.";
    close();
}







