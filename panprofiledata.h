#ifndef PANPROFILEDATA_H
#define PANPROFILEDATA_H

#include <QList>
#include <qmath.h>
#include <QDebug>

class PanProfileData
{
public:
    PanProfileData();
    // 泛基因组拟合成员函数
    void NewGeneFitWithAvg();



    qint32 GenomeNumber;            //基因组数目
    QList<qint32> GenomeNumValue;   //记录每个组合中基因组数目
    QList<qint32> PanValue;         //泛基因大小
    QList<qint32> CoreValue;        //核心基因大小
    QList<double> PanAvgValue;      //泛基因组平均值
    QList<double> CoreAvgValue;     //核心基因平均值

    // Pan 中位数
    QList<double> PanValueQ1;       // 四分之一中位数
    QList<double> PanValueQ2;       // 中位数
    QList<double> PanValueQ3;       // 四分之三中位数
    QList<double> PanValueIQR;      // 四分位间距

    // Core 中位数
    QList<double> CoreValueQ1;
    QList<double> CoreValueQ2;
    QList<double> CoreValueQ3;
    QList<double> CoreValueIQR;

    qreal pancoreBoxplotYMax; // 泛基因组部分的boxplot数据的最大最小值
    qreal pancoreBoxplotYMin;


    bool IsNew;                     //新基因组是否存在数据
    QList<qint32> Newgi;            //新基因中的基因组数目
    QList<double> NewAvgValue;      //新基因的平均数

/*
    // New 中位数
    QList<double> NewValueQ1;
    QList<double> NewValueQ2;
    QList<double> NewValueQ3;
    QList<double> NewValueIQR;

    qreal newBoxplotYMax; //新基因部分的boxplot数据的最大最小值
    qreal newBoxplotYMin;

    bool IsNew;                     //新基因组是否存在数据
    QList<qint32> NewValue;         //新基因的数据
    QList<qint32> Newgi;            //新基因中的基因组数目
    QList<double> NewAvgValue;      //新基因的平均数
*/
    static const qint32 NewGeneBeginValue=2; // 新基因起始值1 or 2

};

#endif // PANPROFILEDATA_H
