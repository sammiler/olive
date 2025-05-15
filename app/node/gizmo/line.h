

#ifndef LINEGIZMO_H
#define LINEGIZMO_H

#include <QLineF>

#include "gizmo.h"

namespace olive {

class LineGizmo : public NodeGizmo {
  Q_OBJECT
 public:
  explicit LineGizmo(QObject *parent = nullptr);

  [[nodiscard]] const QLineF &GetLine() const { return line_; }
  void SetLine(const QLineF &line) { line_ = line; }

  void Draw(QPainter *p) const override;

 private:
  QLineF line_;
};

}  // namespace olive

#endif  // LINEGIZMO_H
