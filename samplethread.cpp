#include "samplethread.h"

SampleThread::SampleThread(SampleParameterData SampleParameterRef, QObject *parent) :
    QThread(parent)
{

    // 获取传递来的参数
    DataType=SampleParameterRef.DataType;
    InputFilePath=SampleParameterRef.InputDataPath;
    SampleMethod=SampleParameterRef.SamplingMethod;
    SampleSize=SampleParameterRef.SampleSize;
    SampleRepeat=SampleParameterRef.SampleRepeat;
    AmpCoefficient=SampleParameterRef.AmpCoefficient;
    IsHeader=SampleParameterRef.Isheader;
    IsClusterID=SampleParameterRef.Isheader;


    qDebug() << "DataType:" << DataType;
    qDebug() << "InputFilePath:" << InputFilePath;
    qDebug() << "SampleMethod:" << SampleMethod;
    qDebug() << "SampleSize:" << SampleSize;
    qDebug() << "SampleRepeat:" << SampleRepeat;
    qDebug() << "AmpCoefficient:" << AmpCoefficient;
    qDebug() << "IsHeader:" << IsHeader;
    qDebug() << "IsClusterID:" << IsClusterID;



}

SampleThread::~SampleThread()
{
    wait();
}

void SampleThread::slotKillMyself()
{
    qDebug() << "Sampling Thread was killed!!";
    terminate();
}


void SampleThread::run()
{

// 报告输入参数开始
    sigSampleThreadProcessMsg("The parameters for current process:\n");
    timeCounter.start();

    if(DataType==1 || DataType==2)
    {
     // 数据类型
        if(DataType==1)
        {
            sigSampleThreadProcessMsg("Data Type: 0/1 Matrix File");
        }

        if(DataType==2)
        {
            sigSampleThreadProcessMsg("Data Type: Gene Cluster File");
            if(IsHeader)
            {
                sigSampleThreadProcessMsg("\tFile include Header: Yes");
            }else
            {
                sigSampleThreadProcessMsg("\tFile include Header: No");
            }

            if(IsClusterID)
            {
                sigSampleThreadProcessMsg("\tFile include ClusterID: Yes");
            }else
            {
                sigSampleThreadProcessMsg("\tFile include ClusterID: No");
            }
        }

    //抽样方法
        if(SampleMethod==0)
        {
            sigSampleThreadProcessMsg("Sample Method: Traverse All Combinations");
        }

        if(SampleMethod==1)
        {
            sigSampleThreadProcessMsg("Sample Method: Totally Random");
        }

        if(SampleMethod==2)
        {
            sigSampleThreadProcessMsg("Sample Method: Distance Guide");
        }

        if(SampleMethod>0)
        {
            //抽样大小
            QString msg="Sample Size:" + QString::number(SampleSize)+"\n";
                    msg=msg + "Sample Repeat:" + QString::number(SampleRepeat)+"\n";

            //放大系数
            if(SampleMethod==2)
            {
                msg=msg + "Amplification Coefficient:" + QString::number(AmpCoefficient) + "\n";
            }

            sigSampleThreadProcessMsg(msg);


        }else
        {
            QString msg="\n";
            sigSampleThreadProcessMsg(msg);
        }

    }

    if(DataType==3)
    {
        sigSampleThreadProcessMsg("Data Type: Pan-Genome Data File\n");
    }

// 报告输入参数结束



// 读取数据
    if(DataType==1) //读取矩阵数据
    {
        emit sigSampleThreadProcessMsg("Reading 0/1 matrix file from " + InputFilePath + "\n");
        allcore= ReadMatrixFile(InputFilePath,GeneMatrixData);
    }

    if(DataType==2) //读取cluster数据
    {
        emit sigSampleThreadProcessMsg("Reading gene cluster file from " + InputFilePath + "\n");
        allcore= ReadClusterFile(InputFilePath,GeneMatrixData,IsHeader,IsClusterID);
    }

    if(DataType==3)//读取panData
    {
        emit sigSampleThreadProcessMsg("Reading pan-genome data file from " + InputFilePath + "\n");
        ReadPanGenomeData(InputFilePath,GenomeNumValue,PanValue,CoreValue,PanAvgValue,CoreAvgValue);
        qDebug() << "read pan-genome data finished";
    }

// 分析matrix 数据, 计算每个cluster里面1的个数
    if(DataType==1 || DataType==2)
    {
        AnalyzeMatrixData(GeneMatrixData,GeneMatrixDataSupA);
    }


    // 汇报基因组数目
    if(DataType==1 || DataType==2)
    {
        emit sigSampleThreadProcessMsg("There are " + QString::number(GeneMatrixData[0].length()) + " genomes in the input data.");
    }
    if(DataType==3)
    {
        emit sigSampleThreadProcessMsg("There are " + QString::number(PanAvgValue.size()) + " genomes in the input data.");
    }



// 抽样并计算泛基因组大小
    if(DataType==1 || DataType==2)
    {
        if(SampleMethod==0) // 不抽样，遍历所有
        {
            emit sigSampleThreadProcessMsg("Sampling with Traverse All Combinations algorithm ...");
            emit sigSampleThreadProcessMsg("There would be " +QString::number(GeneMatrixData[0].length())+" cycles for current job");
            TraverseAll(GeneMatrixData,allcore,GeneMatrixDataSupA,GenomeNumValue,PanValue,CoreValue,PanAvgValue,CoreAvgValue);
        }

        if(SampleMethod==1) //完全随机抽样
        {
            emit sigSampleThreadProcessMsg("Sampling with totally random algorithm ...");
            emit sigSampleThreadProcessMsg("There would be " +QString::number(GeneMatrixData[0].length())+" cycles for current job");
            SamplingRandomlly(GeneMatrixData,allcore,GeneMatrixDataSupA,SampleSize,SampleRepeat,GenomeNumValue,
                             PanValue,CoreValue,PanAvgValue,CoreAvgValue);
        }

        /*
        if(SampleMethod==2) //随机增加
        {
            emit sigSampleThreadProcessMsg("Sampling with add randomlly method ...");
            AddRandomly(GeneMatrixData,allcore,SampleSize,SampleRepeat,GenomeNumValue,PanValue,CoreValue,
                       PanAvgValue,CoreAvgValue,NewValue,Newgi,NewAvgValue);
        }*/

        if(SampleMethod==2) // 基于引导树的抽样
        {
            emit sigSampleThreadProcessMsg("Sampling with diatance guide algorithm ...");
            emit sigSampleThreadProcessMsg("There would be " +QString::number(GeneMatrixData[0].length())+" cycles for current job");
            DistanceGuide(GeneMatrixData,allcore,GeneMatrixDataSupA,SampleSize,SampleRepeat,GenomeNumValue,PanValue,
                          CoreValue,PanAvgValue,CoreAvgValue);
        }
    }

    //封装返回数据
    qDebug() << "trasfer data into panprofile class";
   PanProfile.GenomeNumber=PanAvgValue.size();
   PanProfile.GenomeNumValue=GenomeNumValue;
   PanProfile.PanValue=PanValue;
   PanProfile.CoreValue=CoreValue;
   PanProfile.PanAvgValue=PanAvgValue;
   PanProfile.CoreAvgValue=CoreAvgValue;

    // 计算四分位数
    qDebug() << "pan's Quartile";
    emit sigSampleThreadProcessMsg("Calculating quartile from pan-genome result ... ");
    CalculateBoxplotData(PanProfile.PanValue,PanProfile.GenomeNumValue,PanProfile.PanValueQ1,PanProfile.PanValueQ2,PanProfile.PanValueQ3,PanProfile.PanValueIQR);

    // core data
    qDebug() << "core's Quartile";
    emit sigSampleThreadProcessMsg("Calculating quartile from core genome result ... ");
    CalculateBoxplotData(PanProfile.CoreValue,PanProfile.GenomeNumValue,PanProfile.CoreValueQ1,PanProfile.CoreValueQ2,PanProfile.CoreValueQ3,PanProfile.CoreValueIQR);

    // 四分位数部分的最大最小值
    CalculateBoxplotYMaxMinValue();

    // 涉及新基因的问题
    PanProfile.IsNew=true;
    CalculateNewGeneData();

    //程序运行结束
    emit sigSampleThreadProcessMsg("Sampling finished");

    timeCost = timeCounter.elapsed(); //消耗的时间 ms

    emit sigSampleThreadProcessMsg("Time cost: " + FormatTime2Qstring(timeCost));

    // 返回数据
    emit sigSampleThreadFinished(PanProfile);
}



//程序正文

void SampleThread::ReadPanGenomeData(QString PanGenomeDataFile, QList<qint32> &GenomeNumValue, QList<qint32> &PanValue, QList<qint32> &CoreValue, QList<double> &PanAvgValue, QList<double> &CoreAvgValue)
{
    QFile PGDFHand(PanGenomeDataFile);
    PGDFHand.open(QIODevice::ReadOnly);
    qint32 GenomeNumber = 0;
    QHash <qint32,qlonglong> PanCom;
    QHash <qint32,qlonglong> CoreCom;
    QHash <qint32,qlonglong> ComNum;

    QString line;

    line = PGDFHand.readLine();

    qDebug() << "Section A";

    while(!PGDFHand.atEnd())
    {
        QList <QString> row;
        qint32 genomeNum,pan,core;

        line=PGDFHand.readLine();
        line.replace("\n","");
        line.replace("\r","");

        row=line.split("\t");
        if(row.size()!=3 && row.size()!=4)
        {
            WarningMessage("Data format error!");
            exit(0);
        }


        // 数据格式转换
        genomeNum=row.at(0).toInt();
        pan=row.at(1).toInt();
        core=row.at(row.size()-1).toInt();

        // 计算总基因组数目
        if(GenomeNumber<genomeNum)
        {
            GenomeNumber=genomeNum;
        }

        // 用于计算平均数
        if(PanCom.contains(genomeNum) == false)
        {
            PanCom[genomeNum]=pan;
            CoreCom[genomeNum]=core;
            ComNum[genomeNum]=1;
        }else
        {
            PanCom[genomeNum]=PanCom[genomeNum]+pan;
            CoreCom[genomeNum]=CoreCom[genomeNum]+core;
            ComNum[genomeNum]=ComNum[genomeNum]+1;
        }

        // 存储数据
        GenomeNumValue.push_back(genomeNum);
        PanValue.push_back(pan);
        CoreValue.push_back(core);
    }

    qDebug() << "Section B";

    // 判断数据是否齐全
    for(qint32 i=1; i<=GenomeNumber; i++)
    {
        qreal panAvgtmp,coreAvgtmp;
        if(PanCom.contains(i)==false)
        {
            WarningMessage("Data is incomplete.");
            exit(0);
        }
        panAvgtmp=PanCom[i]*1.0/ComNum[i];
        coreAvgtmp=CoreCom[i]*1.0/ComNum[i];

        //存储数据
        PanAvgValue.push_back(panAvgtmp);
        CoreAvgValue.push_back(coreAvgtmp);
    }
    PGDFHand.close();
    qDebug() << "Section C";
}



void SampleThread::GenerateAllCombination(qint32 m, qint32 n, QList <QString> &AllCombination)
{
    QString opString="";
    QString pattern="10";

    // 创建起始字符串
    for(qint32 i=0;i<n;i++)
    {
        opString="1" + opString;
    }
    for(qint32 i=0;i<(m-n);i++)
    {
        opString=opString + "0";
    }

    AllCombination.push_back(opString);
    while(opString.indexOf(pattern) != -1)
    {
        qint32 pos = opString.indexOf(pattern);
        QString leftString=opString.mid(0,pos);
        QString rightString=opString.mid(pos+2,opString.length()-2-pos);
        qint32 left0Num=0;
        qint32 left1Num=0;

        //计算左边的1和0的数目
        for(qint32 i=0;i<pos;i++)
        {
            if(leftString[i]=='0')
            {
                left0Num++;
            }else
            {
                left1Num++;
            }
        }

        // 重新构造左边的字符串
        leftString="";
        for(qint32 i=0;i<left1Num;i++)
        {
            leftString="1"+leftString;
        }

        for(qint32 i=0;i<left0Num;i++)
        {
            leftString=leftString + "0";
        }

        opString=leftString + "01" + rightString;
        AllCombination.push_back(opString);
    }
}

qint64 SampleThread::CheckCombinationValue(qint32 m, qint32 n, qint64 SampleSize)
{
    // 比较c(m,n) 与 SampleSize大小，若前者大返回0，否则返回前者的值
    QHash <QString, qint64> comValue;
    QString key="";
    qint64 chkValue=0;
    bool goon=true;

    if(m<n || n<0 || m <0)
    {
        qDebug() << "illege input";
        exit(0);
    }

// 预判断
    if(m==n || n==0)
    {
        chkValue=1;
        goon=false;
    }

    if(m == (n+1) || n==1 )
    {
        chkValue=m;
        goon=false;
    }
    if(goon==false)
    {
        if(chkValue>=SampleSize)
        {
            return 0;
        }else
        {
            return chkValue;
        }
    }

// 计算组合数

    //初始化Hash
    key=QString::number(m)+"-"+QString::number(n);
    comValue.insert(key,1);

    while(chkValue<SampleSize && comValue.size()>0)
    {
        QList <QString> allKeys=comValue.keys();
        qint64 keyNum=allKeys.size();

        for(qint64 i=0;i<keyNum;i++)
        {
            QList <QString> row;
            QString curKey=allKeys[i];
            qint64 curValue=0;

            qint64 numA,numB,numAsub,numBsub;

            if(chkValue>=SampleSize)
            {
                return 0;
                break;
            }

            // 判断是否存在
            if(comValue.contains(curKey)==false)
            {
                continue;
            }

            curValue=comValue[curKey];
            row=curKey.split("-");
            numA=row[0].toInt();
            numB=row[1].toInt();
             //C(n+1,k)=C(n,k-1)+C(n,k)

        // pair 1
            numAsub=numA-1;
            numBsub=numB-1;

            if(numBsub<=1 || numBsub>=(numAsub-1))
            {
                if(numBsub==0 || numAsub==numBsub)
                {
                    chkValue=chkValue+curValue;
                }else
                {
                    chkValue=chkValue+curValue*numAsub;
                }
            }else
            {
                QString tmpKey=QString::number(numAsub)+"-"+QString::number(numBsub);
                if(comValue.contains(tmpKey))
                {
                    comValue[tmpKey]=comValue[tmpKey]+curValue;
                }else
                {
                    comValue.insert(tmpKey,curValue);
                }
            }

            // pair 2
                numAsub=numA-1;
                numBsub=numB;

                if(numBsub<=1 || numBsub>=(numAsub-1))
                {
                    if(numBsub==0 || numAsub==numBsub)
                    {
                        chkValue=chkValue+curValue;
                    }else
                    {
                        chkValue=chkValue+curValue*numAsub;
                    }
                }else
                {
                    QString tmpKey=QString::number(numAsub)+"-"+QString::number(numBsub);
                    if(comValue.contains(tmpKey))
                    {
                        comValue[tmpKey]=comValue[tmpKey]+curValue;
                    }else
                    {
                        comValue.insert(tmpKey,curValue);
                    }
                }

                // 删除已经分析的
                comValue.remove(curKey);
        }
    }

    if(chkValue>=SampleSize)
    {
        return 0;
    }else
    {
        return chkValue;
    }

}

void SampleThread::ProduceRandomCombination(qint32 m,qint32 n, qint64 SampleSize, QList <QString> &RandomCombination)
{
    QHash <QString,qint32> combinationHash;
    qint64 SampleTimeLimit=SampleSize * 1.5;
    qint64 SampleTime=0;
    QString templateCombination="";
    QList <qint32> seedID;

    for(qint32 i=0;i<m;i++)
    {
        templateCombination=templateCombination+"0";
        seedID.push_back(i);
    }

    srand((unsigned) time(NULL)); // 伪随机种子

    while( combinationHash.size() < SampleSize && SampleTime < SampleTimeLimit )
    {
        QList <qint32> IDList=seedID;
        QString curComString=templateCombination;

        for(qint32 i=0;i<n;i++)
        {
            qint32 si=rand()%(m-i);
            curComString[IDList[si]]='1';
            IDList.removeAt(si);
        }
        combinationHash.insert(curComString,1);

        SampleTime++;
    }

      RandomCombination= combinationHash.keys();

}

qint64 SampleThread::ReadMatrixFile(QString MatrixFilePath, QList <QString> &GeneMatrixData)
{
    QFile MFHand(MatrixFilePath);
    MFHand.open(QIODevice::ReadOnly);
    qint64 allcore=0;
    QString line;
    qint32 genomeNum=0;

    while(!MFHand.atEnd())
    {
        line=MFHand.readLine();
        line.replace("\n","");
        line.replace("\r","");

        if(line=="")
        {
            continue;
        }

        if( (genomeNum!=0 && genomeNum==line.length() ) || ( genomeNum==0) )
        {
            genomeNum=line.length();
        }else
        {
            return 0;
        }

        if(line.indexOf("0")==-1)
        {
            allcore++;
        }else
        {
            GeneMatrixData.push_back(line);
        }
    }
    MFHand.close();

    return allcore;
}

void SampleThread::CalculatePanGenome(qint64 allcore, QList <qint32> &GeneMatrixDataSupA, qint32 genomeNum, QList <QString> &PanCombination, QList <QString> &GeneMatrixData, QList <qint32> &PanData, QList <qint32> &CoreData)
{
    qint64 comNum=PanCombination.size();
    qint64 clusterNum=GeneMatrixData.size();

    for(qint64 s=0;s<comNum;s++)
    {
        QString curCombination=PanCombination.at(s);
        QList <qint32> genomeID;
        qint32 genomeIDNum;
        qint64 panSize=allcore;
        qint64 coreSize=allcore;


        // 获取genomeID
        for(qint32 i=0;i<genomeNum;i++)
        {
            if(curCombination.at(i)=='1')
            {
                genomeID.push_back(i);
            }
        }
        genomeIDNum=genomeID.size();

        for(qint64 i=0;i<clusterNum;i++)
        {
            QString curCluster=GeneMatrixData.at(i);
            bool panBool=true;
            bool coreBool=true;

            if(genomeIDNum>GeneMatrixDataSupA.at(i))
            {
                coreBool=false;
            }

            for(qint32 j=0;(j<genomeIDNum && ( panBool || coreBool ) );j++)
            {
                if(curCluster.at(genomeID.at(j)) =='1')
                {
                    if(panBool)
                    {
                        panSize++;
                        panBool=false;
                    }
                }else
                {
                    coreBool=false;
                }
            }

            if(coreBool)
            {
                coreSize++;
            }
        }

        PanData.push_back(panSize);
        CoreData.push_back(coreSize);
    }
}



void SampleThread::AddCombinationRandomly(qint64 SampleSize,qint32 genomeNum, QList <QString> &RandomCombination, QList <QString> &PreComID)
{
    qint64 comNum=RandomCombination.size();
    QList < QList <qint32> > lostID; // 二维qlist
    qint64 leftQuota;
    QHash <QString, QString> comSample;

    srand( (unsigned) time(NULL));

    // 第一轮抽样，每种组合抽一次
    qDebug()<< "\tround A";
    for(qint64 i=0; i <comNum;i++)
    {
        QString curCombination=RandomCombination[i];
        QList <qint32> lostIDTmp;
        for(qint32 s=0;s<genomeNum;s++)
        {
            if(curCombination[s]=='0')
            {
                lostIDTmp.push_back(s);
            }
        }

        qint32 lostNum=lostIDTmp.size();
        QString tmpCombination=curCombination;
        qint32 addID=rand()%lostNum;
        tmpCombination[lostIDTmp[addID]]='1';

        //存入当前组合上一状态来源
        if(comSample.contains(tmpCombination))
        {
            QString tmpValue=comSample[tmpCombination];
            tmpValue=tmpValue + "," +QString::number(i);
            comSample[tmpCombination]=tmpValue;
        }else
        {
            comSample.insert(tmpCombination,QString::number(i));
        }




        //记录当前组合未抽到的组合
        lostIDTmp.removeAt(addID);
        lostID.push_back(lostIDTmp);
    }

    qDebug()<< "\tround B";
    // 第二轮抽样
    leftQuota=SampleSize- comSample.size();

    qDebug() << "comNum:" << comNum;

    while(leftQuota>0)
    {
        qDebug()<< "\tround B  leftquota:" << leftQuota;
        //for(qint64 i=0;i<comNum && leftQuota>0; i++)
        for(qint64 i=0;i < comNum && leftQuota>0; i++)
        {
            // 随机数来判断当前组合是否有资格抽样
            qint32 IsQualiValue=rand()%comNum;
            //qDebug() << "\t\trand number:" << IsQualiValue;
            if(IsQualiValue >= leftQuota)
            {
                continue;
            }

            QList <qint32> lostIDTmp=lostID[i];
            qint32 lostNum=lostIDTmp.size();

            if(lostNum==0)
            {
                leftQuota=0;
                break;
            }

            QString tmpCombination=RandomCombination[i];
            qint32 addID=rand()%lostNum;
            tmpCombination[lostIDTmp[addID]]='1';

            //存入当前组合上一状态来源
            if(comSample.contains(tmpCombination))
            {
                QString tmpValue=comSample[tmpCombination];
                tmpValue=tmpValue + "," +QString::number(i);
                comSample[tmpCombination]=tmpValue;
            }else
            {
                comSample.insert(tmpCombination,QString::number(i));
            }

            // 记录当前组合中未被抽到的基因组
            lostIDTmp.removeAt(addID);
            lostID[i]=lostIDTmp;

            leftQuota=SampleSize- comSample.size();
        }
    }

    // 处理抽样数据
    RandomCombination.clear();
    QHash <QString, QString>::iterator itStart,itEnd,it;
    itStart=comSample.begin();
    itEnd=comSample.end();
    for(it=itStart;it!=itEnd;it++)
    {
        RandomCombination.push_back(it.key());
        PreComID.push_back(it.value());
    }
}


void SampleThread::GenomeDiffByCluster(qint32 genomeNum, QList <QString> &GeneMatrixData,QVector <QVector <qint64> > &GenomeDiff)
{
    qint64 clusterNum=GeneMatrixData.size();

    //初始化vector
    for(qint32 i=0; i<genomeNum; i++)
    {
        QVector <qint64> tmpVector;
        for(qint32 j=0; j<genomeNum; j++)
        {
            tmpVector.append(0);
        }
        GenomeDiff.append(tmpVector);
    }

    for(qint32 s=0;s<genomeNum;s++)
    {
        for(qint32 k=s+1;k<genomeNum;k++)
        {
            qint64 diff=0;
            // 计算差异
            for(qint64 i=0; i<clusterNum; i++)
            {
                QString matrix=GeneMatrixData[i];
                if(matrix.at(s) != matrix.at(k))
                {
                    diff++;
                }
            }

            // 存储差值
            GenomeDiff[s][k]=diff;
            GenomeDiff[k][s]=diff;
        }
    }
}


void SampleThread::FilterCombinationByDispersion(QList <QString> &AllCombination,QVector <QVector <qint64> > &GenomeDiff,qint64 SampleSize)
{
    // 该方法中将Dispersion的值放大了1000倍以提高精确度，以便用于基数排序

    QList <QString> OrderedtmpAllCombination;
    QList <qint32> Dispersion;
    QList <qint32> OrderID;
    qint64 comNum=AllCombination.size();
    qint32 genomeNum=AllCombination.at(0).length();
    qlonglong sum_dis=0;

    // 计算每种组合的离散度
    for(qint64 s=0;s<comNum;s++)
    {
        QList <qint32> genomeID;
        qint32 curNum;
        QString curCombination=AllCombination.at(s);

        // 获取genome id
        for(qint32 i=0;i<genomeNum;i++)
        {
            if(curCombination.at(i)=='1')
            {
                genomeID.push_back(i);
            }
        }
        curNum=genomeID.size();

        //计算差异总和
        qint64 diffSum=0;
        for(qint32 i=0;i<curNum;i++)
        {
            for(qint32 j=i+1;j<curNum;j++)
            {
                diffSum=diffSum+GenomeDiff.at(i).at(j);
            }
        }
        qint32 tmpDiff= diffSum*1000/ ( curNum*(curNum-1)/2 );
        Dispersion.push_back(tmpDiff);
        sum_dis=sum_dis+tmpDiff;
    }

    // 数组排序以及组合排序
    RadixSortReturnOrder(Dispersion,OrderID);

    for(qint64 i=0; i<comNum;i++)
    {
        OrderedtmpAllCombination.push_back(AllCombination.at(OrderID.at(i)));
    }

/*
 * 取样策略一:在平均数前后分别取0.5*SampleSize个数
 * 存在问题： 抽样部分整体与平均数相比偏高或者偏低
 */
/*
    qint64 meanPos=0;
    qint64 SampleSizeHalf=SampleSize/2;

    if(avg_dis>Dispersion[ floor(comNum/2) ]) // 从中位数开始搜索位置开始比较
    {
        for(qint64 i=floor(comNum/2);i<comNum-1;i++)
        {
            if(avg_dis >= Dispersion[i] && avg_dis<=Dispersion[i+1])
            {
                meanPos=i;
                break;
            }
        }
    }else
    {
        for(qint64 i=floor(comNum/2);i>1;i--)
        {
            if(avg_dis <= Dispersion[i] && avg_dis>=Dispersion[i-1])
            {
                meanPos=i;
                break;
            }
        }
    }
    tmpAllCombination=OrderedtmpAllCombination.mid(meanPos-SampleSizeHalf,SampleSize);
*/
/*
 * 取样策略二:在所有数中从小到大等步长均匀地取SampleSize个数
 */
    qint32 SampleStep=comNum/SampleSize;
    AllCombination.clear();
    for(qint32 i=0; i<comNum; i=i+SampleStep)
    {
        AllCombination.append(OrderedtmpAllCombination.at(i));
    }
}

/*

QList <QString> SampleThread::FilterCombinationByDispersion(QList <QString> AllCombination,QHash <QString,qint64> GenomeDiff,qint64 SampleSize)
{
    QList <QString> tmpAllCombination=AllCombination;
    QList <double> Dispersion;
    qint64 comNum=tmpAllCombination.size();
    qint32 genomeNum=tmpAllCombination[0].length();
    double sum_dis=0.0;
    double avg_dis;


    // 计算每种组合的离散度
    for(qint64 s=0;s<comNum;s++)
    {
        QList <qint32> genomeID;
        qint32 curNum;
        QString curCombination=tmpAllCombination[s];

        // 获取genome id
        for(qint32 i=0;i<genomeNum;i++)
        {
            if(curCombination[i]=='1')
            {
                genomeID.push_back(i);
            }
        }
        curNum=genomeID.size();

        //计算差异总和
        qint64 diffSum=0;
        for(qint32 i=0;i<curNum;i++)
        {
            for(qint32 j=i+1;j<curNum;j++)
            {
                QString key=QString::number(genomeID[i]) + "-" + QString::number(genomeID[j]);
                diffSum=diffSum+GenomeDiff[key];
            }
        }
        double tmpDiff= floor( diffSum*1000/ (  curNum*(curNum-1)/2  ) )/ 1000.0;
        Dispersion.push_back(tmpDiff);

        sum_dis=sum_dis+tmpDiff;
    }

    // 判断上下界限值
    avg_dis=sum_dis/(1.0*comNum);

    // 数组排序
    for(qint64 i=0; i<comNum;i++)
    {
        for(qint64 j=i+1;j<comNum;j++)
        {
            if(Dispersion[i]>Dispersion[j])
            {
                Dispersion.swap(i,j);
                tmpAllCombination.swap(i,j);
            }
        }
    }

    qint64 meanPos=0;
    qint64 SampleSizeHalf=SampleSize/2;
    if(avg_dis>Dispersion[ floor(comNum/2) ]) // 从中位数开始搜索位置开始比较
    {
        for(qint64 i=floor(comNum/2);i<comNum-1;i++)
        {
            if(avg_dis >= Dispersion[i] && avg_dis<=Dispersion[i+1])
            {
                meanPos=i;
                break;
            }
        }
    }else
    {
        for(qint64 i=floor(comNum/2);i>1;i--)
        {
            if(avg_dis <= Dispersion[i] && avg_dis>=Dispersion[i-1])
            {
                meanPos=i;
                break;
            }
        }
    }

    tmpAllCombination=tmpAllCombination.mid(meanPos-SampleSizeHalf,SampleSize);
    return tmpAllCombination;
}

*/



qint64 SampleThread::ReadClusterFile(QString ClusterFilePath, QList <QString> &GeneMatrixData, bool IsHeader, bool IsClusterID)
{
    QFile CFHand(ClusterFilePath);
    CFHand.open(QIODevice::ReadOnly);

    QString line;
    qint64 allcore=0;
    qint32 genomeNum=0;

    if(IsHeader) // 如果包含文件头则去掉这一行
    {
         line=CFHand.readLine();
    }

    while(!CFHand.atEnd())
    {
        line = CFHand.readLine();
        line.replace("\n","");
        line.replace("\r","");
        QList <QString> row;
        if(line=="")
        {
            continue;
        }

        row=line.split("\t");

        if(IsClusterID) //删除第一列
        {
            row.removeAt(0);
        }

        QString matrix="";

        // 文件格式检查，判断每一行的列数都一样
        if(( row.size()==genomeNum && genomeNum!=0 ) || (genomeNum==0 && row.size()!=genomeNum) )
        {
            genomeNum=row.size();
        }else
        {
            qDebug() << "file line is not equal";
            return 0;
        }

        //转换数据
        for(qint32 i=0; i<genomeNum;i++)
        {
            if(row.at(i) == "-")
            {
                matrix=matrix+"0";
            }else
            {
                matrix=matrix+"1";
            }
        }

        //判断是否为core
        if(matrix.indexOf("0")==-1)
        {
            allcore++;
        }else
        {
            GeneMatrixData.push_back(matrix);
        }
    }

    CFHand.close();

    qDebug() << "allcore:" << allcore << "  file num:" << GeneMatrixData.size();

    return allcore;
}

void SampleThread::SamplingRandomlly(QList <QString> &GeneMatrixData,qint64 allcore, QList <qint32> &GeneMatrixDataSupA, qint64 SampleSize,qint32 SampleRepeat, QList<qint32> &GenomeNumValue, QList<qint32> &PanValue, QList<qint32> &CoreValue, QList<double> &PanAvgValue, QList<double> &CoreAvgValue)
{
    qint32 genomeNum=GeneMatrixData.at(0).length();
    QList <bool> toSample;
    qint32 halfData= (genomeNum+1)/2;

    qDebug() << "check which to sample";
//先计算所有的i值，哪些需要抽样
    for(qint32 i=0;i<=genomeNum;i++)
    {
        toSample.push_back(false);
    }
    for(qint32 i=halfData;i>=1;i--)
    {
        //qint64 chkValue;
        qint64 chkValue2;
        //判断当前组合是否需要抽样

        //chkValue=CheckCombinationValue(genomeNum,i,SampleSize);
        chkValue2=CheckCombinationValue(genomeNum,i,SampleSize * SampleRepeat);


        if(chkValue2==0) //
        {
            toSample[i]=true;
            toSample[genomeNum-i]=true;
        }else
        {
            break;
        }
    }

    qDebug() << "calculate combination";

    for(qint32 i=1;i<=genomeNum;i++)
    {
        QString msg="Current genome number: " + QString::number(i) + "/" + QString::number(genomeNum);
        emit sigSampleThreadProcessMsg(msg);

        qDebug() << "Now: " <<  i << "\t" << toSample[i];

        QList<QString> PanCombination;
        QList<qint32> PanData;
        QList<qint32> CoreData;
        qint64 PanDataSum=0;
        qint64 CoreDataSum=0;
        double PanDataAvg;
        double CoreDataAvg;
        qint64 comNum;

        //抽样
        if(toSample[i])
        {
            /*
             *
             *ProduceRandomCombination(genomeNum,i,SampleSize,PanCombination);
             *  从下面开始引入sample repeat
             */

            for(qint32 k=1; k<=SampleRepeat;k++)
            {
                QString msg="\tSample repeat: " + QString::number(k) + "/" + QString::number(SampleRepeat);
                emit sigSampleThreadProcessMsg(msg);
                QList<QString> PanCombinationTemp;

                ProduceRandomCombination(genomeNum,i,SampleSize,PanCombinationTemp);
                PanCombination.append(PanCombinationTemp);
            }

        }else
        {
            GenerateAllCombination(genomeNum,i,PanCombination);
        }

        qDebug() << "combination number:" << PanCombination.size();
        //计算泛基因组大小
        CalculatePanGenome(allcore,GeneMatrixDataSupA,genomeNum,PanCombination,GeneMatrixData,PanData,CoreData);

        //计算平均数并储存数据
        comNum=PanCombination.size();
        for(qint64 s=0;s<comNum;s++)
        {
            //储存基因组数目
            GenomeNumValue.push_back(i);

            //求和
            PanDataSum=PanDataSum+PanData[s];
            CoreDataSum=CoreDataSum+CoreData[s];
        }
        PanValue.append(PanData);
        CoreValue.append(CoreData);

        //算平均值并储存
        PanDataAvg= floor(PanDataSum*1000/comNum)*1.0/1000.0;
        CoreDataAvg=floor(CoreDataSum*1000/comNum)*1.0/1000.0;
        PanAvgValue.push_back(PanDataAvg);
        CoreAvgValue.push_back(CoreDataAvg);
    }
}



void SampleThread::AddRandomly(QList <QString> &GeneMatrixData, qint64 allcore, QList <qint32> &GeneMatrixDataSupA, qint64 SampleSize, qint32 SampleRepeat, QList<qint32> &GenomeNumValue, QList<qint32> &PanValue, QList<qint32> &CoreValue, QList<double> &PanAvgValue, QList<double> &CoreAvgValue, QList<qint32> &NewValue,QList<qint32> &Newgi, QList<double> &NewAvgValue)
{
    qint32 genomeNum=GeneMatrixData[0].length();
    QList <qint32> PrePanData;

    QList<QString> PanCombination;
    QList<qint32> PanData;
    QList<qint32> CoreData;
    qint64 PanDataSum=0;
    qint64 CoreDataSum=0;
    qint64 comNum;
    qint64 newComNum; // 记录新基因的组合数
    double PanDataAvg;
    double CoreDataAvg;

    emit sigSampleThreadProcessMsg("Current genome number: 1/" + QString::number(genomeNum));
  // 初始情况
    // 获取i=1的时候，所有组合
    GenerateAllCombination(genomeNum,1,PanCombination);
    //计算泛基因组大小
    CalculatePanGenome(allcore,GeneMatrixDataSupA,genomeNum,PanCombination,GeneMatrixData,PanData,CoreData);
    //计算平均数
    comNum=PanData.size();
    for(qint64 s=0;s <comNum;s++)
    {
        //储存数据
        GenomeNumValue.push_back(1);
        PanValue.push_back(PanData[s]);
        CoreValue.push_back(CoreData[s]);

        //计算平均数
        PanDataSum=PanDataSum+PanData[s];
        CoreDataSum=CoreDataSum+CoreData[s];
    }

    //计算平均值
    PanDataAvg= floor(PanDataSum*1000/comNum)*1.0/1000.0;
    CoreDataAvg=floor(CoreDataSum*1000/comNum)*1.0/1000.0;
    PanAvgValue.push_back(PanDataAvg);
    CoreAvgValue.push_back(CoreDataAvg);

    //初始化前一状态的pandata
    PrePanData = PanData;

 //计算每个组合下的泛基因组
    for(qint32 i=2;i<=genomeNum;i++)
    {
        qDebug() << "now: " << i;
        emit sigSampleThreadProcessMsg("Current genome number: "+ QString::number(i) +"/" + QString::number(genomeNum));
        QList<QString> PrePanDataID;
        qint64 NewGeneSum=0;
        double NewGeneAvg;

        //初始化一些数值
        PanData.clear();
        CoreData.clear();
        PanDataSum=0;
        CoreDataSum=0;

        qDebug() << "adding genome";
        //获取所有组合
        AddCombinationRandomly(SampleSize,genomeNum,PanCombination,PrePanDataID);

        qDebug() << "calculating pansize";
        //计算泛基因组
        CalculatePanGenome(allcore,GeneMatrixDataSupA,genomeNum,PanCombination,GeneMatrixData,PanData,CoreData);

        qDebug() << "calculating average";
        //计算平均数
        comNum=PanData.size();
        newComNum=0;
        for(qint64 s=0;s<comNum;s++)
        {
            //储存新基因数据
            QList<QString> AllPreID=PrePanDataID[s].split(","); //获取所有的ID
            qint64 AllPreIDNum=AllPreID.size();
            for(qint64 k=0;k<AllPreIDNum;k++)
            {
                qint32 curV= AllPreID[k].toInt();

                Newgi.push_back(i);
                NewValue.push_back(PanData[s] - PrePanData[curV]);

                //用于新基因的平均值计算
                NewGeneSum=NewGeneSum + PanData[s] - PrePanData[curV];
                newComNum++; //新基因组合数
            }
            //储存数据
            GenomeNumValue.push_back(i);

            //计算平均值
            PanDataSum=PanDataSum+PanData[s];
            CoreDataSum=CoreDataSum+CoreData[s];

        }
        PanValue.append(PanData);//储存pandata数据
        CoreValue.append(CoreData);//储存coredata数据

        //计算平均值
        PanDataAvg= floor(PanDataSum*1000/comNum)*1.0/1000.0;
        CoreDataAvg=floor(CoreDataSum*1000/comNum)*1.0/1000.0;
        NewGeneAvg=floor(NewGeneSum*1000/newComNum)*1.0/1000.0;
        PanAvgValue.push_back(PanDataAvg);
        CoreAvgValue.push_back(CoreDataAvg);
        NewAvgValue.push_back(NewGeneAvg);

        //为i+1记录当前的泛基因组
        PrePanData=PanData;
    }
}

void SampleThread::DistanceGuide(QList <QString> &GeneMatrixData, qint64 allcore, QList <qint32> &GeneMatrixDataSupA, qint64 SampleSize, qint32 SampleRepeat, QList<qint32> &GenomeNumValue, QList<qint32> &PanValue, QList<qint32> &CoreValue, QList<double> &PanAvgValue, QList<double> &CoreAvgValue)
{
    qint32 amplification=AmpCoefficient; //放大系数
    qint32 genomeNum=GeneMatrixData.at(0).length();
    QVector < QVector <qint64> > GenomeDistance;
    qint64 comNum = GeneMatrixData.size();

    QList <bool> toSample;
    QList <bool> toFilter;
    QList <bool> doRepeat;

    qDebug() << "calculate distance";
    //计算两两基因组间的距离
    GenomeDiffByCluster(genomeNum,GeneMatrixData,GenomeDistance);


    //预计算是否抽样以及是否过滤
    for(qint32 i=0;i<=genomeNum;i++)
    {
        toSample.push_back(false);
        toFilter.push_back(false);
        doRepeat.push_back(false);
    }
    /* 引入重复模拟之前
    for(qint32 i=halfData;i>=1;i--)
    {
        qint64 chkValue;
        //判断是否抽样
        chkValue=CheckCombinationValue(genomeNum,i,SampleSize*amplification);
        if(chkValue==0)
        {
            toSample[i]=true;
            toSample[genomeNum-i]=true;
        }

        //判断是否过滤
        chkValue=CheckCombinationValue(genomeNum,i,SampleSize);
        if(chkValue==0)
        {
            toFilter[i]=true;
            toFilter[genomeNum-i]=true;
        }else
        {
            break;
        }
    }*/

    /*
     * 引入重复的模型
     */
    for(qint32 i=1; i<=genomeNum;i++)
    {
        qint64 chkValueA; // 比较sample size
        qint64 chkValueB; // 比较sample size * repeat
        qint64 chkValueC; // 比较sample size * amplification

        chkValueA=CheckCombinationValue(genomeNum,i,SampleSize);
        if(chkValueA!=0)
        {
            toSample[i]=false;
            toFilter[i]=false;
            continue;
        }

        chkValueB=CheckCombinationValue(genomeNum,i,SampleSize*SampleRepeat);

        if(chkValueB!=0) // 综合数目小于重复后的数目
        {
            toSample[i]=false;
            toFilter[i]=false;
            continue;
        }

        chkValueC=CheckCombinationValue(genomeNum,i,SampleSize*amplification);

        /*
         * 注意由于amplification < SampleRepeat,所以当程序能运行到这里的时候
         * 说明总的组合数目 All Combination > SampleSize * SampleRepeat > SampleSize * amplification
         */
        doRepeat[i]=true;

        if(chkValueC!=0) // 比较放大后的数目
        {
            toSample[i]=false;
            toFilter[i]=false;
        }else
        {
            toSample[i]=true;
            toFilter[i]=true;
        }

    }


    for(qint32 i=1;i<=genomeNum;i++)
    {
         qDebug() << i << "\t" << toSample[i] << "\t" << toFilter[i];
    }

    for(qint32 i=1; i<=genomeNum;i++)
    {
        QList<QString> PanCombination;
        QList<qint32> PanData;
        QList<qint32> CoreData;
        qint64 PanDataSum=0;
        qint64 CoreDataSum=0;
        double PanDataAvg;
        double CoreDataAvg;
        qint32 repeatTime;

        qDebug() << "Now: " <<  i << "\t" << toSample[i] << "\t" << toFilter[i];
        QString msg="Current genome number: " + QString::number(i) + "/" + QString::number(genomeNum);
        emit sigSampleThreadProcessMsg(msg);

        if(doRepeat.at(i))
        {
            repeatTime=SampleRepeat;
        }else
        {
            repeatTime=1;
        }

        for(qint32 k=1; k<= repeatTime; k++)
        {
            QString msg="\tSample repeat: " + QString::number(k) + "/" + QString::number(repeatTime);
            emit sigSampleThreadProcessMsg(msg);
            QList<QString> PanCombinationTemp;

            if(toSample.at(i)==true)
            {
                 ProduceRandomCombination(genomeNum,i,SampleSize*amplification,PanCombinationTemp);
            }else
            {
                GenerateAllCombination(genomeNum,i,PanCombinationTemp);
            }

            if(toFilter.at(i)==true)
            {
                FilterCombinationByDispersion(PanCombinationTemp,GenomeDistance,SampleSize);
            }

            PanCombination.append(PanCombinationTemp);
        }


        qDebug() << "combination number:" << PanCombination.size();
        //计算泛基因组大小
        CalculatePanGenome(allcore,GeneMatrixDataSupA,genomeNum,PanCombination,GeneMatrixData,PanData,CoreData);

        qDebug() << "calculating average";
        // 计算平均值的同时将数值放入大数组中，传递出去
        comNum=PanData.size();
        for(qint64 s=0;s<comNum;s++)
        {
            //储存数据
            GenomeNumValue.push_back(i);//i

            //求和
            PanDataSum=PanDataSum+PanData.at(s);
            CoreDataSum=CoreDataSum+CoreData.at(s);
        }        
        PanValue.append(PanData); //pan
        CoreValue.append(CoreData); //core

        PanDataAvg= floor(PanDataSum*1000/comNum)*1.0/1000.0;
        CoreDataAvg=floor(CoreDataSum*1000/comNum)*1.0/1000.0;
        PanAvgValue.push_back(PanDataAvg);
        CoreAvgValue.push_back(CoreDataAvg); 
    }
}

void SampleThread::TraverseAll(QList<QString> &GeneMatrixData, qint64 allcore, QList <qint32> &GeneMatrixDataSupA, QList<qint32> &GenomeNumValue, QList<qint32> &PanValue, QList<qint32> &CoreValue, QList<double> &PanAvgValue, QList<double> &CoreAvgValue)
{
    qint32 genomeNum = GeneMatrixData.at(0).length();
    for(qint32 i=1;i<=genomeNum;i++)
    {
        QString msg="Current genome number: " + QString::number(i) + "/" + QString::number(genomeNum);
        emit sigSampleThreadProcessMsg(msg);

        QList<QString> PanCombination;
        QList<qint32> PanData;
        QList<qint32> CoreData;
        qint64 PanDataSum=0;
        qint64 CoreDataSum=0;
        double PanDataAvg;
        double CoreDataAvg;
        qint64 comNum;

        //遍历组合
        GenerateAllCombination(genomeNum,i,PanCombination);

        //计算泛基因组大小
        CalculatePanGenome(allcore,GeneMatrixDataSupA,genomeNum,PanCombination,GeneMatrixData,PanData,CoreData);

        //计算平均数并储存数据
        comNum=PanCombination.size();
        for(qint64 s=0;s<comNum;s++)
        {
            //储存基因组数目
            GenomeNumValue.push_back(i);

            //求和
            PanDataSum=PanDataSum+PanData.at(s);
            CoreDataSum=CoreDataSum+CoreData.at(s);
        }
        PanValue.append(PanData);
        CoreValue.append(CoreData);

        //算平均值并储存
        PanDataAvg= floor(PanDataSum*1000/comNum)*1.0/1000.0;
        CoreDataAvg=floor(CoreDataSum*1000/comNum)*1.0/1000.0;
        PanAvgValue.push_back(PanDataAvg);
        CoreAvgValue.push_back(CoreDataAvg);
    }

}
/*
// 计算 boxplot data
void SampleThread::CalculateBoxplotData(QList<qint32> RawData, QList<qint32> GenomeNumData, QList<double> &Q1, QList<double> &Q2, QList<double> &Q3, QList<double> &IQR)
{
    qint32 Min,Max; // 数值中最大最小数
    qint32 DNum=GenomeNumData.size();//数量
    QHash<qint32,QString> RawHash; // 对数据进行分类
    qint32 CurNum;
    QString CurKey;

    // 计算最大最小数
    Min=GenomeNumData[0];
    Max=GenomeNumData[0];

    for(qint32 i=0; i<DNum;i++)
    {
        CurNum=GenomeNumData[i];

        if(CurNum<Min)
        {
            Min=CurNum;
        }

        if(CurNum>Max)
        {
            Max=CurNum;
        }

        if(RawHash.contains(CurNum))
        {
            RawHash[CurNum]=RawHash[CurNum] + "," + QString::number(RawData[i]);
        }else
        {
            RawHash.insert(CurNum,QString::number(RawData[i]));
        }
    }

    for(qint32 s=Min;s<=Max;s++)
    {
        QList <QString> splitTmp=RawHash[s].split(",");
        QList <double> data;
        qint32 dataNum=splitTmp.size();

        // 转换数据类型
        for(qint32 i=0; i<dataNum;i++)
        {
            data.push_back(splitTmp[i].toDouble());
        }

        // 排序
        for(qint32 i=0; i<dataNum;i++)
        {
            for(qint32 j=i;j<dataNum;j++)
            {
                if(data[i]>data[j])
                {
                    data.swap(i,j);
                }
            }
        }

        // 取中位数值
        double q1value=data[floor(dataNum*0.25)];
        double q2value;
        double q3value=data[floor(dataNum*0.75)];
        double iqrvalue=q3value-q1value;

        if(floorl(dataNum/2) *2 ==dataNum)
        {
            q2value=(data[floorl(dataNum/2)] + data[1+floorl(dataNum/2)])*1.0/2;
        }else
        {
            if(dataNum>1)
            {
                q2value=data[1+floorl(dataNum/2)];
            }else
            {
                q2value=data[0];
            }

        }

        // 封装数据
        Q1.push_back(q1value);
        Q2.push_back(q2value);
        Q3.push_back(q3value);
        IQR.push_back(iqrvalue);

        qDebug() << "Now:" << s << "\tQ1:" << q1value << "\tQ2:" << q2value << "\tQ3:" << q3value << "\tIQR:" << iqrvalue;
    }
}

*/

// 计算 boxplot data
void SampleThread::CalculateBoxplotData(QList<qint32> RawData, QList<qint32> GenomeNumData, QList<double> &Q1, QList<double> &Q2, QList<double> &Q3, QList<double> &IQR)
{
    qint32 Min,Max; // 数值中最大最小数
    qint32 DNum=GenomeNumData.size();//数量
    QHash <qint32,QList <qint32> > RawHash; // 对数据进行分类
    qint32 CurNum;


    // 计算最大最小数
    Min=GenomeNumData.at(0);
    Max=GenomeNumData.at(0);

    // 整理数据
    emit sigSampleThreadProcessMsg("   sorting sampling result ...");

    for(qint32 i=0; i<DNum;i++)
    {
        CurNum=GenomeNumData.at(i);

        if(CurNum<Min)
        {
            Min=CurNum;
        }

        if(CurNum>Max)
        {
            Max=CurNum;
        }

        RawHash[CurNum].push_back(RawData.at(i));
    }

    emit sigSampleThreadProcessMsg("   there are " + QString::number(Max) + " cycles for current task.");
    for(qint32 s=Min;s<=Max;s++)
    {
        emit sigSampleThreadProcessMsg("   \tgenome number N=" + QString::number(s));
        QList <qint32> data = RawHash[s];
        qint32 dataNum = data.size();

        // 排序
        if(dataNum>=500)
        {
            RadixSort(data);
        }else
        {
            for(qint32 i=0; i<dataNum;i++)
            {
                for(qint32 j=i;j<dataNum;j++)
                {
                    if(data.at(i)>data.at(j))
                    {
                        data.swap(i,j);
                    }
                }
            }
        }

        // 取中位数值
        double q1value=data.at(floor(dataNum*0.25));
        double q2value;
        double q3value=data.at(floor(dataNum*0.75));
        double iqrvalue=q3value-q1value;

        if(floorl(dataNum/2) *2 ==dataNum)
        {
            q2value=(data.at(floorl(dataNum/2)) + data.at(1+floorl(dataNum/2)))*1.0/2;
        }else
        {
            if(dataNum>1)
            {
                q2value=data.at(1+floorl(dataNum/2));
            }else
            {
                q2value=data.at(0);
            }

        }

        // 封装数据
        Q1.push_back(q1value);
        Q2.push_back(q2value);
        Q3.push_back(q3value);
        IQR.push_back(iqrvalue);

        qDebug() << "Now:" << s << "\tQ1:" << q1value << "\tQ2:" << q2value << "\tQ3:" << q3value << "\tIQR:" << iqrvalue;
    }
}





void SampleThread::CalculateBoxplotYMaxMinValue()
{
    qreal pcMax=-1e8;
    qreal pcMin=1e8;

    qint32 dataSize;
    QList<qreal> pcvalue;
    qint32 pcnum;


// pan core 部分
    dataSize=PanProfile.CoreValueIQR.size();
    for(qint32 i=0; i<dataSize;i++)
    {
        qreal pmax,pmin,cmax,cmin;

        pmax=PanProfile.PanValueQ3.at(i)+1.5*PanProfile.PanValueIQR.at(i);
        pmin=PanProfile.PanValueQ1.at(i)-1.5*PanProfile.PanValueIQR.at(i);

        cmax=PanProfile.CoreValueQ3.at(i)+1.5*PanProfile.CoreValueIQR.at(i);
        cmin=PanProfile.CoreValueQ1.at(i)-1.5*PanProfile.CoreValueIQR.at(i);

        pcvalue.push_back(pmax);
        pcvalue.push_back(pmin);
        pcvalue.push_back(cmax);
        pcvalue.push_back(cmin);
    }

    pcnum=pcvalue.size();
    for(qint32 i=0; i< pcnum; i++)
    {
        if(pcMax<pcvalue.at(i))
        {
            pcMax=pcvalue.at(i);
        }

        if(pcMin>pcvalue.at(i))
        {
            pcMin=pcvalue.at(i);
        }
    }

    PanProfile.pancoreBoxplotYMax=pcMax;
    PanProfile.pancoreBoxplotYMin=pcMin;
}


// 根据泛基因组数据计算新基因的增长

void SampleThread::CalculateNewGeneData()
{
    qint32 genomeNum=PanProfile.GenomeNumber;
    for(qint32 i=2;i<=genomeNum;i++)
    {
        PanProfile.NewAvgValue.push_back(PanProfile.PanAvgValue[i-1]-PanProfile.PanAvgValue[i-2]);
        PanProfile.Newgi.push_back(i);
    }
}

// 警告信息
void SampleThread::WarningMessage(QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.setWindowTitle("Tips");
    msgBox.exec();
}


// 格式化时间输出
QString SampleThread::FormatTime2Qstring(qlonglong timeCost)
{
    QString output="";
    QString unit;

    if(timeCost>=604800000) //week
    {
        qint32 num = timeCost/604800000;
        if(num >1)
        {
            unit = "weeks";
        }else
        {
            unit= "week";
        }
        output=QString::number(num) + unit;
        timeCost = timeCost % 604800000;
    }

    if(timeCost>=86400000) //day
    {
        qint32 num = timeCost/604800000;
        if(num >1)
        {
            unit = "days";
        }else
        {
            unit= "day";
        }

        if(output !="")
        {
            output=output + ", " +QString::number(num) + unit;
        }else
        {
            output=QString::number(num) + unit;
        }
        timeCost = timeCost % 604800000;
    }

    if(timeCost>=3600000) //hour
    {
        qint32 num = timeCost/3600000;
        if(num >1)
        {
            unit = "hours";
        }else
        {
            unit= "hour";
        }

        if(output !="")
        {
            output=output + ", " +QString::number(num) + unit;
        }else
        {
            output=QString::number(num) + unit;
        }
        timeCost = timeCost % 3600000;
    }

    if(timeCost>=60000) //minute
    {
        qint32 num = timeCost/60000;
        if(num >1)
        {
            unit = "minutes";
        }else
        {
            unit= "minute";
        }

        if(output !="")
        {
            output=output + ", " +QString::number(num) + unit;
        }else
        {
            output=QString::number(num) + unit;
        }
        timeCost = timeCost % 60000;
    }

    if(timeCost>=1000) //second
    {
        qint32 num = timeCost/1000;
        if(num >1)
        {
            unit = "seconds";
        }else
        {
            unit= "second";
        }

        if(output !="")
        {
            output=output + ", " +QString::number(num) + unit;
        }else
        {
            output=QString::number(num) + unit;
        }
        timeCost = timeCost % 1000;
    }
    return output;
}


//基数排序

void SampleThread::RadixSort(QList<qint32> &DataSet)
{
    qint32 MaxDigitNum; // 数值精度
    qint32 MaxNumber = 0; //最大数
    qint64 DataSize=DataSet.size();


    QList <QList<qint32> > bucket; // 桶
    qint32 DigitNumAssistant;

// 先判断最大数
    for(qint64 i=0; i< DataSize; i++)
    {
        // 寻找最大的数
        if(MaxNumber < DataSet.at(i))
        {
            MaxNumber = DataSet.at(i);
        }
     }

//计算总的位数,即最大数的长度
    QString tmpString = QString::number(MaxNumber);
    MaxDigitNum = tmpString.length();


// 排序

    for(qint32 digit=1; digit<=MaxDigitNum; digit++)
    {
        //创建桶
        for(qint32 i=0; i<=9;i++)
        {
            QList<qint32> tmpBucket;
            bucket.push_back(tmpBucket);
        }

        DigitNumAssistant = qPow(10,(digit-1));

        for(qint64 i=0; i< DataSize; i++)
        {
            // 寻找最大的数
            if(MaxNumber < DataSet.at(i))
            {
                MaxNumber = DataSet.at(i);
            }

            qint32 probe;
            probe = DataSet.at(i)/DigitNumAssistant;
            probe = probe % 10;
            switch (probe)
            {
                case 0:
                    bucket[0].push_back(DataSet.at(i));
                    break;
                case 1:
                    bucket[1].push_back(DataSet.at(i));
                    break;
                case 2:
                    bucket[2].push_back(DataSet.at(i));
                    break;
                case 3:
                    bucket[3].push_back(DataSet.at(i));
                    break;
                case 4:
                    bucket[4].push_back(DataSet.at(i));
                    break;
                case 5:
                    bucket[5].push_back(DataSet.at(i));
                    break;
                case 6:
                    bucket[6].push_back(DataSet.at(i));
                    break;
                case 7:
                    bucket[7].push_back(DataSet.at(i));
                    break;
                case 8:
                    bucket[8].push_back(DataSet.at(i));
                    break;
                case 9:
                    bucket[9].push_back(DataSet.at(i));
                    break;

                default:
                    break;
            }
        }

        //合并数据
        DataSet.clear();
        for(qint32 i=0; i<=9;i++)
        {
            if(bucket.at(i).size() ==0)
            {
                continue;
            }
            DataSet.append(bucket.at(i));
        }
        bucket.clear();
    }
}


// 计算每个cluster里面1的个数
void SampleThread::AnalyzeMatrixData(QList <QString> &GeneMatrixData, QList <qint32> &GeneMatrixDataSupA)
{
    qint64 clusterNum=GeneMatrixData.size();
    for(qint64 i=0; i<clusterNum;i++)
    {
        QString curCluster = GeneMatrixData.at(i);
        curCluster.replace("0","");
        GeneMatrixDataSupA.push_back(curCluster.length());
    }
}


void SampleThread::RadixSortReturnOrder(QList<qint32> &DataSet, QList<qint32> &Order)
{
    qint32 MaxDigitNum; // 数值精度
    qint32 MaxNumber = 0; //最大数
    qint64 DataSize=DataSet.size();

    QList<qint32> OrderID; // 排序后的顺序


    QList <QList<qint32> > bucket; // 桶
    QList <QList<qint32> > bucketOrder; // 桶

    qint32 DigitNumAssistant;

// 先判断最大数
    for(qint64 i=0; i< DataSize; i++)
    {
        // 寻找最大的数
        if(MaxNumber < DataSet.at(i))
        {
            MaxNumber = DataSet.at(i);
        }

        // 构建记录顺序的数组
        OrderID.push_back(i);
     }

//计算总的位数,即最大数的长度
    QString tmpString = QString::number(MaxNumber);
    MaxDigitNum = tmpString.length();

// 排序
    //qDebug() << "sorting";
    for(qint32 digit=1; digit<=MaxDigitNum; digit++)
    {
        //创建桶
        for(qint32 i=0; i<=9;i++)
        {
            QList<qint32> tmpBucket;
            bucket.push_back(tmpBucket);

            QList<qint32> tmpBucketB;  // order
            bucketOrder.push_back(tmpBucketB);
        }

        DigitNumAssistant = qPow(10,(digit-1));

        for(qint64 i=0; i< DataSize; i++)
        {
            // 寻找最大的数
            if(MaxNumber < DataSet.at(i))
            {
                MaxNumber = DataSet.at(i);
            }

            qint32 probe;
            probe = DataSet.at(i)/DigitNumAssistant;
            probe = probe % 10;
            switch (probe)
            {
                case 0:
                    bucket[0].push_back(DataSet.at(i));
                    bucketOrder[0].push_back(OrderID.at(i));
                    break;
                case 1:
                    bucket[1].push_back(DataSet.at(i));
                    bucketOrder[1].push_back(OrderID.at(i));
                    break;
                case 2:
                    bucket[2].push_back(DataSet.at(i));
                    bucketOrder[2].push_back(OrderID.at(i));
                    break;
                case 3:
                    bucket[3].push_back(DataSet.at(i));
                    bucketOrder[3].push_back(OrderID.at(i));
                    break;
                case 4:
                    bucket[4].push_back(DataSet.at(i));
                    bucketOrder[4].push_back(OrderID.at(i));
                    break;
                case 5:
                    bucket[5].push_back(DataSet.at(i));
                    bucketOrder[5].push_back(OrderID.at(i));
                    break;
                case 6:
                    bucket[6].push_back(DataSet.at(i));
                    bucketOrder[6].push_back(OrderID.at(i));
                    break;
                case 7:
                    bucket[7].push_back(DataSet.at(i));
                    bucketOrder[7].push_back(OrderID.at(i));
                    break;
                case 8:
                    bucket[8].push_back(DataSet.at(i));
                    bucketOrder[8].push_back(OrderID.at(i));
                    break;
                case 9:
                    bucket[9].push_back(DataSet.at(i));
                    bucketOrder[9].push_back(OrderID.at(i));
                    break;

                default:
                    break;
            }
        }

        //合并数据
        DataSet.clear();
        OrderID.clear();
        for(qint32 i=0; i<=9;i++)
        {
            if(bucket.at(i).size() ==0)
            {
                continue;
            }
            DataSet.append(bucket.at(i));
            OrderID.append(bucketOrder.at(i));
        }
        bucket.clear();
        bucketOrder.clear();
    }

    Order=OrderID;
}


