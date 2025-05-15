

#include "line.h"

namespace olive {

LineGizmo::LineGizmo(QObject *parent) : NodeGizmo(parent) {}

void LineGizmo::Draw(QPainter *p) const {
  // Draw transposed black
  QLineF transposed = p->transform().map(line_);
  transposed.translate(1, 1);
  transposed = p->transform().inverted().map(transposed);
  p->setPen(QPen(Qt::black, 0));
  p->drawLine(transposed);

  // Draw normal white polygon
  p->setPen(QPen(Qt::white, 0));
  p->setBrush(Qt::NoBrush);
  p->drawLine(line_);
}

}  // namespace olive
