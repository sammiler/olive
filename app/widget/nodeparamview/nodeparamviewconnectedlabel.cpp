#include "nodeparamviewconnectedlabel.h"

#include <QHBoxLayout>
#include <utility>

#include "common/qtutils.h"
#include "core.h"
#include "node/node.h"
#include "node/nodeundo.h"
#include "widget/collapsebutton/collapsebutton.h"
#include "widget/menu/menu.h"

namespace olive {

NodeParamViewConnectedLabel::NodeParamViewConnectedLabel(NodeInput input, QWidget *parent)
    : QWidget(parent), input_(std::move(input)), connected_node_(nullptr), viewer_(nullptr) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  QSizePolicy p = sizePolicy();
  p.setHorizontalStretch(1);
  p.setHorizontalPolicy(QSizePolicy::Expanding);
  setSizePolicy(p);

  // Set up label area
  auto *label_layout = new QHBoxLayout();
  label_layout->setSpacing(QtUtils::QFontMetricsWidth(fontMetrics(), QStringLiteral(" ")));
  label_layout->setContentsMargins(0, 0, 0, 0);
  layout->addLayout(label_layout);

  auto *collapse_btn = new CollapseButton(this);
  collapse_btn->setChecked(false);
  label_layout->addWidget(collapse_btn);

  label_layout->addWidget(new QLabel(tr("Connected to"), this));

  connected_to_lbl_ = new ClickableLabel(this);
  connected_to_lbl_->setCursor(Qt::PointingHandCursor);
  connected_to_lbl_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(connected_to_lbl_, &ClickableLabel::MouseClicked, this, &NodeParamViewConnectedLabel::ConnectionClicked);
  connect(connected_to_lbl_, &ClickableLabel::customContextMenuRequested, this,
          &NodeParamViewConnectedLabel::ShowLabelContextMenu);
  label_layout->addWidget(connected_to_lbl_);

  label_layout->addStretch();

  // Set up "link" font
  QFont link_font = connected_to_lbl_->font();
  link_font.setUnderline(true);
  connected_to_lbl_->setForegroundRole(QPalette::Link);
  connected_to_lbl_->setFont(link_font);

  if (input_.IsConnected()) {
    InputConnected(input_.GetConnectedOutput(), input_);
  } else {
    InputDisconnected(nullptr, input_);
  }

  connect(input_.node(), &Node::InputConnected, this, &NodeParamViewConnectedLabel::InputConnected);
  connect(input_.node(), &Node::InputDisconnected, this, &NodeParamViewConnectedLabel::InputDisconnected);

  // Creating the tree is expensive, hold off until the user specifically requests it
  value_tree_ = nullptr;
  connect(collapse_btn, &CollapseButton::toggled, this, &NodeParamViewConnectedLabel::SetValueTreeVisible);
}

void NodeParamViewConnectedLabel::SetViewerNode(ViewerOutput *viewer) {
  if (viewer_) {
    disconnect(viewer_, &ViewerOutput::PlayheadChanged, this, &NodeParamViewConnectedLabel::UpdateValueTree);
  }

  viewer_ = viewer;

  if (viewer_) {
    connect(viewer_, &ViewerOutput::PlayheadChanged, this, &NodeParamViewConnectedLabel::UpdateValueTree);
    UpdateValueTree();
  }
}

void NodeParamViewConnectedLabel::CreateTree() {
  // Set up table area
  value_tree_ = new NodeValueTree(this);
  layout()->addWidget(value_tree_);
}

void NodeParamViewConnectedLabel::InputConnected(Node *output, const NodeInput &input) {
  if (input_ != input) {
    return;
  }

  connected_node_ = output;

  UpdateLabel();
}

void NodeParamViewConnectedLabel::InputDisconnected(Node *output, const NodeInput &input) {
  if (input_ != input) {
    return;
  }

  Q_UNUSED(output)

  connected_node_ = nullptr;

  UpdateLabel();
}

void NodeParamViewConnectedLabel::ShowLabelContextMenu() {
  Menu m(this);

  QAction *disconnect_action = m.addAction(tr("Disconnect"));
  connect(disconnect_action, &QAction::triggered, this, [this]() {
    Core::instance()->undo_stack()->push(new NodeEdgeRemoveCommand(connected_node_, input_),
                                         Node::GetDisconnectCommandString(connected_node_, input_));
  });

  m.exec(QCursor::pos());
}

void NodeParamViewConnectedLabel::ConnectionClicked() {
  if (connected_node_) {
    emit RequestSelectNode(connected_node_);
  }
}

void NodeParamViewConnectedLabel::UpdateLabel() {
  QString s;

  if (connected_node_) {
    s = connected_node_->Name();
  } else {
    s = tr("Nothing");
  }

  connected_to_lbl_->setText(s);
}

void NodeParamViewConnectedLabel::UpdateValueTree() {
  if (value_tree_ && viewer_ && value_tree_->isVisible()) {
    value_tree_->SetNode(input_, viewer_->GetPlayhead());
  }
}

void NodeParamViewConnectedLabel::SetValueTreeVisible(bool e) {
  if (value_tree_) {
    value_tree_->setVisible(e);
  }

  if (e) {
    if (!value_tree_) {
      CreateTree();
      value_tree_->setVisible(true);
    }

    UpdateValueTree();
  }
}

}  // namespace olive
