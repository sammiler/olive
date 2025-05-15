#include "point.h"

#include <QApplication>

namespace olive {

PointGizmo::PointGizmo(const Shape &shape, bool smaller, QObject *parent)
    : DraggableGizmo{parent}, shape_(shape), smaller_(smaller) {}

PointGizmo::PointGizmo(const Shape &shape, QObject *parent) : PointGizmo(shape, false, parent) {}

PointGizmo::PointGizmo(QObject *parent) : PointGizmo(kSquare, parent) {}

void PointGizmo::Draw(QPainter *p) const {
  QRectF rect = GetDrawingRect(p->transform(), GetStandardRadius());

  if (shape_ != kAnchorPoint) {
    p->setPen(QPen(Qt::black, 0));
    p->setBrush(Qt::white);
  }

  switch (shape_) {
    case kSquare:
      p->drawRect(rect);
      break;
    case kCircle:
      p->drawEllipse(rect);
      break;
    case kAnchorPoint:
      p->setPen(QPen(Qt::white, 0));
      p->setBrush(Qt::NoBrush);

      p->drawEllipse(rect);
      p->drawLines({QPointF(rect.left(), rect.center().y()), QPointF(rect.right(), rect.center().y()),
                    QPointF(rect.center().x(), rect.top()), QPointF(rect.center().x(), rect.bottom())});
      break;
  }
}

QRectF PointGizmo::GetClickingRect(const QTransform &t) const { return GetDrawingRect(t, GetStandardRadius()); }

double PointGizmo::GetStandardRadius() { return QFontMetrics(qApp->font()).height() * 0.25; }

QRectF PointGizmo::GetDrawingRect(const QTransform &transform, double radius) const {
  QRectF r(0, 0, radius, radius);

  r = transform.inverted().mapRect(r);

  double width = r.width();
  double height = r.height();

  if (shape_ == kAnchorPoint) {
    width *= 2;
    height *= 2;
  }

  if (smaller_) {
    width *= 0.5;
    height *= 0.5;
  }

  return {point_.x() - width, point_.y() - height, 2 * width, 2 * height};
}

}  // namespace olive
