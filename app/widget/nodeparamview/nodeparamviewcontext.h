

#ifndef NODEPARAMVIEWCONTEXT_H
#define NODEPARAMVIEWCONTEXT_H

#include "nodeparamviewdockarea.h"
#include "nodeparamviewitem.h"
#include "nodeparamviewitembase.h"

namespace olive {

class NodeParamViewContext : public NodeParamViewItemBase {
  Q_OBJECT
 public:
  explicit NodeParamViewContext(QWidget *parent = nullptr);

  [[nodiscard]] NodeParamViewDockArea *GetDockArea() const { return dock_area_; }

  [[nodiscard]] const QVector<Node *> &GetContexts() const { return contexts_; }

  [[nodiscard]] const QVector<NodeParamViewItem *> &GetItems() const { return items_; }

  NodeParamViewItem *GetItem(Node *node, Node *ctx);

  void AddNode(NodeParamViewItem *item);

  void RemoveNode(Node *node, Node *ctx);

  void RemoveNodesWithContext(Node *ctx);

  void SetInputChecked(const NodeInput &input, bool e);

  void SetTimebase(const rational &timebase);

  void SetTimeTarget(ViewerOutput *n);

  void SetEffectType(Track::Type type);

 signals:
  void AboutToDeleteItem(NodeParamViewItem *item);

 public slots:
  void AddContext(Node *node) { contexts_.append(node); }

  void RemoveContext(Node *node) { contexts_.removeOne(node); }

 protected slots:
  void Retranslate() override;

 private:
  NodeParamViewDockArea *dock_area_;

  QVector<Node *> contexts_;

  QVector<NodeParamViewItem *> items_;

  Track::Type type_;

 private slots:
  void AddEffectButtonClicked();

  void AddEffectMenuItemTriggered(QAction *a);
};

}  // namespace olive

#endif  // NODEPARAMVIEWCONTEXT_H
