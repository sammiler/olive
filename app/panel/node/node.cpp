

#include "node.h"

namespace olive {

NodePanel::NodePanel() : PanelWidget(QStringLiteral("NodePanel")) {
  node_widget_ = new NodeWidget(this);
  connect(this, &NodePanel::shown, node_widget_->view(), &NodeView::CenterOnItemsBoundingRect);

  connect(node_widget_->view(), &NodeView::NodesSelected, this, &NodePanel::NodesSelected);
  connect(node_widget_->view(), &NodeView::NodesDeselected, this, &NodePanel::NodesDeselected);
  connect(node_widget_->view(), &NodeView::NodeSelectionChanged, this, &NodePanel::NodeSelectionChanged);
  connect(node_widget_->view(), &NodeView::NodeSelectionChangedWithContexts, this,
          &NodePanel::NodeSelectionChangedWithContexts);
  connect(node_widget_->view(), &NodeView::NodeGroupOpened, this, &NodePanel::NodeGroupOpened);
  connect(node_widget_->view(), &NodeView::NodeGroupClosed, this, &NodePanel::NodeGroupClosed);

  // Set it as the main widget of this panel
  SetWidgetWithPadding(node_widget_);

  // Set strings
  Retranslate();
}

}  // namespace olive
