#ifndef SAMPLETHREAD_H
#define SAMPLETHREAD_H

#include <QThread>
#include <QPainter>
#include <QHash>
#include <QString>
#include <QFile>
#include <QRegExp>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QTime>
#include <time.h>
#include <qmath.h>
#include <mainwindow.h>
#include <panprofiledata.h>
#include <sampleparameterdata.h>

class SampleThread : public QThread
{
    Q_OBJECT
public:
    explicit SampleThread(SampleParameterData SampleParameterRef,
                          QObject *parent = 0);
    ~SampleThread();
    
signals:
    void sigSampleThreadProcessMsg(QString message); //发出运行日志
    void sigSampleThreadFinished(PanProfileData PanProfileRef);//发出运行结束报告

public slots:
    void slotKillMyself();

private:
// 申明类变量
      // 传递来的参数
      qint32 DataType;
      QString InputFilePath;
      qint32 SampleMethod;
      qint64 SampleSize;
      qint32 SampleRepeat;
      qint32 AmpCoefficient;
      bool IsHeader;
      bool IsClusterID;

      //储存数据  --用于线程之间的传递
      PanProfileData PanProfile;
      bool IsNew;                     //新基因组是否存在数据

      //储存数据  --用于线程内计算
      QList<qint32> GenomeNumValue;
      QList<qint32> PanValue;
      QList<qint32> CoreValue;
      QList<double> PanAvgValue;
      QList<double> CoreAvgValue;
      QList<qint32> NewValue;
      QList<qint32> Newgi;
      QList<double> NewAvgValue;


      QList<QString> GeneMatrixData;
      QList<qint32> GeneMatrixDataSupA; // 记录每个cluster里面1的数目
      qint64 allcore;
      QTime timeStart, timeCounter, timeEnd;
      qlonglong timeCost;

      // 重复次数
      //const static qint32 SimulationTimes=10; // 抽样的重复次数

//函数声明
    void run();


//抽样部分的函数

    //函数声明
    void GenerateAllCombination(qint32 m, qint32 n, QList<QString> &AllCombination);
    //从m个数中选取n个数的所有组合

    qint64 CheckCombinationValue(qint32 m, qint32 n, qint64 SampleSize);
    //检查从m个数中选取n个数的总组合数是否大于SampleSize

    void ProduceRandomCombination(qint32 m,qint32 n, qint64 SampleSize, QList <QString> &RandomCombination);
    //随机产生从m个数中选取n个数的组合

    qint64 ReadMatrixFile(QString MatrixFile, QList<QString> &GeneMatrixData);
    //读取矩阵文件

    void CalculatePanGenome(qint64 allcore, QList <qint32> &GeneMatrixDataSupA, qint32 genomeNum, QList <QString> &PanCombination, QList <QString> &GeneMatrixData, QList <qint32> &PanData, QList <qint32> &CoreData);
    //计算泛基因组大小和核心基因组大小

    void AddCombinationRandomly(qint64 SampleSize, qint32 genomeNum, QList <QString> &RandomCombination, QList<QString> &PreComID);
    //随机增加的方式抽样

    void GenomeDiffByCluster(qint32 genomeNum, QList <QString> &GeneMatrixData,QVector <QVector <qint64> > &GenomeDiff);
    //计算各个基因组间的距离

    void FilterCombinationByDispersion(QList <QString> &AllCombination, QVector <QVector <qint64> > &GenomeDiff, qint64 SampleSize);
    //基于基因组间的离散度，来筛选离散度位于平均离散度附近的组合

    qint64 ReadClusterFile(QString ClusterFilePath, QList <QString> &GeneMatrixData, bool IsHeader, bool IsClusterID);
    //读取cluster文件

    void TraverseAll(QList <QString> &GeneMatrixData, qint64 allcore, QList <qint32> &GeneMatrixDataSupA, QList<qint32> &GenomeNumValue, QList<qint32> &PanValue, QList<qint32> &CoreValue, QList<double> &PanAvgValue, QList<double> &CoreAvgValue);
    //遍历所有组合

    void SamplingRandomlly(QList <QString> &GeneMatrixData ,qint64 allcore, QList <qint32> &GeneMatrixDataSupA, qint64 SampleSize, qint32 SampleRepeat, QList<qint32> &GenomeNumValue, QList<qint32> &PanValue, QList<qint32> &CoreValue, QList<double> &PanAvgValue, QList<double> &CoreAvgValue);
    //完全随机抽样

    void AddRandomly(QList <QString> &GeneMatrixData, qint64 allcore, QList <qint32> &GeneMatrixDataSupA, qint64 SampleSize, qint32 SampleRepeat, QList<qint32> &GenomeNumValue, QList<qint32> &PanValue, QList<qint32> &CoreValue, QList<double> &PanAvgValue, QList<double> &CoreAvgValue, QList<qint32> &NewValue,QList<qint32> &Newgi, QList<double> &NewAvgValue);
    // 随机增加

    void DistanceGuide(QList <QString> &GeneMatrixData, qint64 allcore, QList <qint32> &GeneMatrixDataSupA, qint64 SampleSize, qint32 SampleRepeat, QList<qint32> &GenomeNumValue, QList<qint32> &PanValue, QList<qint32> &CoreValue, QList<double> &PanAvgValue, QList<double> &CoreAvgValue);
    //距离引导的抽样

    void CalculateBoxplotData(QList<qint32> RawData, QList<qint32> GenomeNumData, QList<double> &Q1, QList<double> &Q2, QList<double> &Q3, QList<double> &IQR);
    // 计算boxplot相关的数据

    void CalculateBoxplotYMaxMinValue();
    // 计算boxplot的最大最小值(pan、core、new)

    void CalculateNewGeneData();
    // 计算新基因的数据

    void ReadPanGenomeData(QString PanGenomeDataFile, QList<qint32> &GenomeNumValue, QList<qint32> &PanValue, QList<qint32> &CoreValue, QList<double> &PanAvgValue, QList<double> &CoreAvgValue);
    // 读取泛基因组数据

    void AnalyzeMatrixData(QList <QString> &GeneMatrixData, QList <qint32> &GeneMatrixDataSupA);
    // 分析基因cluster矩阵，每个cluster里面1的数目

    void WarningMessage(QString message);
    // 警告信息

    QString FormatTime2Qstring(qlonglong timeCost);
    // 格式化时间

    void RadixSort(QList<qint32> &DataSet);
    // 基数排序

    void RadixSortReturnOrder(QList<qint32> &DataSet, QList<qint32> &Order);
    //返回排序后顺序的基数排序

    
};

#endif // SAMPLETHREAD_H
