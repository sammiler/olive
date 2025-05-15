#include "nodeviewitemconnector.h"

#include <QApplication>
#include <QFontMetrics>
#include <QPalette>
#include <QPen>

#include "nodeviewitem.h"

namespace olive {

NodeViewItemConnector::NodeViewItemConnector(bool is_output, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent), output_(is_output) {
  QColor c = qApp->palette().text().color();
  setPen(QPen(c, NodeViewItem::DefaultItemBorder()));
  setBrush(c);
}

void NodeViewItemConnector::SetFlowDirection(NodeViewCommon::FlowDirection dir) {
  QFont f;
  QFontMetricsF fm(f);

  int triangle_sz = fm.height() / 2;
  int triangle_sz_half = triangle_sz / 2;

  QPolygonF p;
  p.resize(3);

  switch (dir) {
    case NodeViewCommon::kLeftToRight:
      // Triangle pointing right
      p[0] = QPointF(0, -triangle_sz_half);
      p[1] = QPointF(triangle_sz_half, 0);
      p[2] = QPointF(0, triangle_sz_half);
      break;
    case NodeViewCommon::kTopToBottom:
      // Triangle pointing down
      p[0] = QPointF(-triangle_sz_half, 0);
      p[1] = QPointF(0, triangle_sz_half);
      p[2] = QPointF(triangle_sz_half, 0);
      break;
    case NodeViewCommon::kBottomToTop:
      // Triangle pointing up
      p[0] = QPointF(-triangle_sz_half, 0);
      p[1] = QPointF(0, -triangle_sz_half);
      p[2] = QPointF(triangle_sz_half, 0);
      break;
    case NodeViewCommon::kRightToLeft:
      // Triangle pointing left
      p[0] = QPointF(0, -triangle_sz_half);
      p[1] = QPointF(-triangle_sz_half, 0);
      p[2] = QPointF(0, triangle_sz_half);
      break;
    case NodeViewCommon::kInvalidDirection:
      break;
  }

  setPolygon(p);
}

QPainterPath NodeViewItemConnector::shape() const {
  // Yes, we skip QGraphicsPolygonItem because it adds the polygon. QGraphicsItem adds the
  // boundingRect which we modify below
  return QGraphicsPolygonItem::shape();  // clazy:exclude=skipped-base-method
}

QRectF NodeViewItemConnector::boundingRect() const {
  QRectF b = this->polygon().boundingRect();
  const int radius = QFontMetrics(QFont()).height() / 2;
  b.adjust(-radius, -radius, radius, radius);
  return b;
}

}  // namespace olive
