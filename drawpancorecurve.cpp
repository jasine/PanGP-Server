#include "drawpancorecurve.h"

DrawPanCoreCurve::DrawPanCoreCurve(PanProfileData PanProfileRef,CurveParameterForPanCoreData CurveParameterForPanCoreRef,QObject *parent) :
    QThread(parent)
{

//获取参数
    CurveParameterForPanCore=CurveParameterForPanCoreRef; //图像以及窗体参数
    panProfile=PanProfileRef; //泛基因组数据


//计算分辨率以及各种尺寸的转换

    ImageDPI=CurveParameterForPanCore.ImageDPI;
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
        LegendFrameLineWidth=1;//图例外框线
        LegendTitleTextWidth=1;//图例字
        LegendFontSize=12; // 图例字号
        break;

    case 2:
        FrameLineWidth=1;//主框架外框 1
        AxisMarkLineWidth=1;//刻度线 1
        AxisScaleTextWith=1;//刻度字 1
        AxisTitleTextWith=1;//坐标轴标题 1
        ImageLineWidth=5; //图像区线条 2
        ImageDotWidth=6; //图像区点的大小 6
        LegendFrameLineWidth=2;//图例外框线
        LegendTitleTextWidth=1;//图例字
        LegendFontSize=12; // 图例字号
        break;

    case 3:
        FrameLineWidth=3;//主框架外框 1
        AxisMarkLineWidth=3;//刻度线 1
        AxisScaleTextWith=1;//刻度字 1
        AxisTitleTextWith=3;//坐标轴标题 1
        ImageLineWidth=7; //图像区线条 2
        ImageDotWidth=15; //图像区点的大小 6
        LegendFrameLineWidth=3;//图例外框线
        LegendTitleTextWidth=1;//图例字
        LegendFontSize=12; // 图例字号
        break;

    case 6:
        FrameLineWidth=6;//主框架外框 1
        AxisMarkLineWidth=6;//刻度线 1
        AxisScaleTextWith=1;//刻度字 1
        AxisTitleTextWith=1;//坐标轴标题 1
        ImageLineWidth=16; //图像区线条 2
        ImageDotWidth=20; //图像区点的大小 6
        LegendFrameLineWidth=6;//图例外框线
        LegendTitleTextWidth=20;//图例字
        LegendFontSize=12; // 图例字号
        break;

    default:
        break;
    }

// 根据分辨率做长度尺寸转换
    //图像大小
    ImageWidth=CurveParameterForPanCore.ImageWidth*ImageScaleRatio;
    ImageHeight=CurveParameterForPanCore.ImageHeight*ImageScaleRatio;

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

    //图例的各种计算转换
    LegendWidth=LegendWidthTemplete*ImageScaleRatio;
    LegendHeight=LegendHeightTemplete*ImageScaleRatio;
    LegendLineLength=LegendLineLengthTemplete*ImageScaleRatio;
    LegendHOffset=LegendHOffsetTemplete*ImageScaleRatio; // 水平间距
    LegendVOffset=LegendVOffsetTemplete*ImageScaleRatio;
    LegendTextWidth=LegendTextWidthTemplete*ImageScaleRatio;

    BoxplotHalfWidth=BoxplotHalfWidthTemplete*ImageScaleRatio; // boxplot图的半宽度

//计算颜色
    CurveParameterForPanCore.PanColor =CurveParameterForPanCore.AllColor[CurveParameterForPanCore.PanColorID];
    CurveParameterForPanCore.CoreColor=CurveParameterForPanCore.AllColor[CurveParameterForPanCore.CoreColorID];


//窗体是否打开，该参数决定图像绘制完后发出的信号类型
    IsPanWindowOpen =CurveParameterForPanCore.IsPanWindowOpen;

//  初始化各种参数
   //主框架 宽度和高度

    FrameWidth=ImageWidth - LayerMarginLeft - LayerMarginRight - FrameMarginLeft - FrameMarginRight;
    FrameHeight=ImageHeight - LayerMarginTop - LayerMarginBottom - FrameMarginTop - FrameMarginBottom;



   //主框架X、Y起始坐标位置
   FrameX=LayerMarginLeft+FrameMarginLeft;
   FrameY=LayerMarginTop+FrameMarginTop;

   //主框架 原点坐标  主框架上X、Y起始点在图层上真实坐标（图中所有点的位置都需要根据这个来计算）
   FrameXStartCoordinate =FrameX;
   FrameYStartCoordinate =FrameY+FrameHeight;

//初始化图层
    PanCurveImage= QImage(ImageWidth,ImageHeight,QImage::Format_RGB888); // 设置图层大小和颜色集合

    //设置分辨率
    PanCurveImage.setDotsPerMeterX(ImageDPM);
    PanCurveImage.setDotsPerMeterY(ImageDPM);


    QRgb backColor = qRgb(255,255,255); //图层背景颜色：白色
    PanCurveImage.fill(backColor);
}



DrawPanCoreCurve::~DrawPanCoreCurve()
{
    wait();
}


void DrawPanCoreCurve::slotKillMyself()
{
    qDebug() << "draw thread is going to die!";
    terminate();
}

void DrawPanCoreCurve::run()
{
    if(CurveParameterForPanCore.IsPanWindowOpen == false)
    {
        qDebug() << "draw thread start";
        emit sigdrawCurveThreadProcessMsg("Drawing pan-genome and core genome curve ...");
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

    if(CurveParameterForPanCore.toShowBoxplot)
    {
        // 计算与画泛基因组箱线图
        qDebug() << "calculating and painting pan boxplot";
        CalculateAndPaintPanBoxplot();

        // 计算与画核心基因箱线图
        qDebug() << "calculating and painting core boxplot";
        CalculateAndPaintCoreBoxplot();


    }else
    {
        //计算与画泛基因组散点
        qDebug() << "calculating and painting pan points";
        CalculateAndPaintPanPoint();

        //计算与画核心基因组散点
        qDebug() << "calculating and painting core points";
        CalculateAndPaintCorePoint();
    }



    //计算与画泛基因组曲线
    qDebug() << "calculating and painting pan curve";
    CalculateAndPaintPanLine();

    //计算与画核心基因组曲线
    qDebug() << "calculating and painting core curve";
    CalculateAndPaintCoreLine();

    // 画X轴的标题
    qDebug() << "calculating and painting X axis title";
    paintXAxisTitle();

    // 画Y轴的标题
    qDebug() << "calculating and painting Y axis title";
    paintYAxisTitle();

    // 画图例
    if(CurveParameterForPanCore.toDrawPanLegend==true)
    {
        qDebug() << "calculating and painting legend";
        paintLegend();
    }else
    {
        qDebug() << "skip";
    }




    qDebug() << "draw thread end";
    //计算结束发出返回数据信号
    if(IsPanWindowOpen ==false)
    {
        qDebug() << "Image created";
        emit sigdrawCurveFinish(PanCurveImage);
    }else
    {
        qDebug() << "Image changed";
        emit sigdrawCurveFinishRePaint(PanCurveImage);
    }
}


//计算X轴的刻度个数以及刻度长度对于的画布上的比例
void DrawPanCoreCurve::CalculateXScaleNum()
{
    qint32 genomeNumber=panProfile.GenomeNumber;

//计算刻度数目以及步长 开始
    if(genomeNumber<=14)
    {
        XMarkNumber=genomeNumber+2;
        XMarkStep=1;
    }else
    {
        qint32 minPartNum=6;//X轴被拆分的份数
        qint32 maxPartNum=12;
        qint32 maxStep;
        qint32 minStep;
        qint32 tmpStep;
        bool IsFound;
        qint32 step;

        //最大步长、最少刻度数
        maxStep=qFloor(genomeNumber*1.0/minPartNum );


        //最大步长、最少刻度数
        tmpStep=qFloor(genomeNumber*1.0/maxPartNum );
        if(tmpStep*maxPartNum == genomeNumber)
        {
            minStep=tmpStep;
        }else
        {
            minStep=tmpStep+1;
        }

        IsFound=false;
        for(qint32 i=maxStep;i>=minStep;i--)
        {
            if( (i%100) == 0)
            {
                IsFound=true;
                step=i;
                break;
            }


            if( (i%50) == 0 && genomeNumber<600)
            {
                IsFound=true;
                step=i;
                break;
            }

            if( (i%10) == 0 && genomeNumber<300)
            {
                IsFound=true;
                step=i;
                break;
            }

            if((i%5) ==0 && genomeNumber<60)
            {
                IsFound=true;
                step=i;
                break;
            }

            if((i%4) ==0 && genomeNumber<25)
            {
                IsFound=true;
                step=i;
                break;
            }

            if((i%3) ==0 && genomeNumber<20)
            {
                IsFound=true;
                step=i;
                break;
            }

            if((i%2) ==0 && genomeNumber<20)
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

       /* if( floor(genomeNumber/XMarkStep) * XMarkStep ==genomeNumber  )
        {
            XMarkNumber=floor(genomeNumber/XMarkStep)+1;
        }else
        {
            XMarkNumber= floor(genomeNumber/XMarkStep)+2;
        } */

        XMarkNumber= qFloor(genomeNumber*1.0/XMarkStep)+2;

    }
//计算刻度数目以及步长 结束
    //XScale=FrameWidth/((XMarkNumber-1);
     XScale=FrameWidth/((XMarkNumber-1)*XMarkStep);
}


//计算Y轴的刻度个数以及刻度长度对于的画布上的比例
void DrawPanCoreCurve::CalculateYScaleNum()
{
    qint32 MaxValue=panProfile.PanAvgValue[panProfile.PanAvgValue.size()-1];//实际最大值
    qint32 MinValue=panProfile.CoreAvgValue[panProfile.CoreAvgValue.size()-1];//实际最小值
    //qint32 MaxValue;
    //qint32 MinValue;

    qint32 minPartNum=4;//Y轴被拆分的份数
    qint32 maxPartNum=6;
    qint32 maxStep;
    qint32 minStep;
    qint32 tmpStep;
    qint32 ymax;
    qint32 ymin;
    qint32 yRange;

// 计算Y轴上的最大值以及最小值
    if(CurveParameterForPanCore.toShowBoxplot==true)
    {
        if(MaxValue < panProfile.pancoreBoxplotYMax)
        {
            MaxValue = panProfile.pancoreBoxplotYMax;
        }

        if(MinValue > panProfile.pancoreBoxplotYMin)
        {
            MinValue = panProfile.pancoreBoxplotYMin;
        }
    }

    bool IsFound=false;
    qint32 step=0;
    qint32 DiffValue=MaxValue-MinValue;//Y轴上点值跨度

    //最大步长、最少刻度数
    maxStep=qCeil(DiffValue*1.0/minPartNum);


    //最小步长、最多刻度数
    tmpStep=qFloor(DiffValue*1.0/maxPartNum);
    if(tmpStep*maxPartNum == DiffValue)
    {
        minStep=tmpStep;
    }else
    {
        minStep=tmpStep+1;
    }

    //各种条件下的判断
    //以10为尺度单位来判断
    if(IsFound==false && maxStep<100)
    {
        step=10*qFloor(maxStep*1.0/10);
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

//计算刻度数目以及步长 结束
    //YScale=FrameHeight/(YMarkNumber-1);
    YScale=FrameHeight/yRange;
}


//画坐标系的外框
void DrawPanCoreCurve::paintOutFrame()
{
    QPainter painter(&PanCurveImage);
    painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setWidth(FrameLineWidth);//  线宽度
    pen.setColor(Qt::darkGray);//画笔颜色
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);//刷子设为空
    painter.drawRect(FrameX,FrameY,FrameWidth,FrameHeight); //绘制矩形
}


//计算并画X轴刻度线
void DrawPanCoreCurve::CalculateAndPaintXScaleMark()
{
    qreal YStart=FrameYStartCoordinate;
    qreal YEnd=FrameYStartCoordinate+MarkLineLength;

/* 设置画笔信息   开始*/
    QPainter painter(&PanCurveImage);
    painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setWidth(AxisMarkLineWidth);//  线宽度
    pen.setColor(Qt::darkGray);//画笔颜色
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);//刷子设为空
/* 设置画笔信息   结束*/

    //计算各条线的位置
    for(qint32 i=0; i<XMarkNumber;i++)
    {
        QPoint startPoint,endPoint;// 每条线的起始位点和终止位点
        qreal curX=FrameXStartCoordinate + XScale *1.0*i*XMarkStep; // X坐标位置

        startPoint.setX(curX);
        startPoint.setY(YStart);
        endPoint.setX(curX);
        endPoint.setY(YEnd);
        painter.drawLine(startPoint,endPoint);
    }
}


//计算并画Y轴刻度线
void DrawPanCoreCurve::CalculateAndPaintYScaleMark()
{

    qreal XStart=FrameXStartCoordinate-MarkLineLength;
    qreal XEnd=FrameXStartCoordinate;

/* 设置画笔信息   开始*/
    QPainter painter(&PanCurveImage);
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
void DrawPanCoreCurve::CalculateAndPaintXScaleDigit()
{
    qreal YCoord=FrameYStartCoordinate+MarkLineLength+DMarkDigit;

/* 设置画笔信息   开始*/
    QPainter painter(&PanCurveImage);
    painter.setRenderHint(QPainter::TextAntialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setWidth(AxisScaleTextWith);//  线宽度
    pen.setColor(Qt::black);//画笔颜色
    painter.setPen(pen);
    QFont font;
    font.setPointSize(ImageFontRatio*CurveParameterForPanCore.PanCoreXScaleValueSize);
    font.setFamily(CurveParameterForPanCore.PanCoreXScaleValueFont.family());
    painter.setFont(font);
    painter.setBrush(Qt::NoBrush);//刷子设为空
/* 设置画笔信息   结束*/

    //计算各条线的位置
    for(qint32 i=0; i<XMarkNumber;i++)
    {
        QString XlabelText=QString::number(0+i*XMarkStep);
        QRect labelRect;
        QPoint pointTopLeft,pointBottomRight;

        qreal tlx,tly,brx,bry;
        tlx=FrameXStartCoordinate + i*XMarkStep*XScale - XLabelWidth*0.5;
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
void DrawPanCoreCurve::CalculateAndPaintYScaleDigit()
{
    qreal XCoord=FrameXStartCoordinate-MarkLineLength-DMarkDigit;

/* 设置画笔信息   开始*/
    QPainter painter(&PanCurveImage);
    painter.setRenderHint(QPainter::TextAntialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setWidth(AxisScaleTextWith);//  线宽度
    pen.setColor(Qt::black);//画笔颜色
    painter.setPen(pen);
    QFont font;
    font.setPointSize(ImageFontRatio*CurveParameterForPanCore.PanCoreYScaleValueSize);
    font.setFamily(CurveParameterForPanCore.PanCoreYScaleValueFont.family());
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


//泛基因组大小散点
void DrawPanCoreCurve::CalculateAndPaintPanPoint()
{
    /* 设置画笔信息   开始*/
        QPainter painter(&PanCurveImage);
        painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
        QPen pen; //笔
        QFont font;
        //pen.setCapStyle(Qt::FlatCap);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setWidth(ImageDotWidth);//  线宽度
        pen.setColor(CurveParameterForPanCore.PanColor);//画笔颜色
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);//刷子设为空
    /* 设置画笔信息   结束*/

        qint32 pointNumber=panProfile.GenomeNumValue.size();//点数目

        for(qint32 i=0; i<pointNumber;i++)
        {
            qint32 XValue=panProfile.GenomeNumValue[i];
            qint32 YValue=panProfile.PanValue[i];

            qreal X=FrameXStartCoordinate + (XValue-0)*XScale;
            qreal Y=FrameYStartCoordinate - (YValue-YMarkStartValue)*YScale;
            //qDebug() << "X:" << X << "\tY:" << Y;
            painter.drawPoint(X,Y);
        }
}


//核心基因组大小散点
void DrawPanCoreCurve::CalculateAndPaintCorePoint()
{
    /* 设置画笔信息   开始*/
        QPainter painter(&PanCurveImage);
        painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
        QPen pen; //笔
        pen.setCapStyle(Qt::RoundCap);
        //pen.setJoinStyle(Qt::RoundJoin);
        pen.setWidth(ImageDotWidth);//  线宽度
        pen.setColor(CurveParameterForPanCore.CoreColor);//画笔颜色
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);//刷子设为空
    /* 设置画笔信息   结束*/

        qint32 pointNumber=panProfile.GenomeNumValue.size();//点数目

        for(qint32 i=0; i<pointNumber;i++)
        {
            qint32 XValue=panProfile.GenomeNumValue[i];
            qint32 YValue=panProfile.CoreValue[i];

            qreal X=FrameXStartCoordinate + (XValue-0)*XScale;
            qreal Y=FrameYStartCoordinate - (YValue-YMarkStartValue)*YScale;
            //qDebug() << "X:" << X << "\tY:" << Y;
            painter.drawPoint(X,Y);
        }

}


//泛基因组曲线
void DrawPanCoreCurve::CalculateAndPaintPanLine()
{
        qreal Xpre=0.0;
        qreal Ypre=0.0;
    /* 设置画笔信息   开始*/
        QPainter painter(&PanCurveImage);
        painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线

        QPen pen; //笔
        pen.setCapStyle(Qt::RoundCap);
        //pen.setJoinStyle(Qt::RoundJoin);
        pen.setJoinStyle(Qt::MiterJoin);
        pen.setWidth(ImageLineWidth);//  线宽度
        pen.setColor(CurveParameterForPanCore.PanColor);//画笔颜色

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);//刷子设为空
    /* 设置画笔信息   结束*/

        qint32 pointNumber=panProfile.PanAvgValue.size();//点数目

        for(qint32 i=0; i<pointNumber;i++)
        {
            qint32 XValue=i+1;
            qint32 YValue=panProfile.PanAvgValue[i];

            qreal X=FrameXStartCoordinate + (XValue-0)*XScale;
            qreal Y=FrameYStartCoordinate - (YValue-YMarkStartValue)*YScale;
            //qDebug() << "X:" << X << "\tY:" << Y;
            painter.drawPoint(X,Y);

            if(i>0)
            {
                painter.drawLine(Xpre,Ypre,X,Y);
            }
            Xpre=X;
            Ypre=Y;
        }
}


//核心基因组曲线
void DrawPanCoreCurve::CalculateAndPaintCoreLine()
{
    qreal Xpre=0.0;
    qreal Ypre=0.0;
/* 设置画笔信息   开始*/
    QPainter painter(&PanCurveImage);
    painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setWidth(ImageLineWidth);//  线宽度
    pen.setColor(CurveParameterForPanCore.CoreColor);//画笔颜色
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);//刷子设为空
/* 设置画笔信息   结束*/

    qint32 pointNumber=panProfile.CoreAvgValue.size();//点数目

    for(qint32 i=0; i<pointNumber;i++)
    {
        qint32 XValue=i+1;
        qint32 YValue=panProfile.CoreAvgValue[i];

        qreal X=FrameXStartCoordinate + (XValue-0)*XScale;
        qreal Y=FrameYStartCoordinate - (YValue-YMarkStartValue)*YScale;
        //qDebug() << "X:" << X << "\tY:" << Y;
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
void DrawPanCoreCurve::paintXAxisTitle()
{
    /* 设置画笔信息   开始*/
        QPainter painter(&PanCurveImage);
        painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
        QPen pen; //笔
        QFont font(CurveParameterForPanCore.PanCoreXTitleFont.family(), ImageFontRatio*CurveParameterForPanCore.PanCoreXTitleSize, QFont::Normal) ;// 字体信息
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

    painter.drawText(textRect,Qt::AlignCenter,CurveParameterForPanCore.PanCoreXTitle);
}

void DrawPanCoreCurve::paintYAxisTitle()
{

    /* 设置画笔信息   开始*/
        QPainter painter(&PanCurveImage);
        painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
        QPen pen; //笔
        QFont font(CurveParameterForPanCore.PanCoreYTitleFont.family(), ImageFontRatio*CurveParameterForPanCore.PanCoreYTitleSize, QFont::Normal) ;// 字体信息
        pen.setWidth(AxisTitleTextWith);//  线宽度
        pen.setColor(Qt::black);//画笔颜色
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);//刷子设为空
        painter.setFont(font);
    /* 设置画笔信息   结束*/

    QPoint pointTopLeft,pointbottomRight;
    QRect  textRect;
    //qreal tlx,tly;
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

    painter.drawText(textRect,Qt::AlignCenter,CurveParameterForPanCore.PanCoreYTitle);
}


void DrawPanCoreCurve::paintLegend()
{
/* 设置画笔信息   开始*/
    QPainter painter(&PanCurveImage);
    painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
    QPen pen; //笔
    QFont font;


/* 设置画笔信息   结束*/

//图例边框
    painter.setBrush(Qt::NoBrush);//刷子设为空
    pen.setWidth(LegendFrameLineWidth);//  线宽度
    pen.setColor(Qt::black);//画笔颜色
    painter.setPen(pen);

    qreal lx,ly;
    QPoint pointLeft,pointRight,pointMid;
    QRect TextRect;


    lx=ImageScaleRatio*CurveParameterForPanCore.LegendTopLeftX;
    ly=ImageScaleRatio*CurveParameterForPanCore.LegendTopLeftY;

    //legend outline
    painter.drawRect(lx,ly,LegendWidth,LegendHeight);


//pan
    /* 改变画笔属性 */
    pen.setWidth(ImageLineWidth);//  线宽度
    pen.setColor(CurveParameterForPanCore.PanColor);//画笔颜色
    painter.setPen(pen);

    pointLeft.setX(lx+LegendHOffset);
    pointLeft.setY(ly+LegendVOffset*0.5);
    pointRight.setX(lx+LegendHOffset+LegendLineLength);
    pointRight.setY(ly+LegendVOffset*0.5);

    painter.drawLine(pointLeft,pointRight);

    // 图例后面的文字
    /* 改变画笔属性 */
    pen.setWidth(LegendTitleTextWidth);//  线宽度
    pen.setColor(CurveParameterForPanCore.PanColor);//画笔颜色
    painter.setPen(pen);

    pointLeft.setX(lx+LegendHOffset+LegendLineLength+LegendHOffset*2);
    pointLeft.setY(ly);
    pointRight.setX(pointLeft.x()+LegendTextWidth);
    pointRight.setY(pointLeft.y()+LegendVOffset);

    TextRect.setTopLeft(pointLeft);
    TextRect.setBottomRight(pointRight);

    font.setPointSize(LegendFontSize);
    //font.setPointSize(CurveParameterForPanCore.PanCoreXTitleSize);
    font.setFamily("Times");
    painter.setFont(font);

    painter.drawText(TextRect,Qt::AlignVCenter,"Pan-Genome Size");

    if(CurveParameterForPanCore.toShowBoxplot ==false)
    {
        // 图例上的点
        /* 改变画笔属性 */
        pointMid.setX(lx+LegendHOffset+LegendLineLength*0.5);
        pointMid.setY(ly+LegendVOffset*0.5);
        pen.setWidth(ImageDotWidth);//  线宽度
        pen.setColor(CurveParameterForPanCore.PanColor);//画笔颜色
        painter.setPen(pen);
        painter.drawPoint(pointMid);
    }

//core

    /* 改变画笔属性 */
    pen.setWidth(ImageLineWidth);//  线宽度
    pen.setColor(CurveParameterForPanCore.CoreColor);//画笔颜色
    painter.setPen(pen);

    pointLeft.setX(lx+LegendHOffset);
    pointLeft.setY(ly+LegendVOffset*1.5);
    pointRight.setX(lx+LegendHOffset+LegendLineLength);
    pointRight.setY(ly+LegendVOffset*1.5);

    painter.drawLine(pointLeft,pointRight);

    // 图例后面的文字
    /* 改变画笔属性 */
    pen.setWidth(LegendTitleTextWidth);//  线宽度
    pen.setColor(CurveParameterForPanCore.CoreColor);//画笔颜色
    painter.setPen(pen);

    pointLeft.setX(lx+LegendHOffset+LegendLineLength+LegendHOffset*2);
    pointLeft.setY(ly+LegendVOffset);
    pointRight.setX(pointLeft.x()+LegendTextWidth);
    pointRight.setY(pointLeft.y()+LegendVOffset);

    TextRect.setTopLeft(pointLeft);
    TextRect.setBottomRight(pointRight);

    painter.drawText(TextRect,Qt::AlignVCenter,"Core Genome Size");

    if(CurveParameterForPanCore.toShowBoxplot ==false)
    {
        // 图例上的点
        /* 改变画笔属性 */
        pointMid.setX(lx+LegendHOffset+LegendLineLength*0.5);
        pointMid.setY(ly+LegendVOffset*1.5);
        pen.setWidth(ImageDotWidth);//  线宽度
        pen.setCapStyle(Qt::RoundCap);
        pen.setColor(CurveParameterForPanCore.CoreColor);//画笔颜色
        painter.setPen(pen);
        painter.drawPoint(pointMid);
    }

}

// 画泛基因组的箱线图
void DrawPanCoreCurve::CalculateAndPaintPanBoxplot()
{
/* 设置画笔信息   开始*/
    QPainter painter(&PanCurveImage);
    painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
    QPen pen; //笔
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setWidth(ImageLineWidth);//  线宽度
    pen.setColor(CurveParameterForPanCore.PanColor);//画笔颜色
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);//刷子设为空
/* 设置画笔信息   结束*/

    for(qint32 i=0;i<panProfile.GenomeNumber;i++)
    {
        qreal xCoord=FrameXStartCoordinate + (i+1) * XScale;
        qreal ymaxCoord =FrameYStartCoordinate - (panProfile.PanValueQ3[i] + 1.5 * panProfile.PanValueIQR[i] - YMarkStartValue)*YScale;
        qreal yq1Coord  =FrameYStartCoordinate - (panProfile.PanValueQ1[i] - YMarkStartValue)*YScale;
        qreal yq2Coord  =FrameYStartCoordinate - (panProfile.PanValueQ2[i] - YMarkStartValue)*YScale;
        qreal yq3Coord  =FrameYStartCoordinate - (panProfile.PanValueQ3[i] - YMarkStartValue)*YScale;
        qreal yminCoord =FrameYStartCoordinate - (panProfile.PanValueQ1[i] - 1.5 * panProfile.PanValueIQR[i] - YMarkStartValue)*YScale;

        // 画上边缘横线
        painter.drawLine(xCoord -BoxplotHalfWidth,ymaxCoord,xCoord+BoxplotHalfWidth,ymaxCoord);

        // 画下边缘横线
        painter.drawLine(xCoord -BoxplotHalfWidth,yminCoord,xCoord+BoxplotHalfWidth,yminCoord);

        // 画上边缘纵线
        painter.drawLine(xCoord,ymaxCoord,xCoord,yq3Coord);

        // 画下边缘纵线
        painter.drawLine(xCoord,yminCoord,xCoord,yq1Coord);

        // 画四分位矩形
        QPoint p1,p2;
        QRect rect;
        p1.setX(xCoord-BoxplotHalfWidth);
        p1.setY(yq3Coord);
        p2.setX(xCoord+BoxplotHalfWidth);
        p2.setY(yq1Coord);
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        painter.drawRect(rect);

        // 画中位数线
        pen.setWidth(ImageLineWidth);//  线宽度
        painter.drawLine(xCoord -BoxplotHalfWidth,yq2Coord,xCoord+BoxplotHalfWidth,yq2Coord);
    }
}


// 画核心基因的箱线图
void DrawPanCoreCurve::CalculateAndPaintCoreBoxplot()
{
    /* 设置画笔信息   开始*/
        QPainter painter(&PanCurveImage);
        painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
        QPen pen; //笔
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setWidth(ImageLineWidth);//  线宽度
        pen.setColor(CurveParameterForPanCore.CoreColor);//画笔颜色
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);//刷子设为空
    /* 设置画笔信息   结束*/

        for(qint32 i=0;i<panProfile.GenomeNumber;i++)
        {
            qreal xCoord=FrameXStartCoordinate + (i+1) * XScale;
            qreal ymaxCoord =FrameYStartCoordinate - (panProfile.CoreValueQ3[i] + 1.5 * panProfile.CoreValueIQR[i] - YMarkStartValue)*YScale;
            qreal yq1Coord  =FrameYStartCoordinate - (panProfile.CoreValueQ1[i] - YMarkStartValue)*YScale;
            qreal yq2Coord  =FrameYStartCoordinate - (panProfile.CoreValueQ2[i] - YMarkStartValue)*YScale;
            qreal yq3Coord  =FrameYStartCoordinate - (panProfile.CoreValueQ3[i] - YMarkStartValue)*YScale;
            qreal yminCoord =FrameYStartCoordinate - (panProfile.CoreValueQ1[i] - 1.5 * panProfile.CoreValueIQR[i] - YMarkStartValue)*YScale;

            // 画上边缘横线
            painter.drawLine(xCoord -BoxplotHalfWidth,ymaxCoord,xCoord+BoxplotHalfWidth,ymaxCoord);

            // 画下边缘横线
            painter.drawLine(xCoord -BoxplotHalfWidth,yminCoord,xCoord+BoxplotHalfWidth,yminCoord);

            // 画上边缘纵线
            painter.drawLine(xCoord,ymaxCoord,xCoord,yq3Coord);

            // 画下边缘纵线
            painter.drawLine(xCoord,yminCoord,xCoord,yq1Coord);

            // 画四分位矩形
            QPoint p1,p2;
            QRect rect;
            p1.setX(xCoord-BoxplotHalfWidth);
            p1.setY(yq3Coord);
            p2.setX(xCoord+BoxplotHalfWidth);
            p2.setY(yq1Coord);
            rect.setTopLeft(p1);
            rect.setBottomRight(p2);
            painter.drawRect(rect);

            // 画中位数线
            pen.setWidth(ImageLineWidth);//  线宽度
            painter.drawLine(xCoord -BoxplotHalfWidth,yq2Coord,xCoord+BoxplotHalfWidth,yq2Coord);
        }
}



void DrawPanCoreCurve::zztest()
{
    /* 设置画笔信息   开始*/
        QPainter painter(&PanCurveImage);
        painter.setRenderHint(QPainter::Antialiasing,true);//平滑直线
        QPen pen; //笔
        pen.setWidth(ImageLineWidth);//  线宽度
        pen.setColor(Qt::black);//画笔颜色
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);//刷子设为空
    /* 设置画笔信息   结束*/
        QPoint p1,p2;
        p1.setX(10);
        p1.setY(10);
        p2.setX(200);
        p2.setY(200);
        painter.drawLine(p1,p2);

}









