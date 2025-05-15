

#ifndef NODEGIZMO_H
#define NODEGIZMO_H

#include <QObject>
#include <QPainter>

#include "node/globals.h"

namespace olive {

class NodeGizmo : public QObject {
  Q_OBJECT
 public:
  explicit NodeGizmo(QObject *parent = nullptr);
  ~NodeGizmo() override;

  virtual void Draw(QPainter *p) const {}

  [[nodiscard]] const NodeGlobals &GetGlobals() const { return globals_; }
  void SetGlobals(const NodeGlobals &globals) { globals_ = globals; }

  [[nodiscard]] bool IsVisible() const { return visible_; }
  void SetVisible(bool e) { visible_ = e; }

 signals:

 private:
  NodeGlobals globals_;

  bool visible_;
};

}  // namespace olive

#endif  // NODEGIZMO_H
