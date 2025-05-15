#include "param.h"

#include "window/mainwindow/mainwindow.h"

namespace olive {

ParamPanel::ParamPanel() : TimeBasedPanel(QStringLiteral("ParamPanel")) {
  auto *view = new NodeParamView(this);
  connect(view, &NodeParamView::FocusedNodeChanged, this, &ParamPanel::FocusedNodeChanged);
  connect(view, &NodeParamView::SelectedNodesChanged, this, &ParamPanel::SelectedNodesChanged);
  connect(view, &NodeParamView::RequestViewerToStartEditingText, this, &ParamPanel::RequestViewerToStartEditingText);
  connect(this, &ParamPanel::shown, view, &NodeParamView::UpdateElementY);
  SetTimeBasedWidget(view);

  Retranslate();
}

void ParamPanel::DeleteSelected() { dynamic_cast<NodeParamView *>(GetTimeBasedWidget())->DeleteSelected(); }

void ParamPanel::SelectAll() { dynamic_cast<NodeParamView *>(GetTimeBasedWidget())->SelectAll(); }

void ParamPanel::DeselectAll() { dynamic_cast<NodeParamView *>(GetTimeBasedWidget())->DeselectAll(); }

void ParamPanel::SetContexts(const QVector<Node *> &contexts) {
  dynamic_cast<NodeParamView *>(GetTimeBasedWidget())->SetContexts(contexts);
}

void ParamPanel::Retranslate() { SetTitle(tr("Parameter Editor")); }

}  // namespace olive
