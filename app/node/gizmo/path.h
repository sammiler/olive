

#ifndef PATHGIZMO_H
#define PATHGIZMO_H

#include <QPainterPath>

#include "draggable.h"

namespace olive {

class PathGizmo : public DraggableGizmo {
  Q_OBJECT
 public:
  explicit PathGizmo(QObject *parent = nullptr);

  [[nodiscard]] const QPainterPath &GetPath() const { return path_; }
  void SetPath(const QPainterPath &path) { path_ = path; }

  void Draw(QPainter *p) const override;

 private:
  QPainterPath path_;
};

}  // namespace olive

#endif  // PATHGIZMO_H
