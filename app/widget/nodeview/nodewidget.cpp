

#include "nodewidget.h"

#include <QVBoxLayout>

namespace olive {

NodeWidget::NodeWidget(QWidget *parent) : QWidget(parent) {
  auto *outer_layout = new QVBoxLayout(this);
  outer_layout->setContentsMargins(0, 0, 0, 0);

  toolbar_ = new NodeViewToolBar();
  outer_layout->addWidget(toolbar_);

  // Create NodeView widget
  node_view_ = new NodeView(this);
  outer_layout->addWidget(node_view_);

  // Connect toolbar to NodeView
  connect(toolbar_, &NodeViewToolBar::MiniMapEnabledToggled, node_view_, &NodeView::SetMiniMapEnabled);
  connect(toolbar_, &NodeViewToolBar::AddNodeClicked, node_view_, &NodeView::ShowAddMenu);

  // Set defaults
  toolbar_->SetMiniMapEnabled(true);
  node_view_->SetMiniMapEnabled(true);

  setSizePolicy(node_view_->sizePolicy());
}

}  // namespace olive
