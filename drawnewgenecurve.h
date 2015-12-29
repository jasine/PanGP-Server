#ifndef DRAWNEWGENECURVE_H
#define DRAWNEWGENECURVE_H

#include <QThread>
#include <QPoint>
#include <QPainter>
#include <QDebug>
#include <panprofiledata.h>
#include <curveparameterfornewgenedata.h>
#include <QColor>
#include <qmath.h>
#include <QFile>
#include <QTextItem>
#include <fittingresultdata.h>


class DrawNewGeneCurve : public QThread
{
    Q_OBJECT
public:
    explicit DrawNewGeneCurve(PanProfileData PanProfileRef,
                              CurveParameterForNewGeneData CurveParameterForNewGeneRef,
                              FittingResultData FittingResultRef,
                              QObject *parent = 0);
    ~DrawNewGeneCurve();
    QImage NewGeneCurveImage; // 新基因的图像数据

    CurveParameterForNewGeneData CurveParameterForNewGene; // 图像参数
    PanProfileData PanProfile; //泛基因组数据
    FittingResultData FittingResult; // 新基因拟合结果


signals:
    void sigdrawNewGeneCurveThreadProcessMsg(QString message); //发出运行日志
    void sigdrawNewGeneCurveFinish(QImage PCI);//发出运行完成信号
    void sigdrawNewGeneCurveFinishRePaint(QImage PCI); //发出重绘图完成信号



public slots:
    void slotKillMyself();// 关闭自己

private:
//函数声明
    void run();

//画图函数
    void paintOutFrame();     //画外边框
    void paintXAxisTitle();   //画X轴标题
    void paintYAxisTitle();   //画Y轴标题
    //void paintLegend();       //画图例

//计算函数
    void CalculateXScaleNum(); //计算X轴刻度数目以及比例
    void CalculateYScaleNum(); //计算Y轴刻度数目以及比例

// 计算之后画图

    void CalculateAndPaintXScaleMark(); //计算并画X轴刻度线
    void CalculateAndPaintYScaleMark(); //计算并画Y轴刻度线
    void CalculateAndPaintXScaleDigit();//计算并画X轴刻度数字
    void CalculateAndPaintYScaleDigit();//计算并画Y轴刻度数字
    void CalculateAndPaintNewGeneBar();    //新基因的直方图
    void CalculateAndPaintNewGeneTrendLine();    //新基因的趋势曲线


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

    bool IsXMarkExtend; // x轴刻度是否拓展
    QList <qint32> XMarkList; // X 轴刻度位置
    qint32 XMarkNumber;//X轴刻度数
    qint32 XMarkStep;//X轴刻度步长
    qreal XMarkStartValue;//X轴起始值
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
    static const qreal LegendWidth=140;
    static const qreal LegendHeight=25;
    static const qreal LegendLineLength=20;
    static const qreal LegendHOffset=5; // 水平间距
    static const qreal LegendVOffset=25;
    static const qreal LegendTextWidth=120;

    //boxplot 宽度
    static const qreal BarWidthRatio=0.25; // 直方bar图的半宽度与一个单位刻度比率

//主框架坐标（相对于图层）  ---结束










    
};

#endif // DRAWNEWGENECURVE_H
