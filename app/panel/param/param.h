

#ifndef PARAM_H
#define PARAM_H

#include "panel/curve/curve.h"
#include "panel/timebased/timebased.h"
#include "widget/nodeparamview/nodeparamview.h"

namespace olive {

class ParamPanel : public TimeBasedPanel {
  Q_OBJECT
 public:
  ParamPanel();

  [[nodiscard]] NodeParamView *GetParamView() const { return dynamic_cast<NodeParamView *>(GetTimeBasedWidget()); }

  [[nodiscard]] const QVector<Node *> &GetContexts() const { return GetParamView()->GetContexts(); }

  void CloseContextsBelongingToProject(Project *p) const { GetParamView()->CloseContextsBelongingToProject(p); }

 public slots:
  void SetSelectedNodes(const QVector<Node::ContextPair> &nodes) const {
    GetParamView()->SetSelectedNodes(nodes, false);
  }

  void DeleteSelected() override;

  void SelectAll() override;

  void DeselectAll() override;

  void SetContexts(const QVector<Node *> &contexts);

 signals:
  void FocusedNodeChanged(Node *n);

  void SelectedNodesChanged(const QVector<Node::ContextPair> &nodes);

  void RequestViewerToStartEditingText();

 protected:
  void Retranslate() override;
};

}  // namespace olive

#endif  // PARAM_H
