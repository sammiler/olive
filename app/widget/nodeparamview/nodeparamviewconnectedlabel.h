#ifndef NODEPARAMVIEWCONNECTEDLABEL_H
#define NODEPARAMVIEWCONNECTEDLABEL_H

#include "node/param.h"
#include "widget/clickablelabel/clickablelabel.h"
#include "widget/nodevaluetree/nodevaluetree.h"

namespace olive {

class NodeParamViewConnectedLabel : public QWidget {
  Q_OBJECT
 public:
  explicit NodeParamViewConnectedLabel(NodeInput input, QWidget *parent = nullptr);

  void SetViewerNode(ViewerOutput *viewer);

 signals:
  void RequestSelectNode(Node *n);

 private slots:
  void InputConnected(Node *output, const NodeInput &input);

  void InputDisconnected(Node *output, const NodeInput &input);

  void ShowLabelContextMenu();

  void ConnectionClicked();

 private:
  void UpdateLabel();

  void UpdateValueTree();

  void CreateTree();

  ClickableLabel *connected_to_lbl_;

  NodeInput input_;

  Node *connected_node_;

  NodeValueTree *value_tree_;

  ViewerOutput *viewer_;

 private slots:
  void SetValueTreeVisible(bool e);
};

}  // namespace olive

#endif  // NODEPARAMVIEWCONNECTEDLABEL_H
