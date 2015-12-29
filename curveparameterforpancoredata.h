#ifndef CURVEPARAMETERFORPANCOREDATA_H
#define CURVEPARAMETERFORPANCOREDATA_H

#include <QString>
#include <QColor>
#include <QFont>
#include <QVector>
#include <QHash>

class CurveParameterForPanCoreData
{
public:
    CurveParameterForPanCoreData();

    QVector <QColor> AllColor; // 所有颜色集合

//Pan-Core (泛基因组以及核心基因的参数)
    qint32 PanWinWidth; //窗口宽度
    qint32 PanWinHeight;    //窗口高度

    bool IsPanWindowOpen; //窗体是否已经打开

    qint32 ImageWidth;  //图片宽度
    qint32 ImageHeight; //图片高度

    QString PanCoreXTitle;//X轴标题 泛基因组与核心基因组
    QString PanCoreYTitle;//Y轴标题 泛基因组与核心基因组

    qreal PanCoreXTitleSize; // X轴标题字体大小
    qreal PanCoreYTitleSize; // Y轴标题字体大小

    QFont PanCoreXTitleFont; // X轴标题字体
    QFont PanCoreYTitleFont; // Y轴标题字体

    qreal PanCoreXScaleValueSize; // X-刻度值大小
    qreal PanCoreYScaleValueSize; // Y-刻度值大小

    QFont PanCoreXScaleValueFont; // X刻度的字体
    QFont PanCoreYScaleValueFont; // Y刻度值字体

    QColor PanColor; // 泛基因组颜色
    QColor CoreColor;// 核心基因颜色
    qint32 PanColorID; // 泛基因组颜色ID
    qint32 CoreColorID; // 核心基因组颜色ID

    bool toDrawPanLegend;// 是否要画泛基因组的图例
    qreal LegendTopLeftX;//左上角X坐标
    qreal LegendTopLeftY;//左上角Y坐标

    qreal LegendTopLeftXCapture; // 拖动图例时捕获的鼠标坐标位置
    qreal LegendTopLeftYCapture;

    bool toShowBoxplot; // 默认显示的是boxplot
    qreal ImageDPI; // Dot Per Inch

};

#endif // CURVEPARAMETERFORPANCOREDATA_H
