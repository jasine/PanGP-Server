#ifndef FITTINGRESULTDATA_H
#define FITTINGRESULTDATA_H

#include <QList>
#include <qmath.h>
#include <QDebug>

class FittingResultData
{
public:
    FittingResultData();


        qint32 FitType;

    /*
     * 泛基因组拟合结果
     *
     * 模型 1):y=A*x**B +C
     *
     */

        bool IsPanCoreFittingFinished;
        double panM1A;
        double panM1B;
        double panM1C;
        double panM1RSquared;
        double panM1A95Interval;
        double panM1C95Interval;
        double panM1VarianceIntercept;
        double panM1VarianceSlope;


    /*
     * 核心基因组拟合结果
     *
     * 模型 1):y=A*exp(B*x) +C
     *
     */

        double coreM1A;
        double coreM1B;
        double coreM1C;
        double coreM1RSquared;
        double coreM1A95Interval;
        double coreM1C95Interval;
        double coreM1VarianceIntercept;
        double coreM1VarianceSlope;

    /*
     * 新基因组拟合结果
     *
     * 模型 1):y=A*x**B
     *
     */

        bool IsNewFittingFinished;
        double newM1A;
        double newM1B;
        double newM1RSquared;
        double newM1A95IntervalMin;
        double newM1A95IntervalMax;
        double newM1B95Interval;
        double newM1VarianceIntercept;
        double newM1VarianceSlope;



};

#endif // FITTINGRESULTDATA_H
