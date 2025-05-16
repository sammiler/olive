#ifndef KD_SEGMENTED_INDICATORS_P_H
#define KD_SEGMENTED_INDICATORS_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当使用 Qt Widgets 时编译以下部分

#include "../DropIndicatorOverlayInterface_p.h" // 包含拖放指示器覆盖界面的私有头文件

#include <QHash>   // 包含 QHash 类，用于哈希表
#include <QPolygon> // 包含 QPolygon 类，用于表示多边形

namespace KDDockWidgets {

/**
 * @brief SegmentedIndicators 类提供一种分段式的拖放指示器。
 *
 * 这种指示器通常表现为围绕目标停靠区域边缘的多个高亮区域（段），
 * 用户可以将窗口拖动到这些段上以指示停靠位置。
 * 它会根据鼠标位置高亮显示相应的段，并绘制这些段。
 */
class DOCKS_EXPORT SegmentedIndicators : public DropIndicatorOverlayInterface
{
    Q_OBJECT
public:
    /**
     * @brief 构造一个 SegmentedIndicators 对象。
     * @param dropArea 此指示器覆盖层所属的拖放区域。
     */
    explicit SegmentedIndicators(DropArea *dropArea);

    /**
     * @brief 销毁 SegmentedIndicators 对象。
     */
    ~SegmentedIndicators() override;

    /**
     * @brief 实现悬停行为，确定鼠标下方的停靠段。
     * @param pt 全局鼠标坐标点。
     * @return 根据鼠标位置确定的 DropLocation (停靠位置)。
     */
    DropLocation hover_impl(QPoint pt) override;

    /**
     * @brief 根据给定的位置确定其对应的 DropLocation。
     *
     * 遍历所有当前可见的指示器段，检查给定点是否在某个段内。
     *
     * @param pos 要检查的坐标点。
     * @return 如果点在某个段内，则返回该段对应的 DropLocation；否则返回 NoDrop。
     */
    [[nodiscard]] DropLocation dropLocationForPos(QPoint pos) const;


    static int s_segmentGirth;                    ///< 静态成员，定义指示器段的厚度（宽度或高度）。
    static int s_segmentPenWidth;                 ///< 静态成员，定义绘制指示器段边框的画笔宽度。
    static int s_centralIndicatorMaxWidth;        ///< 静态成员，定义中心指示器的最大宽度。
    static int s_centralIndicatorMaxHeight;       ///< 静态成员，定义中心指示器的最大高度。
    static qreal s_draggedWindowOpacity;          ///< 静态成员，定义拖动窗口时的不透明度。
    static QColor s_segmentPenColor;              ///< 静态成员，定义指示器段边框的颜色。
    static QColor s_segmentBrushColor;            ///< 静态成员，定义指示器段填充的默认颜色。
    static QColor s_hoveredSegmentBrushColor;     ///< 静态成员，定义鼠标悬停时指示器段填充的颜色。

protected:
    /**
     * @brief 重写 QWidget::paintEvent()，用于绘制所有指示器段。
     * @param event 绘制事件对象。
     */
    void paintEvent(QPaintEvent *) override;

    /**
     * @brief 获取指定停靠位置的指示器的理想位置。
     *
     * 对于分段指示器，这通常是对应段的某个参考点，或者对于中心指示器是其中心点。
     *
     * @param DropLocation 未使用的停靠位置（在此实现中可能不直接使用特定位置的单个指示器）。
     * @return 返回一个 QPoint，通常为空，因为分段指示器是绘制在整个覆盖层上的。
     */
    [[nodiscard]] QPoint posForIndicator(DropLocation) const override;

private:
    /**
     * @brief 为给定的矩形计算所有指示器段的多边形表示。
     * @param rect 用于计算段的基础矩形。
     * @param inner 指示是计算内部段还是外部段。
     * @param useOffset 是否在计算时使用偏移量（例如，用于绘制拖动时的预览）。
     * @return 一个 QHash，将 DropLocation 映射到其对应的 QPolygon。
     */
    [[nodiscard]] QHash<DropLocation, QPolygon> segmentsForRect(QRect, bool inner, bool useOffset = false) const;

    /**
     * @brief 更新所有指示器段的几何形状。
     *
     * 通常在关联的拖放区域大小改变或状态改变时调用。
     */
    void updateSegments();

    /**
     * @brief 使用给定的 QPainter 绘制所有可见的指示器段。
     * @param p 用于绘制的 QPainter 对象。
     */
    void drawSegments(QPainter *p);

    /**
     * @brief 使用给定的 QPainter 绘制单个指示器段。
     * @param p 用于绘制的 QPainter 对象。
     * @param segment 要绘制的段的多边形表示。
     */
    void drawSegment(QPainter *p, const QPolygon &segment);

    QPoint m_hoveredPt = {};                            ///< 当前鼠标悬停的坐标点。
    QHash<DropLocation, QPolygon> m_segments;           ///< 存储所有指示器段的多边形表示，键为 DropLocation。
};

} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QTWIDGETS


#endif // KD_SEGMENTED_INDICATORS_P_H