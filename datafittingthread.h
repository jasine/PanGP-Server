#ifndef DATAFITTINGTHREAD_H
#define DATAFITTINGTHREAD_H

#include <QObject>
#include <QThread>
#include <qmath.h>
#include <QList>
#include <QDebug>

#include <panprofiledata.h>
#include <fittingresultdata.h>


class DataFittingThread : public QThread
{
    Q_OBJECT
public:
    explicit DataFittingThread(PanProfileData PanProfileRef, FittingResultData FittingResultRef, QObject *parent = 0);

    ~DataFittingThread();

    PanProfileData panProfile; // 接收传输过来的泛基因组数据
    FittingResultData FittingResult;


    //小数的精度
    double FormatDecimalNumber(double f, qint32 n);

/*
 *
 *   统计基础函数相关
 *
 */

    static const double PI = 3.1415926536;

    // 公共变量
    QList<double> XData;
    QList<double> YData;
    qint32 XYNum;

    double SumX;
    double SumY;
    double SumXX;
    double SumYY;
    double SumXY;

    double sumSqDevX;
    double sumSqDevY;
    double sumSqDevXY;

    double slope;
    double intercept;
    double rSquared;

    double varianceIntercept;
    double varianceSlope;


    // 初始化 X、Y值
    void SetData(QList<double> XDataSource, QList<double> YDataSource);

    //求和X, Y, XY, XX, YY, XY
    void CalculateXYSum();

    //拟合
    bool Regress();

    //计算R值
    void RSquared();

    //计算predictedYs
    void PredictedYs(QList<double> &predictedYValue);

    //计算 Intercept 和 Slope 的 方差variance
    void VarianceOfEstimates();

    // t probabilities
    double tProbability(qint32 n, double x);

    // t distribution
    double tDistribution(qint32 n, double p);

    // u distribution
    double uDistribution(double p);

    
signals:
    void sigFittingFinished(FittingResultData FittingResultRef);//返回拟合结果的信号

private:
    void run();

    
public slots:
    void slotKillMyself();
    
};

#endif // DATAFITTINGTHREAD_H
