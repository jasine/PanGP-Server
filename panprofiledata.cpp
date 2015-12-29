#include "panprofiledata.h"

PanProfileData::PanProfileData()
{
}

void PanProfileData::NewGeneFitWithAvg()
{
/*
 *Model:
 *      Y = kX**m
 *  =>   ln(Y) = ln(k) + m*ln(X)
 *
 *      b = m;
 *      a = ln(k);


*/
    qDebug() << "fitting with pan avg data";
    qint32 Nsize=NewAvgValue.size();
    qint32 Xsum=0;
    qint32 Ysum=0;
    qint32 XYsum=0;
    qint32 XSQsum=0;
    qint32 YSQsum=0;



    for(qint32 i=0; i<Nsize;i++)
    {
        qreal x=qLn((i+NewGeneBeginValue)*1.0);
        qreal y=qLn(NewAvgValue[i]*1.0);

        qDebug() << x << "\t" << y;

        Xsum=Xsum + x;
        Ysum=Ysum + y;
        XYsum=XYsum + x*y;
        XSQsum=XSQsum + x*x;
    }
    qreal b= ( XYsum - (Xsum*Ysum*1.0/Nsize) ) / ( XSQsum - Xsum*Xsum*1.0/Nsize );
    qreal a= Ysum*1.0/Nsize - b * Xsum*1.0/Nsize;
    qreal k= qExp(a);
    qreal r= (XYsum - Xsum*Ysum*1.0/Nsize) / qSqrt( ( XSQsum - Xsum*Xsum*1.0/Nsize ) * ( YSQsum - Ysum*Ysum*1.0/Nsize ) );

    qDebug() << "b:" << b;
    qDebug() << "a:" << a;
    qDebug() << "k:" << k;
    qDebug() << "r:" << r;
}

