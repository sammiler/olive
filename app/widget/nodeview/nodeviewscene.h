

#ifndef NODEVIEWSCENE_H
#define NODEVIEWSCENE_H

#include <QGraphicsScene>
#include <QTimer>

#include "node/project.h"
#include "nodeviewcontext.h"
#include "nodeviewedge.h"
#include "nodeviewitem.h"
#include "undo/undostack.h"

namespace olive {

class NodeViewScene : public QGraphicsScene {
  Q_OBJECT
 public:
  explicit NodeViewScene(QObject *parent = nullptr);

  void SelectAll();
  void DeselectAll();

  [[nodiscard]] QVector<NodeViewItem *> GetSelectedItems() const;

  [[nodiscard]] const QHash<Node *, NodeViewContext *> &context_map() const { return context_map_; }

  [[nodiscard]] Qt::Orientation GetFlowOrientation() const;

  [[nodiscard]] NodeViewCommon::FlowDirection GetFlowDirection() const { return direction_; }

  void SetFlowDirection(NodeViewCommon::FlowDirection direction);

  [[nodiscard]] bool GetEdgesAreCurved() const { return curved_edges_; }

 public slots:
  NodeViewContext *AddContext(Node *node);
  void RemoveContext(Node *node);

  /**
   * @brief Set whether edges in this scene should be curved or not
   */
  void SetEdgesAreCurved(bool curved);

 private:
  QHash<Node *, NodeViewContext *> context_map_;

  Project *graph_{};

  NodeViewCommon::FlowDirection direction_;

  bool curved_edges_;
};

}  // namespace olive

#endif  // NODEVIEWSCENE_H
