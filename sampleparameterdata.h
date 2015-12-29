#ifndef SAMPLEPARAMETERDATA_H
#define SAMPLEPARAMETERDATA_H

#include <QList>
#include <QString>
#include <qmath.h>
#include <QDebug>

class SampleParameterData
{
public:
    SampleParameterData();

    qint32 DataType;
    QString InputDataPath;
    qint32 SamplingMethod;
    qint64 SampleSize;
    qint32 SampleRepeat;
    qint32 AmpCoefficient;
    bool Isheader;
    bool IsClusterID;

    /*
    qint32 DataType;
    QString InputDataPath;
    qint32 SamplingMethod; // 抽样方法
    qint64 SampleSize; //抽样大小
    qint32 SampleRepeat; // 抽样重复次数
    qint32 AmpCoefficient; // 放大系数
    bool Isheader; // 是否包含文件头
    bool IsClusterID; // 是否包含cluster ID
    */


};

#endif // SAMPLEPARAMETERDATA_H
