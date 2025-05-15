

#ifndef POINTGIZMO_H
#define POINTGIZMO_H

#include <QPointF>

#include "draggable.h"

namespace olive {

class PointGizmo : public DraggableGizmo {
  Q_OBJECT
 public:
  enum Shape { kSquare, kCircle, kAnchorPoint };

  explicit PointGizmo(const Shape &shape, bool smaller, QObject *parent = nullptr);
  explicit PointGizmo(const Shape &shape, QObject *parent = nullptr);
  explicit PointGizmo(QObject *parent = nullptr);

  [[nodiscard]] const Shape &GetShape() const { return shape_; }
  void SetShape(const Shape &s) { shape_ = s; }

  [[nodiscard]] const QPointF &GetPoint() const { return point_; }
  void SetPoint(const QPointF &pt) { point_ = pt; }

  [[nodiscard]] bool GetSmaller() const { return smaller_; }
  void SetSmaller(bool e) { smaller_ = e; }

  void Draw(QPainter *p) const override;

  [[nodiscard]] QRectF GetClickingRect(const QTransform &t) const;

 private:
  static double GetStandardRadius();

  [[nodiscard]] QRectF GetDrawingRect(const QTransform &transform, double radius) const;

  Shape shape_;

  QPointF point_;

  bool smaller_;
};

}  // namespace olive

#endif  // POINTGIZMO_H
