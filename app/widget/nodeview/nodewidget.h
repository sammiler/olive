

#ifndef NODEWIDGET_H
#define NODEWIDGET_H

#include <QWidget>

#include "nodeview.h"
#include "nodeviewtoolbar.h"

namespace olive {

class NodeWidget : public QWidget {
  Q_OBJECT
 public:
  explicit NodeWidget(QWidget *parent = nullptr);

  [[nodiscard]] NodeView *view() const { return node_view_; }

  void SetContexts(const QVector<Node *> &nodes) {
    node_view_->SetContexts(nodes);
    toolbar_->setEnabled(!nodes.isEmpty());
  }

 private:
  NodeView *node_view_;

  NodeViewToolBar *toolbar_;
};

}  // namespace olive

#endif  // NODEWIDGET_H
