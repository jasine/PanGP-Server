#include "drawnewgenecurve.h"

DrawNewGeneCurve::DrawNewGeneCurve(PanProfileData PanProfileRef, CurveParameterForNewGeneData CurveParameterForNewGeneRef,FittingResultData FittingResultRef, QObject *parent) :
    QThread(parent)
{
// 获取传来的参数
    CurveParameterForNewGene=CurveParameterForNewGeneRef;
    PanProfile=PanProfileRef;
    FittingResult=FittingResultRef;


//计算分辨率以及各种尺寸的转换

    ImageDPI=CurveParameterForNewGene.ImageDPI;
    ImageDPM=ImageDPI*1.0/0.0254;
    ImageScaleRatio=ImageDPI/100;
    ImageFontRatio= 1 + (ImageScaleRatio-1)*0.1;
    ImageLineRatio= 1 + (ImageScaleRatio-1)*0.5;

//线条、点、字pen的粗细参数
    switch ((qint32)ImageScaleRatio) {
    case 1:
        FrameLineWidth=1;//主框架外框 1
        AxisMarkLineWidth=1;//刻度线 1
        AxisScaleTextWith=1;//刻度字 1
        AxisTitleTextWith=1;//坐标轴标题 1
        ImageLineWidth=2; //图像区线条 2
        ImageDotWidth=6; //图像区点的大小 6
        break;

    case 2:
        FrameLineWidth=1;//主框架外框 1
        AxisMarkLineWidth=1;//刻度线 1
        AxisScaleTextWith=1;//刻度字 1
        AxisTitleTextWith=1;//坐标轴标题 1
        ImageLineWidth=5; //图像区线条 2
        ImageDotWidth=6; //图像区点的大小 6
        break;

    case 3:
        FrameLineWidth=3;//主框架外框 1
        AxisMarkLineWidth=3;//刻度线 1
        AxisScaleTextWith=1;//刻度字 1
        AxisTitleTextWith=3;//坐标轴标题 1
        ImageLineWidth=7; //图像区线条 2
        ImageDotWidth=15; //图像区点的大小 6
        break;

    case 6:
        FrameLineWidth=6;//主框架外框 1
        AxisMarkLineWidth=6;//刻度线 1
        AxisScaleTextWith=1;//刻度字 1
        AxisTitleTextWith=1;//坐标轴标题 1
        ImageLineWidth=16; //图像区线条 2
        ImageDotWidth=20; //图像区点的大小 6
        break;

    default:
        break;
    }

// 根据分辨率做长度尺寸转换
    //图像大小
    ImageWidth=CurveParameterForNewGene.ImageWidth*ImageScaleRatio;
    ImageHeight=CurveParameterForNewGene.ImageHeight*ImageScaleRatio;

    //图层相对于窗体基调坐标
    LayerMarginTop=LayerMarginTopTemplete*ImageScaleRatio;
    LayerMarginBottom=LayerMarginBottomTemplete*ImageScaleRatio;
    LayerMarginLeft=LayerMarginLeftTemplete*ImageScaleRatio;
    LayerMarginRight=LayerMarginRightTemplete*ImageScaleRatio;

    FrameMarginTop=FrameMarginTopTemplete*ImageScaleRatio;
    FrameMarginBottom=FrameMarginBottomTemplete*ImageScaleRatio;
    FrameMarginLeft=FrameMarginLeftTemplete*ImageScaleRatio;
    FrameMarginRight=FrameMarginRightTemplete*ImageScaleRatio;

    DMarkDigit=DMarkDigitTemplete*ImageScaleRatio; //刻度与数字之间间距
    MarkLineLength=MarkLineLengthTemplete*ImageScaleRatio;//刻度线线长

    XLabelWidth=XLabelWidthTemplete*ImageScaleRatio; //X轴刻度数字域宽
    XLabelHeight=XLabelHeightTemplete*ImageScaleRatio;//X轴刻度数字域高

    YLabelWidth=YLabelWidthTemplete*ImageScaleRatio; //X轴刻度数字域宽
    YLabelHeight=YLabelHeightTemplete*ImageScaleRatio;//X轴刻度数字域高

    XAxisTitlePadding=XAxisTitlePaddingTemplete*ImageScaleRatio;// X轴标题的内边距
    XAxisTitleHeight=XAxisTitleHeightTemplete*ImageScaleRatio; // X轴标题的宽度

    YAxisTitlePadding=YAxisTitlePaddingTemplete*ImageScaleRatio;// X轴标题的内边距
    YAxisTitleWidth=YAxisTitleWidthTemplete*ImageScaleRatio; // X轴标题的宽度

//初始化各种参数
   //主框架 宽度和高度
    FrameWidth=ImageWidth - LayerMarginLeft - LayerMarginRight - FrameMarginLeft - FrameMarginRight;
    FrameHeight=ImageHeight - LayerMarginTop - LayerMarginBottom - FrameMarginTop - FrameMarginBottom;

   //主框架X、Y起始坐标位置
    FrameX=LayerMarginLeft+FrameMarginLeft;
    FrameY=LayerMarginTop+FrameMarginTop;

   //主框架 原点坐标  主框架上X、Y起始点在图层上真实坐标（图中所有点的位置都需要根据这个来计算）
    FrameXStartCoordinate =FrameX;
    FrameYStartCoordinate =FrameY+FrameHeight;

//debug
    qDebug() << "\n\n---------------------------------------\n\n";
    qDebug() << "image ImageScaleRatio:" << ImageScaleRatio;
    qDebug() << "DPI:" << CurveParameterForNewGene.ImageDPI;
    qDebug() << "image width raw:" << CurveParameterForNewGene.ImageWidth;
    qDebug() << "image width:" << ImageWidth;
    qDebug() << "image height raw:" << CurveParameterForNewGene.ImageHeight;
    qDebug() << "image height:" << ImageHeight;
    qDebug() << "\n\n---------------------------------------\n\n";


//初始化图层
    NewGeneCurveImage= QImage(ImageWidth,ImageHeight,QImage::Format_RGB888); // 设置图层大小和颜色集合
    QRgb backColor = qRgb(255,255,255); //图层背景颜色：白色
    NewGeneCurveImage.fill(backColor);

    //设置分辨率
    NewGeneCurveImage.setDotsPerMeterX(ImageDPM);
    NewGeneCurveImage.setDotsPerMeterY(ImageDPM);


}

DrawNewGeneCurve::~DrawNewGeneCurve()
{
    wait();
}


// 关掉自己
void DrawNewGeneCurve::slotKillMyself()
{
    qDebug() << "draw new gene curve thread is going to die!";
    terminate();
}

// 画图主程序
void DrawNewGeneCurve::run()
{
    if(CurveParameterForNewGene.IsNewGeneWindowOpen == false)
    {
        //画图启动
        sigdrawNewGeneCurveThreadProcessMsg("Drawing new gene curve ...");
    }

    //画主框架
    qDebug() << "draw the frame...";
    paintOutFrame();

    //计算X、Y轴的刻度个数以及刻度长度对于的画布上的比例
    qDebug() << "calculating x scale";
    CalculateXScaleNum();
    qDebug() << "calculating y scale";
    CalculateYScaleNum();

    //计算与画X轴刻度线
    qDebug() << "calculating and painting x scale mark";
    CalculateAndPaintXScaleMark();

    //计算与画Y轴刻度线
    qDebug() << "calculating and painting y scale mark";
    CalculateAndPaintYScaleMark();

    //计算与画X轴刻度数字
    qDebug() << "calculating and painting x scale digit";
    CalculateAndPaintXScaleDigit();

    //计算与画Y轴刻度数字
    qDebug() << "calculating and painting y scale digit";
    CalculateAndPaintYScaleDigit();

    //计算与画新基因散点
    qDebug() << "calculating and painting new point";
    CalculateAndPaintNewGeneBar();

    //趋势线
    if(CurveParameterForNewGene.toShowTrendLine==true)
    {
        qDebug() << "\n\n\n========================================\n\n\n";
        qDebug() << "fittype:" << FittingResult.FitType;
        qDebug() << "newM1A:" << FittingResult.newM1A;
        qDebug() << "newM1B:" << FittingResult.newM1B;
        qDebug() << "\n\n\n========================================\n\n\n";


        qDebug() << "calculating and painting trend line";
        CalculateAndPaintNewGeneTrendLine();
    }

    // 画X轴的标题
    qDebug() << "calculating and painting X axis title";
    paintXAxisTitle();

    // 画Y轴的标题
    qDebug() << "calculating and painting Y axis title";
    paintYAxisTitle();

    //debug
    paintOutFrame();

    // 图例
    /*
    if(CurveParameterForNewGene.toDrawNewGeneLegend==true)
    {
        qDebug() << "draw legend";
        paintLegend();
    }else
    {
        qDebug() << "skip legend";
    }*/

    // 判断新基因窗口是否打开来决定发送信号类型
    if(CurveParameterForNewGene.IsNewGeneWindowOpen == true)
    {
        qDebug() << "new gene image was created";
        emit sigdrawNewGeneCurveFinishRePaint(NewGeneCurveImage);
    }else
    {
        qDebug() << "new gene image was updated";
        emit sigdrawNewGeneCurveFinish(NewGeneCurveImage);
    }

}

// 画坐标轴外框
void DrawNewGeneCurve::paintOutFrame()
{
    QPainter painter(&NewGeneCurveImage);
    painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setWidth(FrameLineWidth);//  线宽度
    pen.setColor(Qt::darkGray);//画笔颜色
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);//刷子设为空
    painter.drawRect(FrameX,FrameY,FrameWidth,FrameHeight); //绘制矩形
}


//计算X轴的刻度个数以及刻度长度对于的画布上的比例
void DrawNewGeneCurve::CalculateXScaleNum()
{
    qint32 genomeNumber=PanProfile.GenomeNumber;

//计算刻度数目以及步长 开始
    if(genomeNumber<=25)
    {
        IsXMarkExtend=false;
        XMarkStartValue=1.5;
        XMarkNumber=genomeNumber;
        XMarkStep=1;

        // X轴上的所有刻度
        for(qint32 i=2;i<=genomeNumber;i++)
        {
            XMarkList.push_back(i);
        }

        XScale=FrameWidth/(genomeNumber*1.0 - 1);


    }else
    {
        qint32 minPartNum=5;//X轴被拆分的份数
        qint32 maxPartNum=10;
        qint32 maxStep;
        qint32 minStep;
        qint32 tmpStep;
        bool IsFound;
        qint32 step;

        //最大步长、最少刻度数
        maxStep=floor( genomeNumber/minPartNum );


        //最大步长、最少刻度数
        tmpStep=floor( genomeNumber/maxPartNum );
        if(tmpStep*maxPartNum == genomeNumber)
        {
            minStep=tmpStep;
        }else
        {
            minStep=tmpStep+1;
        }

        IsFound=false;
        for(qint32 i=minStep;i<=maxStep;i++)
        {

            if( (i%10) == 0 && genomeNumber>=90)
            {
                IsFound=true;
                step=i;
                break;
            }

            if((i%4) ==0)
            {
                IsFound=true;
                step=i;
                break;
            }

            if((i%2) ==0)
            {
                IsFound=true;
                step=i;
                break;
            }

            if((i%3) ==0)
            {
                IsFound=true;
                step=i;
                break;
            }

            if((i%5) ==0)
            {
                IsFound=true;
                step=i;
                break;
            }
        }

        if(IsFound)
        {
            XMarkStep=step;
        }else
        {
            XMarkStep=maxStep;
        }

        XMarkNumber= floor(genomeNumber/XMarkStep)+2;


        IsXMarkExtend=true;
        XMarkStartValue=1;
        // X轴上的所有刻度
        XMarkList.push_back(2); //第一个
        for(qint32 i=1;i<(XMarkNumber-1);i++)
        {
            XMarkList.push_back(i*XMarkStep);
        }

        XMarkList.push_back(genomeNumber); // 最后一个


        //计算刻度数目以及步长 结束
        //XScale=FrameWidth/((XMarkNumber-1);
        //XScale=FrameWidth/((XMarkNumber-1)*XMarkStep);
        XScale=FrameWidth/(genomeNumber);
    }
}


//计算Y轴的刻度个数以及刻度长度对于的画布上的比例
void DrawNewGeneCurve::CalculateYScaleNum()
{
    qint32 minPartNum=4;//Y轴被拆分的份数
    qint32 maxPartNum=6;
    qint32 maxStep;
    qint32 minStep;
    qint32 tmpStep;
    qint32 ymax;
    qint32 ymin;
    qint32 yRange;

    bool IsFound=false;
    qint32 step=0;

    // 计算实际最大值（由于新基因画的是直方图，所以最小值为0）
    qreal MaxValue=0.0;
    qreal MinValue=0.0;

    MaxValue=PanProfile.NewAvgValue[0];

    // 计算在趋势线中的最大值
    if(CurveParameterForNewGene.toShowTrendLine)
    {
        // 如果新基因没有拟合，该程序将会意外终止
        if(FittingResult.IsNewFittingFinished == false)
        {
            qDebug() << "Error: New gene data is not fitted yet!";
            exit(0);
        }

        // 计算最大值
        for(qint32 i=PanProfile.Newgi[0];i<=PanProfile.Newgi[PanProfile.Newgi.size()-1];i++)
        {
            //模型 1):y=A*x**B
            qreal fitYValue=FittingResult.newM1A*qPow(i,FittingResult.newM1B);
            if(fitYValue>MaxValue)
            {
                MaxValue=fitYValue;
            }
        }
    }

    //qreal DiffValue=MaxValue-MinValue;//Y轴上点值跨度
    // 为了使图形美观，坐标中最大值只占整个坐标系的75%
    qreal DiffValue=(MaxValue-MinValue)*4/3;//Y轴上点值跨度

    //最大步长、最少刻度数
    maxStep=qFloor(DiffValue*1.0/minPartNum);


    //最小步长、最多刻度数
    tmpStep=qFloor(DiffValue*1.0/maxPartNum);
    if(tmpStep*maxPartNum == DiffValue)
    {
        minStep=tmpStep;
    }else
    {
        minStep=tmpStep+1;
    }

    qDebug() << "minStep:" << minStep << "\tmaxStep:" << maxStep;

    //各种条件下的判断
    //以1为尺度单位来判断
    if(IsFound==false && maxStep<10)
    {
        step = maxStep;
        IsFound=true;
    }

    //以10为尺度单位来判断
    if(IsFound==false && maxStep<100)
    {
        step=10*qFloor(maxStep/10);
        IsFound=true;
    }

    if(IsFound==false && maxStep>=100 && maxStep<200 && minStep<=100)
    {
        step=100;
        IsFound=true;
    }

    if(IsFound==false && maxStep>=100 && maxStep<200 && minStep>100)
    {
        step=150;
        IsFound=true;
    }

    //以100为尺度单位来判断
    if(IsFound==false && maxStep<1000 && maxStep>=200)
    {
        step=100*qFloor(maxStep*1.0/100);
        IsFound=true;
    }

    //以1000为尺度单位来判断
    if(IsFound==false && maxStep<10000 && maxStep >=1000)
    {
        step=1000*qFloor(maxStep*1.0/1000);
        IsFound=true;
    }

    //以10000为尺度单位来判断
    if(IsFound==false &&maxStep >=10000)
    {
        step=10000*qFloor(maxStep*1.0/10000);
        IsFound=true;
    }

    ymin= step * qFloor(MinValue*1.0/step);
    ymax= step * qCeil(MaxValue*1.0/step);

    yRange=ymax-ymin;

    YMarkStep=step;
    YMarkNumber=1 + yRange/step;
    YMarkStartValue=ymin;

    qDebug() << "ymin:" << ymin << "\tstep:" << step << "\tymax:"  << ymax << "\tyrange:" << yRange;
    qDebug() << "ystart:" << YMarkStartValue;
    qDebug() << "ymarknumber:" << YMarkNumber;
    qDebug() << "ystep:" << YMarkStep;

//计算刻度数目以及步长 结束
    //YScale=FrameHeight/(YMarkNumber-1);
    YScale=FrameHeight/yRange;
}

//计算并画X轴刻度线
void DrawNewGeneCurve::CalculateAndPaintXScaleMark()
{
    qreal YStart=FrameYStartCoordinate;
    qreal YEnd=FrameYStartCoordinate+MarkLineLength;

/* 设置画笔信息   开始*/
    QPainter painter(&NewGeneCurveImage);
    painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setWidth(AxisMarkLineWidth);//  线宽度
    pen.setColor(Qt::darkGray);//画笔颜色
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);//刷子设为空
/* 设置画笔信息   结束*/


    //计算各条线的位置
    for(qint32 i=0; i<XMarkList.size();i++)
    {
        QPoint startPoint,endPoint;// 每条线的起始位点和终止位点
        qreal curX=FrameXStartCoordinate + XScale *(XMarkList[i]*1.0 - XMarkStartValue); // X坐标位置

        startPoint.setX(curX);
        startPoint.setY(YStart);
        endPoint.setX(curX);
        endPoint.setY(YEnd);
        painter.drawLine(startPoint,endPoint);
    }

}


//计算并画Y轴刻度线
void DrawNewGeneCurve::CalculateAndPaintYScaleMark()
{

    qreal XStart=FrameXStartCoordinate-MarkLineLength;
    qreal XEnd=FrameXStartCoordinate;

/* 设置画笔信息   开始*/
    QPainter painter(&NewGeneCurveImage);
    painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setWidth(AxisMarkLineWidth);//  线宽度
    pen.setColor(Qt::darkGray);//画笔颜色
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);//刷子设为空
/* 设置画笔信息   结束*/


    //计算各条线的位置
    for(qint32 i=0; i<YMarkNumber;i++)
    {
        QPoint startPoint,endPoint;// 每条线的起始位点和终止位点
        qreal curY=FrameYStartCoordinate - 1.0*i*YMarkStep*YScale; // X坐标位置

        startPoint.setX(XStart);
        startPoint.setY(curY);
        endPoint.setX(XEnd);
        endPoint.setY(curY);
        painter.drawLine(startPoint,endPoint);
    }
}


//计算并画X轴刻度数字
void DrawNewGeneCurve::CalculateAndPaintXScaleDigit()
{
    qreal YCoord=FrameYStartCoordinate+MarkLineLength+DMarkDigit;

/* 设置画笔信息   开始*/
    QPainter painter(&NewGeneCurveImage);
    painter.setRenderHint(QPainter::TextAntialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setWidth(AxisScaleTextWith);//  线宽度
    pen.setColor(Qt::black);//画笔颜色
    painter.setPen(pen);
    QFont font;
    font.setPointSize(CurveParameterForNewGene.NewGeneXScaleValueSize);
    font.setFamily(CurveParameterForNewGene.NewGeneXScaleValueFont.family());
    painter.setFont(font);
    painter.setBrush(Qt::NoBrush);//刷子设为空
/* 设置画笔信息   结束*/


    //计算各条线的位置
    for(qint32 i=0; i<XMarkList.size();i++)
    {
        //QString XlabelText=QString::number(0+i*XMarkStep);
        QString XlabelText=QString::number(XMarkList[i]);
        QRect labelRect;
        QPoint pointTopLeft,pointBottomRight;

        qreal tlx,tly,brx,bry;
        tlx= FrameXStartCoordinate + XScale *(XMarkList[i]*1.0 - XMarkStartValue) - XLabelWidth*0.5;
        //tlx=FrameXStartCoordinate + i*XMarkStep*XScale - XLabelWidth*0.5;
        brx=tlx+XLabelWidth;
        tly=YCoord;
        bry=YCoord+XLabelHeight;

        pointTopLeft.setX(tlx);
        pointTopLeft.setY(tly);

        pointBottomRight.setX(brx);
        pointBottomRight.setY(bry);

        labelRect.setTopLeft(pointTopLeft);
        labelRect.setBottomRight(pointBottomRight);

        painter.drawText(labelRect,Qt::AlignCenter,XlabelText);
    }
}


//计算并画Y轴刻度数字
void DrawNewGeneCurve::CalculateAndPaintYScaleDigit()
{
    qreal XCoord=FrameXStartCoordinate-MarkLineLength-DMarkDigit;

/* 设置画笔信息   开始*/
    QPainter painter(&NewGeneCurveImage);
    painter.setRenderHint(QPainter::TextAntialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setWidth(AxisScaleTextWith);//  线宽度
    pen.setColor(Qt::black);//画笔颜色
    painter.setPen(pen);
    QFont font;
    font.setPointSize(CurveParameterForNewGene.NewGeneYScaleValueSize);
    font.setFamily(CurveParameterForNewGene.NewGeneYScaleValueFont.family());
    painter.setFont(font);
    painter.setBrush(Qt::NoBrush);//刷子设为空
/* 设置画笔信息   结束*/



    //计算各条线的位置
    for(qint32 i=0; i<YMarkNumber;i++)
    {
        QString YlabelText=QString::number(YMarkStartValue+i*YMarkStep);
        QRect labelRect;
        QPoint pointTopLeft,pointBottomRight;

        qreal tlx,tly,brx,bry;
        tlx=XCoord-YLabelWidth;
        brx=XCoord;
        tly=FrameYStartCoordinate - i*YMarkStep*YScale - YLabelHeight*0.5;
        bry=tly+YLabelHeight;

        pointTopLeft.setX(tlx);
        pointTopLeft.setY(tly);

        pointBottomRight.setX(brx);
        pointBottomRight.setY(bry);

        labelRect.setTopLeft(pointTopLeft);
        labelRect.setBottomRight(pointBottomRight);

        painter.drawText(labelRect,Qt::AlignCenter,YlabelText);
    }
}


//新基因的直方图
void DrawNewGeneCurve::CalculateAndPaintNewGeneBar()
{
/* 设置画笔信息   开始*/
    QPainter painter(&NewGeneCurveImage);
    painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线

    QPen pen; //笔
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::BevelJoin);
    pen.setWidth(0);//  线宽度
    pen.setColor(CurveParameterForNewGene.BarColor);//画笔颜色
    painter.setPen(pen);

    QBrush brush; // 刷子
    brush.setColor(CurveParameterForNewGene.BarColor);//刷子颜色
    brush.setStyle(Qt::SolidPattern);

    painter.setBrush(brush);//刷子设为空
/* 设置画笔信息   结束*/

    qint32 BarNumber=PanProfile.NewAvgValue.size();//点数目

    for(qint32 i=0; i<BarNumber;i++)
    {
        QRect BarRect;
        qint32 XValue=PanProfile.Newgi[i];
        qint32 YValue=PanProfile.NewAvgValue[i];

        qreal TopLeftX=FrameXStartCoordinate + (XValue - BarWidthRatio - XMarkStartValue)*XScale;
        qreal TopLeftY=FrameYStartCoordinate - (YValue-YMarkStartValue)*YScale;

        qreal BottomRightX=FrameXStartCoordinate + (XValue + BarWidthRatio - XMarkStartValue)*XScale;
        qreal BottomRightY=FrameYStartCoordinate;

        BarRect.setTopLeft(QPoint(TopLeftX,TopLeftY));
        BarRect.setBottomRight(QPoint(BottomRightX,BottomRightY));

        painter.drawRect(BarRect);
    }

}


//趋势线
void DrawNewGeneCurve::CalculateAndPaintNewGeneTrendLine()
{
    qreal Xpre;
    qreal Ypre;
/* 设置画笔信息   开始*/
    QPainter painter(&NewGeneCurveImage);
    painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setWidth(ImageLineWidth);//  线宽度
    pen.setStyle(Qt::DotLine);
    pen.setColor(CurveParameterForNewGene.TrendLineColor);//画笔颜色
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);//刷子设为空
    /* 设置画笔信息   结束*/

    QList <qreal> simulationX;
    QList <qreal> simulationY;

    qDebug()<< "A:" << FittingResult.newM1A;
    qDebug()<< "B:" << FittingResult.newM1B;

    for(qreal sx=2.0; sx<=PanProfile.GenomeNumber;sx=sx+0.002)
    {
        simulationX.push_back(sx);
        //模型 1):y=A*x**B
        qreal sy = FittingResult.newM1A * qPow(sx,FittingResult.newM1B);
        simulationY.push_back(sy);

    }

    qint32 pointNumber=simulationX.size();//点数目



    for(qint32 i=0; i<pointNumber;i++)
    {
        qreal XValue=simulationX[i];
        qreal YValue=simulationY[i];

        qreal X=FrameXStartCoordinate + (XValue - XMarkStartValue)*XScale;
        qreal Y=FrameYStartCoordinate - (YValue-YMarkStartValue)*YScale;



        painter.drawPoint(X,Y);

        if(i>0)
        {
            painter.drawLine(Xpre,Ypre,X,Y);
        }
        Xpre=X;
        Ypre=Y;
    }
}


// 画X轴的标题
void DrawNewGeneCurve::paintXAxisTitle()
{
    /* 设置画笔信息   开始*/
        QPainter painter(&NewGeneCurveImage);
        painter.setRenderHint(QPainter::TextAntialiasing,true);//平滑直线
        QPen pen; //笔
        QFont font(CurveParameterForNewGene.NewGeneXTitleFont.family(), CurveParameterForNewGene.NewGeneXTitleSize, QFont::Normal) ;// 字体信息
        pen.setWidth(AxisTitleTextWith);//  线宽度
        pen.setColor(Qt::black);//画笔颜色
        painter.setPen(pen);
        painter.setFont(font);
        painter.setBrush(Qt::NoBrush);//刷子设为空
    /* 设置画笔信息   结束*/

    QPoint pointTopLeft,pointbottomRight;
    QRect  textRect;
    qreal tlx,tly,brx,bry;

//  X 轴
    qreal YCoord=FrameYStartCoordinate + MarkLineLength + DMarkDigit + XLabelHeight + XAxisTitlePadding;

    tlx = FrameXStartCoordinate;
    brx = FrameXStartCoordinate + FrameWidth;
    tly = YCoord;
    bry = tly - XAxisTitleHeight;

    pointTopLeft.setX(tlx);
    pointTopLeft.setY(tly);
    pointbottomRight.setX(brx);
    pointbottomRight.setY(bry);

    textRect.setTopLeft(pointTopLeft);
    textRect.setBottomRight(pointbottomRight);

    painter.drawText(textRect,Qt::AlignCenter,CurveParameterForNewGene.NewGeneXTitle);
}


// 画Y轴的标题
void DrawNewGeneCurve::paintYAxisTitle()
{

    /* 设置画笔信息   开始*/
        QPainter painter(&NewGeneCurveImage);
        painter.setRenderHint(QPainter::TextAntialiasing,true);//平滑直线
        QPen pen; //笔
        QFont font(CurveParameterForNewGene.NewGeneYTitleFont.family(), CurveParameterForNewGene.NewGeneYTitleSize, QFont::Normal) ;// 字体信息
        pen.setWidth(AxisTitleTextWith);//  线宽度
        pen.setColor(Qt::black);//画笔颜色
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);//刷子设为空
        painter.setFont(font);
    /* 设置画笔信息   结束*/

    QPoint pointTopLeft,pointbottomRight;
    QRect  textRect;
    qreal brx,bry;

//  Y 轴
    qreal XCoord=FrameXStartCoordinate - MarkLineLength - DMarkDigit - YLabelWidth - YAxisTitlePadding;

    //tlx = XCoord - YAxisTitleWidth;
    brx = XCoord;
    //tly = FrameYStartCoordinate - FrameHeight;
    bry = FrameYStartCoordinate;

    // 临时旋转一下,世界坐标系
    QTransform transform;
    transform.translate(brx,bry);//转移原点
    transform.rotate(-90.0);
    painter.setWorldTransform(transform);

    // 旋转之后那地方的坐标
    pointTopLeft.setX(0);
    pointTopLeft.setY(YAxisTitleWidth);
    pointbottomRight.setX(FrameHeight);
    pointbottomRight.setY(0);

    textRect.setTopLeft(pointTopLeft);
    textRect.setBottomRight(pointbottomRight);
/*
    QPoint tmp1,tmp2;
    tmp1.setX(200);
    tmp1.setY(200);

    tmp2.setX(400);
    tmp2.setY(400);

    textRect.setTopLeft(tmp1);
    textRect.setBottomRight(tmp2); */

    painter.drawText(textRect,Qt::AlignCenter,CurveParameterForNewGene.NewGeneYTitle);
}

/*
void DrawNewGeneCurve::paintLegend()
{
    // 设置画笔信息   开始
        QPainter painter(&NewGeneCurveImage);
        painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
        QPen pen; //笔
        painter.setBrush(Qt::NoBrush);//刷子设为空
        pen.setWidth(1);//  线宽度
        pen.setColor(Qt::black);//画笔颜色
        painter.setPen(pen);

    // 设置画笔信息   结束


    qreal lx,ly;
    QPoint pointLeft,pointRight,pointMid;
    QRect TextRect;

    lx=CurveParameterForNewGene.NewGeneLegendTopLeftX;
    ly=CurveParameterForNewGene.NewGeneLegendTopLeftY;

    //legend outline
    painter.drawRect(lx,ly,LegendWidth,LegendHeight);


//newgene
    // 改变画笔属性
    pen.setWidth(2);//  线宽度
    pen.setColor(CurveParameterForNewGene.TrendLineColor);//画笔颜色
    painter.setPen(pen);

    pointLeft.setX(lx+LegendHOffset);
    pointLeft.setY(ly+LegendVOffset*0.5);
    pointRight.setX(lx+LegendHOffset+LegendLineLength);
    pointRight.setY(ly+LegendVOffset*0.5);

    painter.drawLine(pointLeft,pointRight);

    // 图例后面的文字
    pointLeft.setX(lx+LegendHOffset+LegendLineLength+LegendHOffset*2);
    pointLeft.setY(ly);
    pointRight.setX(pointLeft.x()+LegendTextWidth);
    pointRight.setY(pointLeft.y()+LegendVOffset);

    TextRect.setTopLeft(pointLeft);
    TextRect.setBottomRight(pointRight);

    painter.drawText(TextRect,Qt::AlignVCenter,"New Gene Size");
}

*/
