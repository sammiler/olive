#include "polygon.h"

namespace olive {

PolygonGizmo::PolygonGizmo(QObject *parent) : DraggableGizmo{parent} {}

void PolygonGizmo::Draw(QPainter *p) const {
  // Draw transposed black
  QPolygonF transposed = p->transform().map(polygon_);
  transposed.translate(1, 1);
  transposed = p->transform().inverted().map(transposed);
  p->setPen(QPen(Qt::black, 0));
  p->drawPolyline(transposed);

  // Draw normal white polygon
  p->setPen(QPen(Qt::white, 0));
  p->setBrush(Qt::NoBrush);
  p->drawPolyline(polygon_);
}

}  // namespace olive
