#ifndef KD_SEGMENTED_INDICATORS_P_H
#define KD_SEGMENTED_INDICATORS_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS

#include "../DropIndicatorOverlayInterface_p.h"

#include <QHash>
#include <QPolygon>

namespace KDDockWidgets {

class DOCKS_EXPORT SegmentedIndicators : public DropIndicatorOverlayInterface
{
    Q_OBJECT
public:
    explicit SegmentedIndicators(DropArea *dropArea);
    ~SegmentedIndicators() override;
    DropLocation hover_impl(QPoint pt) override;

    [[nodiscard]] DropLocation dropLocationForPos(QPoint pos) const;


    static int s_segmentGirth;
    static int s_segmentPenWidth;
    static int s_centralIndicatorMaxWidth;
    static int s_centralIndicatorMaxHeight;
    static qreal s_draggedWindowOpacity;
    static QColor s_segmentPenColor;
    static QColor s_segmentBrushColor;
    static QColor s_hoveredSegmentBrushColor;

protected:
    void paintEvent(QPaintEvent *) override;
    [[nodiscard]] QPoint posForIndicator(DropLocation) const override;

private:
    [[nodiscard]] QHash<DropLocation, QPolygon> segmentsForRect(QRect, bool inner, bool useOffset = false) const;
    void updateSegments();
    void drawSegments(QPainter *p);
    void drawSegment(QPainter *p, const QPolygon &segment);
    QPoint m_hoveredPt = {};
    QHash<DropLocation, QPolygon> m_segments;
};

}

#endif


#endif
