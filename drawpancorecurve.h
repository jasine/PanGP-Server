#ifndef DRAWPANCORECURVE_H
#define DRAWPANCORECURVE_H

#include <QThread>
#include <QPoint>
#include <QPainter>
#include <QDebug>
#include <panprofiledata.h>
#include <curveparameterforpancoredata.h>
#include <QColor>
#include <qmath.h>
#include <QFile>
#include <QTextItem>

class DrawPanCoreCurve : public QThread
{
    Q_OBJECT
public:
    explicit DrawPanCoreCurve(PanProfileData PanProfileRef,CurveParameterForPanCoreData CurveParameterForPanCoreRef,
                              QObject *parent = 0);
    ~DrawPanCoreCurve();

    QPainter PanCurvePainter;  //定义画笔
    QImage PanCurveImage; // 图片将画在这个上面

signals:
    void sigdrawCurveThreadProcessMsg(QString message); //发出运行日志
    void sigdrawCurveFinish(QImage PCI);//发出运行完成信号
    void sigdrawCurveFinishRePaint(QImage PCI); //发出重绘图完成信号
    //void sigdrawCurveFinish();//发出运行完成信号

public slots:

    void slotKillMyself();

private:
//函数声明
    void run();

//画图函数
    void paintOutFrame();     //画外边框
    void paintXAxisTitle();   //画X轴标题
    void paintYAxisTitle();   //画Y轴标题
    void paintLegend();       //画图例

//计算函数
    void CalculateXScaleNum(); //计算X轴刻度数目以及比例
    void CalculateYScaleNum(); //计算Y轴刻度数目以及比例

// 计算之后画图

    void CalculateAndPaintXScaleMark(); //计算并画X轴刻度线
    void CalculateAndPaintYScaleMark(); //计算并画Y轴刻度线
    void CalculateAndPaintXScaleDigit();//计算并画X轴刻度数字
    void CalculateAndPaintYScaleDigit();//计算并画Y轴刻度数字
    void CalculateAndPaintPanPoint();    //泛基因组大小散点
    void CalculateAndPaintCorePoint();   //核心基因组大小散点
    void CalculateAndPaintPanLine();     //泛基因组曲线
    void CalculateAndPaintCoreLine();    //核心基因组曲线
    void CalculateAndPaintPanBoxplot();  //计算泛基因组的箱线图
    void CalculateAndPaintCoreBoxplot(); //计算核心基因组的箱线图

    void zztest();



    PanProfileData panProfile;//泛基因组数据
    CurveParameterForPanCoreData CurveParameterForPanCore;//泛基因组窗体以及图形参数

    //记录窗体是否已经打开，以确定绘图完成发出的信号
    bool IsPanWindowOpen;

/*
 *1)X坐标轴刻度说明：如果总基因组数目（n<=10），刻度数目等于1+n；若n>10,拆成6~12份，步长中尾数为0或者5的优先，否则去最大步长
 *2)Y坐标轴刻度说明：刻度数目为
 *
 *
 *
 */

//画图的参数
    qreal ImageDPI;
    qreal ImageDPM;
    qreal ImageScaleRatio;//坐标的缩放尺寸
    qreal ImageFontRatio;//字体的缩放尺寸
    qreal ImageLineRatio;//线条的缩放尺寸

    qreal ImageWidth;//图像的宽高
    qreal ImageHeight;

    qreal WinWidth;//窗口的宽高
    qreal WinHeight;


//线条、点、字pen的粗细参数
    qint32 FrameLineWidth;//主框架外框 1
    qint32 AxisMarkLineWidth;//刻度线 1
    qint32 AxisScaleTextWith;//刻度字 1
    qint32 AxisTitleTextWith;//坐标轴标题 1
    qint32 ImageLineWidth; //图像区线条 2
    qint32 ImageDotWidth; //图像区点的大小 6
    qint32 LegendFrameLineWidth;//图例外框线
    qint32 LegendTitleTextWidth;//图例字


//图层基调坐标（相对于窗体）

    static const qreal LayerMarginTopTemplete=50;
    static const qreal LayerMarginBottomTemplete=50;
    static const qreal LayerMarginLeftTemplete=50;
    static const qreal LayerMarginRightTemplete=50;

    qreal LayerMarginTop;
    qreal LayerMarginBottom;
    qreal LayerMarginLeft;
    qreal LayerMarginRight;

//主框架坐标（相对于图层）  ---开始
    //const qreal FrameMarginTop=0;
    static const qreal FrameMarginTopTemplete=0;
    static const qreal FrameMarginBottomTemplete=60; // 默认100
    static const qreal FrameMarginLeftTemplete=100; // 默认100
    static const qreal FrameMarginRightTemplete=0;

    qreal FrameMarginTop;
    qreal FrameMarginBottom;
    qreal FrameMarginLeft;
    qreal FrameMarginRight;

    qreal FrameWidth; // 主框架宽度
    qreal FrameHeight;// 主框架高度
    qreal FrameX; //主框架X坐标
    qreal FrameY; //主框架Y坐标

    qint32 XMarkNumber;//X轴刻度数
    qint32 XMarkStep;//X轴刻度步长
    qint32 YMarkNumber;//Y轴刻度数
    qint32 YMarkStep;//Y轴刻度步长
    qint32 YMarkStartValue;//Y轴刻度起始值

    qreal XScale; //X轴方向数值与坐标比例
    qreal YScale; //Y轴方向数值与坐标比例

    qreal FrameXStartCoordinate;//主框架上X原点 在图层上真实坐标（图中所有点的位置都需要根据这个来计算）
    qreal FrameYStartCoordinate;//主框架上Y原点 在图层上真实坐标（图中所有点的位置都需要根据这个来计算）


    qreal LayerXStart; // 主图层X轴起点
    qreal LayerYStart; // 主图层Y轴起点
    qreal LayerWidth;  // 主图层宽度
    qreal LayerHeight; // 主图层高度

    QColor PanColor;   //泛基因组
    QColor CoreColor;  //

//刻度与坐标轴
    static const qreal DMarkDigitTemplete=10.0; //刻度与数字之间间距 15.0
    static const qreal MarkLineLengthTemplete=5.0;//刻度线线长

    static const qreal XLabelWidthTemplete=60.0; //X轴刻度数字域宽 40
    static const qreal XLabelHeightTemplete=30.0;//X轴刻度数字域高 20

    static const qreal YLabelWidthTemplete=60.0; //Y轴刻度数字域宽 40
    static const qreal YLabelHeightTemplete=30.0;//Y轴刻度数字域高 20

    static const qreal XAxisTitlePaddingTemplete=50;// X轴标题的内边距
    static const qreal XAxisTitleHeightTemplete=40; // X轴标题的高度

    static const qreal YAxisTitlePaddingTemplete=50;// X轴标题的内边距
    static const qreal YAxisTitleWidthTemplete=30; // X轴标题的宽度


    qreal DMarkDigit; //刻度与数字之间间距
    qreal MarkLineLength;//刻度线线长

    qreal XLabelWidth; //X轴刻度数字域宽
    qreal XLabelHeight;//X轴刻度数字域高

    qreal YLabelWidth; //X轴刻度数字域宽
    qreal YLabelHeight;//X轴刻度数字域高

    qreal XAxisTitlePadding;// X轴标题的内边距
    qreal XAxisTitleHeight; // X轴标题的宽度

    qreal YAxisTitlePadding;// X轴标题的内边距
    qreal YAxisTitleWidth; // X轴标题的宽度


//legend 图例的参数

    static const qreal LegendWidthTemplete=180;
    static const qreal LegendHeightTemplete=50;
    static const qreal LegendLineLengthTemplete=20;
    static const qreal LegendHOffsetTemplete=5; // 水平间距
    static const qreal LegendVOffsetTemplete=25;
    static const qreal LegendTextWidthTemplete=150;


    qreal LegendWidth;
    qreal LegendHeight;
    qreal LegendLineLength;
    qreal LegendHOffset; // 水平间距
    qreal LegendVOffset;
    qreal LegendTextWidth;
    qint32 LegendFontSize;

//boxplot 宽度
    static const qreal BoxplotHalfWidthTemplete=3; // boxplot图的半宽度
    qreal BoxplotHalfWidth; // boxplot图的半宽度



//主框架坐标（相对于图层）  ---结束
    
};

#endif // DRAWPANCORECURVE_H
