

#include "path.h"

namespace olive {

PathGizmo::PathGizmo(QObject *parent) : DraggableGizmo{parent} {}

void PathGizmo::Draw(QPainter *p) const {
  // Draw transposed black
  QPainterPath transposed = p->transform().map(path_);
  transposed.translate(1, 1);
  transposed = p->transform().inverted().map(transposed);
  p->setPen(QPen(Qt::black, 0));
  p->drawPath(transposed);

  // Draw normal white polygon
  p->setPen(QPen(Qt::white, 0));
  p->setBrush(Qt::NoBrush);
  p->drawPath(path_);
}

}  // namespace olive
