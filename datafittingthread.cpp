#include "datafittingthread.h"

DataFittingThread::DataFittingThread(PanProfileData PanProfileRef, FittingResultData FittingResultRef, QObject *parent) :
    QThread(parent)
{
    // 接收传递过来的数据
    panProfile=PanProfileRef;
    FittingResult=FittingResultRef;
}

DataFittingThread::~DataFittingThread()
{
    wait();
}


void DataFittingThread::slotKillMyself()
{
    qDebug() << "Fitting Thread was killed!";
    terminate();
}


void DataFittingThread::run()
{
    if(FittingResult.FitType == 10 ||  FittingResult.FitType == 11) // pan core 的拟合
    {

        /*
         * +++++++++++++++++++++++++++++++++++++++++++++++++++++++
         * +++++++++++++++   Pan Model A    ++++++++++++++++++++++
         * +++++++++++++++++++++++++++++++++++++++++++++++++++++++
         */

        // pan-genome 部分 Model 1: y=A*x**B +C
        QList<double> XConvertedData;
        QList<double> YConvertedData;
        double RMax = 0;
        double BStepValue=0.001;
        double BValue=1;

        YConvertedData.clear();
        YConvertedData=panProfile.PanAvgValue;

        bool goon = true;
        while(goon && BValue>-5)
        {
            if(BValue==0)
            {
                BValue=BValue-BStepValue;
            }

            //qDebug() << "=================== Round B:" << BValue << " ====================";

            //转换Y的数值
            XConvertedData.clear();
            for(qint32 i=1; i<=panProfile.GenomeNumber;i++)
            {
                double tmpXValue = pow(i,BValue);
                XConvertedData.push_back(tmpXValue);
            }

            //拟合
            SetData(XConvertedData,YConvertedData);
            Regress();
            RSquared();
            VarianceOfEstimates();

            //比较R值
            //qDebug() << "RMax:" << RMax << " \t R: " << rSquared ;
            if(rSquared >= RMax)
            {
                RMax=rSquared;
                FittingResult.panM1RSquared=rSquared;
                FittingResult.panM1A=slope;
                FittingResult.panM1B=BValue;
                FittingResult.panM1C=intercept;
                FittingResult.panM1VarianceIntercept=varianceIntercept; // C
                FittingResult.panM1VarianceSlope=varianceSlope; // A

                // B 的值下降一个步长
                BValue=BValue-BStepValue;
            }else
            {
                goon=false;
            }
        }
        // 优化小数点位数
        FittingResult.panM1A=FormatDecimalNumber(FittingResult.panM1A,2);
        FittingResult.panM1B=FormatDecimalNumber(FittingResult.panM1B,2);
        FittingResult.panM1C=FormatDecimalNumber(FittingResult.panM1C,2);
        FittingResult.panM1A95Interval = FormatDecimalNumber(tDistribution( (panProfile.PanAvgValue.size()-2),0.025)*sqrt(FittingResult.panM1VarianceSlope),2);
        FittingResult.panM1C95Interval = FormatDecimalNumber(tDistribution( (panProfile.PanAvgValue.size()-2),0.025)*sqrt(FittingResult.panM1VarianceIntercept),2);


        /*
         * +++++++++++++++++++++++++++++++++++++++++++++++++++++++
         * +++++++++++++++   Core Model A    +++++++++++++++++++++
         * +++++++++++++++++++++++++++++++++++++++++++++++++++++++
         */

        //core-genome 部分
        RMax=0;
        BValue=0;
        YConvertedData.clear();
        YConvertedData=panProfile.CoreAvgValue;

        goon = true;
        while(goon && BValue>-50)
        {
            if(BValue==0)
            {
                BValue=BValue-BStepValue;
            }

            //qDebug() << "=================== Round B:" << BValue << " ====================";

            //转换Y的数值
            XConvertedData.clear();
            for(qint32 i=1; i<=panProfile.GenomeNumber;i++)
            {
                double tmpXValue =qExp(BValue * i );;
                XConvertedData.push_back(tmpXValue);
            }

            //拟合
            SetData(XConvertedData,YConvertedData);
            Regress();
            RSquared();
            VarianceOfEstimates();

            //比较R值
            //qDebug() << "RMax:" << RMax << " \t R: " << rSquared ;
            if(rSquared >= RMax)
            {
                RMax=rSquared;
                FittingResult.coreM1RSquared=rSquared;
                FittingResult.coreM1A=slope;
                FittingResult.coreM1B=BValue;
                FittingResult.coreM1C=intercept;
                FittingResult.coreM1VarianceIntercept=varianceIntercept; // C
                FittingResult.coreM1VarianceSlope=varianceSlope; // A

                // B 的值下降一个步长
                BValue=BValue-BStepValue;
            }else
            {
                goon=false;
            }
        }

        // 优化小数点位数
        FittingResult.coreM1A=FormatDecimalNumber(FittingResult.coreM1A,2);
        FittingResult.coreM1B=FormatDecimalNumber(FittingResult.coreM1B,2);
        FittingResult.coreM1C=FormatDecimalNumber(FittingResult.coreM1C,2);

        FittingResult.coreM1A95Interval = FormatDecimalNumber(tDistribution( (panProfile.CoreAvgValue.size()-2),0.025)*sqrt(FittingResult.coreM1VarianceSlope),2);
        FittingResult.coreM1C95Interval = FormatDecimalNumber(tDistribution( (panProfile.CoreAvgValue.size()-2),0.025)*sqrt(FittingResult.coreM1VarianceIntercept),2);


        qDebug() << "+++++++++++++++++ core-genome fitting ++++++++++++++";

        qDebug() << "intercept : " << FittingResult.panM1C;
        qDebug() << "slope : " << FittingResult.panM1A;
        qDebug() << "R: " << FittingResult.panM1RSquared;
        qDebug() << "varianceIntercept: " << varianceIntercept;
        qDebug() << "varianceSlope: " << varianceSlope;
        qDebug() << "\n\n";

    }


    // 2 新基因的拟合
    if(FittingResult.FitType == 20 ||  FittingResult.FitType == 21) //new gene 的拟合
    {

    /*
     * +++++++++++++++++++++++++++++++++++++++++++++++++++++++
     * +++++++++++++++   New Model A    +++++++++++++++++++++
     * +++++++++++++++++++++++++++++++++++++++++++++++++++++++
     */
        QList<double> XConvertedData;
        QList<double> YConvertedData;

        for(qint32 i=2; i<=panProfile.GenomeNumber;i++)
        {
            XConvertedData.push_back(qLn(i));
            YConvertedData.push_back(qLn(panProfile.PanAvgValue[i-1]-panProfile.PanAvgValue[i-2]));
        }

        SetData(XConvertedData,YConvertedData);
        Regress();
        RSquared();
        VarianceOfEstimates();

        FittingResult.newM1RSquared=rSquared;
        FittingResult.newM1A=qExp(intercept);
        FittingResult.newM1A95IntervalMin=FormatDecimalNumber((qExp(intercept - tDistribution( (panProfile.GenomeNumber-3),0.025)*sqrt(varianceIntercept))),2);
        FittingResult.newM1A95IntervalMax=FormatDecimalNumber((qExp(intercept + tDistribution( (panProfile.GenomeNumber-3),0.025)*sqrt(varianceIntercept))),2);
        FittingResult.newM1B=FormatDecimalNumber(slope,2);
        FittingResult.newM1B95Interval=FormatDecimalNumber(tDistribution( (panProfile.GenomeNumber-3),0.025)*sqrt(varianceSlope),2);

        FittingResult.IsNewFittingFinished=true; // 新基因拟合完成

    }

    emit sigFittingFinished(FittingResult);
    qDebug() << "fitting finished";


}



double DataFittingThread::FormatDecimalNumber(double f, qint32 n)
{
    qint32 tmpValue= pow(10,n);
    qint32 d= f * tmpValue;
    double re= d*1.0/(tmpValue*1.0);
    return re;
}


/********************************************************************
 *
 * 以下是统计的基础函数（包括线性拟合、统计分布）
 *
 ********************************************************************/

//初始化参数
void DataFittingThread::SetData(QList<double> XDataSource, QList<double> YDataSource)
{
    SumX=0.0;
    SumY=0.0;
    SumXY=0.0;
    SumXX=0.0;
    SumYY=0.0;

    XData=XDataSource;
    YData=YDataSource;
    XYNum=XData.size();
}

//求和，X,Y,XX,YY,XY
void DataFittingThread::CalculateXYSum()
{
    for(qint32 i=0; i<XYNum; i++)
    {
        SumX=SumX + XData[i];
        SumY=SumY + YData[i];
        SumXY=SumXY + XData[i]*YData[i];
        SumXX=SumXX + XData[i]*XData[i];
        SumYY=SumYY + YData[i]*YData[i];
    }
}


//拟合
bool DataFittingThread::Regress()
{
    bool regressOK=true;
    CalculateXYSum();//求各种和

    sumSqDevX = SumXX - SumX*SumX/XYNum;

    if(sumSqDevX !=0)
    {
        sumSqDevY = SumYY - SumY*SumY/XYNum;
        sumSqDevXY = SumXY - SumX*SumY/XYNum;
        slope = sumSqDevXY/sumSqDevX;
        intercept = (SumY - slope*SumX)/XYNum;
    }else
    {
        regressOK=false;
    }

    return regressOK;

}

//计算R-Squared

void DataFittingThread::RSquared()
{
    double denom;
    denom = sumSqDevX * sumSqDevY;
    //qDebug() << "sumSqDevX:" << sumSqDevX;
    //qDebug() << "sumSqDevY:" << sumSqDevY;
    //qDebug() << "sumSqDevXY:" << sumSqDevXY;
    if(denom ==0)
    {
        rSquared = 1.0;
    }else
    {
        rSquared = sumSqDevXY*sumSqDevXY/denom;
    }
}


//计算预测的Y值
void DataFittingThread::PredictedYs(QList<double> &predictedYValue)
{
    for(qint32 i=0; i< XYNum; i++)
    {
        double tmpValue;
        tmpValue = intercept + slope * XData[i];
        predictedYValue.push_back(tmpValue);
    }
}


//计算 Intercept 和 Slope 的 方差variance

void DataFittingThread::VarianceOfEstimates()
{
    QList<double> predictedYValue;
    double s=0.0;
    double sx=0.0;
    double sxx=0.0;
    double denominator;

    PredictedYs(predictedYValue);//预测Y值

    for(qint32 i=0; i< XYNum ; i++)
    {
        double variance = (predictedYValue[i] - YData[i]) * (predictedYValue[i] - YData[i]);

        if(variance !=0)
        {
            s = s + 1.0/variance;
            sx = sx + XData[i]/variance;
            sxx = sxx + XData[i]*XData[i]/variance;
        }
    }

    denominator = s * sxx - sx * sx;
    if(denominator !=0)
    {
        varianceIntercept = sxx/denominator;
        varianceSlope = s/denominator;
    }else
    {
        //这地方应该报错了
        varianceIntercept=1e9;
        varianceSlope=1e9;
    }
}


double DataFittingThread::uDistribution(double p)
{
    double y = 0 - log(4 * p * (1-p) );
    double x = qSqrt(
                y * (1.570796288
                  + y * (0.03706987906
                    + y * (-0.8364353589E-3
                      + y *(-0.2250947176E-3
                        + y * (0.6841218299E-5
                          + y * (0.5824238515E-5
                            + y * (-0.104527497E-5
                              + y * (0.8360937017E-7
                                + y * (-0.3231081277E-8
                                  + y * (0.3657763036E-10
                                    + y *0.6936233982E-12)))))))))));
    if(p > 0.5)
    {
        x= 0-x;
    }
    return x;
}



double DataFittingThread::tDistribution(qint32 n, double p)
{

    if(p>1 || p<=0)
    {
        //该报错了；
    }

    if(p==0.5)
    {
        return 0;
    }else
    {
        if(p<0.5)
        {
            return (0 - tDistribution(n, 1-p));
        }
    }


    double u = uDistribution(p);
    double u2 = u * u;

    double a = (u2 + 1) /4;
    double b = ((5 * u2 +16)*u2 + 3 ) /96;
    double c = (((3*u2 + 19)*u2 + 17)* u2 - 15) /384;
    double d = ((((79 * u2 + 776) * u2 + 1482) * u2 - 1920) * u2 - 945) /92160;
    double e = (((((27 * u2 + 339) * u2 + 930) * u2 - 1782) * u2 - 765) * u2 + 17955) /368640;
    double x = u * (1 + (a + (b + (c + (d + e / n) / n) / n) / n) / n);

    if( n <= log10(p) * log10(p) + 3 )
    {
        double round;

        do {
            double p1 = tProbability(n,x);
            qint32 n1 = n +1;
            double delta = (p1 -p)/ qExp((n1 * log(n1 / (n + x * x)) + log(n/n1/2/PI) - 1 + (1/n1 - 1/n) / 6) / 2);
            x = x + delta;

            /*
             * perl 中原来的写法：
             * $round = sprintf("%.".abs(int(log10(abs $x)-4))."f",$delta);
             */
            qint32 jingdu = abs( (qint32) (log10(abs(x)) - 4) );
            round = ((qint64) ( delta * pow(10,jingdu)))*1.0/pow(10,jingdu);

        }while( x && (round!=0) );
    }

    return x;
}


double DataFittingThread::tProbability(qint32 n, double x)
{
    double a;
    double b;

    double w = qAtan2(x/qSqrt(n), 1);
    double z = qCos(w) * qCos(w);
    double y=1.0;

    for(qint32 i=n-2; i>=2; i=i-2)
    {
        y=1 + (i-1)/i*z*y;
    }

    if( (n%2)==0 )
    {
        a=qSin(w)/2;
        b=0.5;
    }else
    {
        if( n==1 )
        {
            a = 0;
        }else
        {
            a = qSin(w)*qCos(w)/PI;
        }

        b=0.5 + w/PI;
    }

    if( (1 - b - a*y )>0 )
    {
        return (1 - b - a*y);
    }else
    {
        return 0.0;
    }
}












