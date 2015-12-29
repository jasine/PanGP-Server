#ifndef CURVEPARAMETERFORNEWGENEDATA_H
#define CURVEPARAMETERFORNEWGENEDATA_H

#include <QString>
#include <QColor>
#include <QFont>
#include <QVector>
#include <QHash>

class CurveParameterForNewGeneData
{
public:
    CurveParameterForNewGeneData();

    QVector <QColor> AllColor; // 所有颜色集合

 //New Gene（新基因的图像与窗口参数）
    qint32 NewGeneWinWidth; //窗口大小
    qint32 NewGeneWinHeight;

    bool IsNewGeneWindowOpen; // 新基因的窗口是否打开

    qint32 ImageWidth;  //图片宽度
    qint32 ImageHeight; //图片高度

    qint32 NewGeneImageWidth; // 图片宽度
    qint32 NewGeneImageHeight; // 图片高度

    QString NewGeneXTitle; // 标题
    QString NewGeneYTitle;

    qreal NewGeneXTitleSize;// 字号
    qreal NewGeneYTitleSize;

    QFont NewGeneXTitleFont; // 字体
    QFont NewGeneYTitleFont;

    qreal NewGeneXScaleValueSize; // 刻度大小
    qreal NewGeneYScaleValueSize;

    QFont NewGeneXScaleValueFont; // 刻度字号
    QFont NewGeneYScaleValueFont;

    //QColor NewGeneColor; // 新基因的颜色
    //qint32 NewGeneColorID; // 新基因的颜色ID

    QColor BarColor; // 直方图颜色
    qint32 BarColorID; // 直方图颜色ID；

    QColor TrendLineColor;  // 趋势线颜色
    qint32 TrendLineColorID; // 趋势线颜色ID

    bool toDrawNewGeneLegend; // 是否需要画新基因的图例
    qreal NewGeneLegendTopLeftX;// 图里的左上角位置
    qreal NewGeneLegendTopLeftY;

    //bool toShowBoxplot; // 默认显示的是boxplot
    bool toShowTrendLine; // 默认不显示趋势线

    qint32 ImageDPI; // 图像的DPI

};

#endif // CURVEPARAMETERFORNEWGENEDATA_H
