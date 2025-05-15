

#ifndef POLYGONGIZMO_H
#define POLYGONGIZMO_H

#include <QPolygonF>

#include "draggable.h"

namespace olive {

class PolygonGizmo : public DraggableGizmo {
  Q_OBJECT
 public:
  explicit PolygonGizmo(QObject *parent = nullptr);

  [[nodiscard]] const QPolygonF &GetPolygon() const { return polygon_; }
  void SetPolygon(const QPolygonF &polygon) { polygon_ = polygon; }

  void Draw(QPainter *p) const override;

 private:
  QPolygonF polygon_;
};

}  // namespace olive

#endif  // POLYGONGIZMO_H
