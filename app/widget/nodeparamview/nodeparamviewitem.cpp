#include "nodeparamviewitem.h"

#include <QCheckBox>
#include <QDebug>

#include "common/qtutils.h"
#include "core.h"
#include "dialog/speedduration/speeddurationdialog.h"
#include "node/group/group.h"
#include "node/nodeundo.h"
#include "node/project/sequence/sequence.h"

namespace olive {

const int NodeParamViewItemBody::kKeyControlColumn = 10;
const int NodeParamViewItemBody::kArrayInsertColumn = kKeyControlColumn - 1;
const int NodeParamViewItemBody::kArrayRemoveColumn = kArrayInsertColumn - 1;
const int NodeParamViewItemBody::kExtraButtonColumn = kKeyControlColumn - 1;

const int NodeParamViewItemBody::kOptionalCheckBox = 0;
const int NodeParamViewItemBody::kArrayCollapseBtnColumn = 1;
const int NodeParamViewItemBody::kLabelColumn = 2;
const int NodeParamViewItemBody::kWidgetStartColumn = 3;
const int NodeParamViewItemBody::kMaxWidgetColumn = kArrayRemoveColumn;

#define super NodeParamViewItemBase

NodeParamViewItem::NodeParamViewItem(Node* node, NodeParamViewCheckBoxBehavior create_checkboxes, QWidget* parent)
    : super(parent),
      body_(nullptr),
      node_(node),
      create_checkboxes_(create_checkboxes),
      ctx_(nullptr),
      time_target_(nullptr) {
  node_->Retranslate();

  // Create and add contents widget
  RecreateBody();

  connect(node_, &Node::LabelChanged, this, &NodeParamViewItem::Retranslate);
  connect(node_, &Node::InputArraySizeChanged, this, &NodeParamViewItem::InputArraySizeChanged);

  // FIXME: Implemented to pick up when an input is set to hidden or not - DEFINITELY not a fast
  //        way of doing this, but "fine" for now.
  connect(node_, &Node::InputFlagsChanged, this, &NodeParamViewItem::RecreateBody);

  setBackgroundRole(QPalette::Window);

  // Connect title bar enabled checkbox
  // title_bar()->SetEnabledCheckBoxVisible(true);
  // title_bar()->SetEnabledCheckBoxChecked(node_->IsEnabled());
  // connect(title_bar(), &NodeParamViewItemTitleBar::EnabledCheckBoxClicked, node_, &Node::SetEnabled);

  Retranslate();
}

void NodeParamViewItem::Retranslate() {
  node_->Retranslate();

  title_bar()->SetText(GetTitleBarTextFromNode(node_));

  body_->Retranslate();
}

void NodeParamViewItem::RecreateBody() {
  if (body_) {
    body_->setParent(nullptr);
    body_->deleteLater();
  }

  body_ = new NodeParamViewItemBody(node_, create_checkboxes_, this);
  connect(body_, &NodeParamViewItemBody::RequestSelectNode, this, &NodeParamViewItem::RequestSelectNode);
  connect(body_, &NodeParamViewItemBody::ArrayExpandedChanged, this, &NodeParamViewItem::ArrayExpandedChanged);
  connect(body_, &NodeParamViewItemBody::InputCheckedChanged, this, &NodeParamViewItem::InputCheckedChanged);
  connect(body_, &NodeParamViewItemBody::RequestEditTextInViewer, this, &NodeParamViewItem::RequestEditTextInViewer);
  body_->Retranslate();
  body_->SetTimebase(timebase_);
  body_->SetTimeTarget(time_target_);
  SetBody(body_);
}

int NodeParamViewItem::GetElementY(const NodeInput& c) const {
  if (IsExpanded()) {
    return body_->GetElementY(c);
  } else {
    // Not expanded, put keyframes at the titlebar Y
    return mapToGlobal(title_bar()->rect().center()).y();
  }
}

void NodeParamViewItem::SetInputChecked(const NodeInput& input, bool e) { body_->SetInputChecked(input, e); }

NodeParamViewItemBody::NodeParamViewItemBody(Node* node, NodeParamViewCheckBoxBehavior create_checkboxes,
                                             QWidget* parent)
    : QWidget(parent), node_(node), time_target_(nullptr), create_checkboxes_(create_checkboxes) {
  auto* root_layout = new QGridLayout(this);

  int insert_row = 0;

  QVector<Node*> connected_signals;

  // Create widgets all root level components
  foreach (QString input, node->inputs()) {
    Node* n = node;

    NodeInput resolved = NodeGroup::ResolveInput(NodeInput(n, input));
    if (!connected_signals.contains(resolved.node())) {
      connect(resolved.node(), &Node::InputArraySizeChanged, this, &NodeParamViewItemBody::InputArraySizeChanged);
      connect(resolved.node(), &Node::InputConnected, this, &NodeParamViewItemBody::EdgeChanged);
      connect(resolved.node(), &Node::InputDisconnected, this, &NodeParamViewItemBody::EdgeChanged);

      connected_signals.append(resolved.node());
    }

    input_group_lookup_.insert({resolved.node(), resolved.input()}, {n, input});

    if (!(n->GetInputFlags(input) & kInputFlagHidden)) {
      CreateWidgets(root_layout, n, input, -1, insert_row);

      insert_row++;

      if (n->InputIsArray(input)) {
        // Insert here
        auto* array_widget = new QWidget(this);

        auto* array_layout = new QGridLayout(array_widget);
        array_layout->setContentsMargins(QtUtils::QFontMetricsWidth(fontMetrics(), QStringLiteral("    ")), 0, 0, 0);

        root_layout->addWidget(array_widget, insert_row, 1, 1, 10);

        // Start with zero elements for efficiency. We will make the widgets for them if the user
        // requests the array UI to be expanded
        int arr_sz = 0;

        // Add one last add button for appending to the array
        auto* append_btn = new NodeParamViewArrayButton(NodeParamViewArrayButton::kAdd, this);
        connect(append_btn, &NodeParamViewArrayButton::clicked, this, &NodeParamViewItemBody::ArrayAppendClicked);
        array_layout->addWidget(append_btn, arr_sz, kArrayInsertColumn);

        array_widget->setVisible(false);

        array_ui_.insert({n, input}, {array_widget, arr_sz, append_btn});

        insert_row++;
      }
    }
  }
}

void NodeParamViewItemBody::CreateWidgets(QGridLayout* layout, Node* node, const QString& input, int element, int row) {
  NodeInput input_ref(node, input, element);

  InputUI ui_objects;

  // Store layout and row
  ui_objects.layout = layout;
  ui_objects.row = row;

  // Create optional checkbox if requested
  if (create_checkboxes_) {
    ui_objects.optional_checkbox = new QCheckBox(this);
    connect(ui_objects.optional_checkbox, &QCheckBox::clicked, this, &NodeParamViewItemBody::OptionalCheckBoxClicked);
    layout->addWidget(ui_objects.optional_checkbox, row, kOptionalCheckBox);

    if (create_checkboxes_ == kCheckBoxesOnNonConnected && input_ref.IsConnected()) {
      ui_objects.optional_checkbox->setVisible(false);
    }
  }

  // Add descriptor label
  ui_objects.main_label = new QLabel(this);

  // Create input label
  layout->addWidget(ui_objects.main_label, row, kLabelColumn);

  if (node->InputIsArray(input)) {
    if (element == -1) {
      // Create a collapse toggle for expanding/collapsing the array
      auto* array_collapse_btn = new CollapseButton(this);

      // Default to collapsed
      array_collapse_btn->setChecked(false);

      // Add collapse button to layout
      layout->addWidget(array_collapse_btn, row, kArrayCollapseBtnColumn);

      // Connect signal to show/hide array params when toggled
      connect(array_collapse_btn, &CollapseButton::toggled, this, &NodeParamViewItemBody::ArrayCollapseBtnPressed);

      array_collapse_buttons_.insert({node, input}, array_collapse_btn);

    } else {
      auto* insert_element_btn = new NodeParamViewArrayButton(NodeParamViewArrayButton::kAdd, this);
      auto* remove_element_btn = new NodeParamViewArrayButton(NodeParamViewArrayButton::kRemove, this);

      layout->addWidget(insert_element_btn, row, kArrayInsertColumn);
      layout->addWidget(remove_element_btn, row, kArrayRemoveColumn);

      ui_objects.array_insert_btn = insert_element_btn;
      ui_objects.array_remove_btn = remove_element_btn;

      connect(insert_element_btn, &NodeParamViewArrayButton::clicked, this, &NodeParamViewItemBody::ArrayInsertClicked);
      connect(remove_element_btn, &NodeParamViewArrayButton::clicked, this, &NodeParamViewItemBody::ArrayRemoveClicked);
    }
  }

  // Create a widget/input bridge for this input
  ui_objects.widget_bridge = new NodeParamViewWidgetBridge(NodeInput(node, input, element), this);
  connect(ui_objects.widget_bridge, &NodeParamViewWidgetBridge::WidgetsRecreated, this,
          &NodeParamViewItemBody::ReplaceWidgets);
  connect(ui_objects.widget_bridge, &NodeParamViewWidgetBridge::ArrayWidgetDoubleClicked, this,
          &NodeParamViewItemBody::ToggleArrayExpanded);
  connect(ui_objects.widget_bridge, &NodeParamViewWidgetBridge::RequestEditTextInViewer, this,
          &NodeParamViewItemBody::RequestEditTextInViewer);

  // Place widgets into layout
  PlaceWidgetsFromBridge(layout, ui_objects.widget_bridge, row);

  // In case this input is a group, resolve that actual input to use for connected labels
  NodeInput resolved = NodeGroup::ResolveInput(input_ref);

  if (node->IsInputConnectable(input)) {
    // Create clickable label used when an input is connected
    ui_objects.connected_label = new NodeParamViewConnectedLabel(resolved, this);
    connect(ui_objects.connected_label, &NodeParamViewConnectedLabel::RequestSelectNode, this,
            &NodeParamViewItemBody::RequestSelectNode);
    layout->addWidget(ui_objects.connected_label, row, kWidgetStartColumn, 1, kKeyControlColumn - kWidgetStartColumn);
  }

  // Add keyframe control to this layout if parameter is keyframable
  if (node->IsInputKeyframable(input)) {
    ui_objects.key_control = new NodeParamViewKeyframeControl(this);
    ui_objects.key_control->SetInput(resolved);
    layout->addWidget(ui_objects.key_control, row, kKeyControlColumn);
  }

  input_ui_map_.insert(input_ref, ui_objects);

  if (node->IsInputConnectable(input)) {
    UpdateUIForEdgeConnection(input_ref);
  }

  SetTimeTargetOnInputUI(ui_objects);
  SetTimebaseOnInputUI(ui_objects);
}

void NodeParamViewItemBody::SetTimeTarget(ViewerOutput* target) {
  time_target_ = target;

  foreach (const InputUI& ui_obj, input_ui_map_) {
    SetTimeTargetOnInputUI(ui_obj);
  }
}

void NodeParamViewItemBody::SetTimeTargetOnInputUI(const InputUI& ui_obj) {
  // Only keyframable inputs have a key control widget
  if (ui_obj.key_control) {
    ui_obj.key_control->SetTimeTarget(time_target_);
  }
  if (ui_obj.connected_label) {
    ui_obj.connected_label->SetViewerNode(time_target_);
  }
  ui_obj.widget_bridge->SetTimeTarget(time_target_);
}

void NodeParamViewItemBody::Retranslate() {
  for (auto i = input_ui_map_.begin(); i != input_ui_map_.end(); i++) {
    const NodeInput& ic = i.key();

    if (ic.IsArray() && ic.element() >= 0) {
      // Make the label the array index
      i.value().main_label->setText(tr("%1:").arg(ic.element() + ic.GetProperty(QStringLiteral("arraystart")).toInt()));
    } else {
      // Set to the input's name
      i.value().main_label->setText(tr("%1:").arg(ic.name()));
    }
  }
}

int NodeParamViewItemBody::GetElementY(NodeInput c) const {
  if (c.IsArray() && !array_ui_.value(c.input_pair()).widget->isVisible()) {
    // Array is collapsed, so we'll return the Y of its root
    c.set_element(-1);
  }

  // c = NodeGroup::ResolveInput(c);

  // Find its row in the parameters
  QLabel* lbl = input_ui_map_.value(c).main_label;

  // Find label's Y position
  QPoint lbl_center = lbl->rect().center();

  // Find global position
  lbl_center = lbl->mapToGlobal(lbl_center);

  // Return Y
  return lbl_center.y();
}

void NodeParamViewItemBody::EdgeChanged(Node* output, const NodeInput& input) {
  Q_UNUSED(output)

  const NodeInputPair& pair = input_group_lookup_.value({input.node(), input.input()});
  NodeInput resolved(pair.node, pair.input, input.element());

  UpdateUIForEdgeConnection(resolved);
}

void NodeParamViewItemBody::UpdateUIForEdgeConnection(const NodeInput& input) {
  // Show/hide bridge widgets
  if (input_ui_map_.contains(input)) {
    const InputUI& ui_objects = input_ui_map_[input];

    bool is_connected = NodeGroup::ResolveInput(input).IsConnected();

    foreach (QWidget* w, ui_objects.widget_bridge->widgets()) {
      w->setVisible(!is_connected);
    }

    // Show/hide connection label
    ui_objects.connected_label->setVisible(is_connected);

    if (ui_objects.key_control) {
      ui_objects.key_control->setVisible(!is_connected);
    }

    // Show/hide optional checkbox if requested
    if (create_checkboxes_ == kCheckBoxesOnNonConnected) {
      ui_objects.optional_checkbox->setVisible(!is_connected);
    }
  }
}

void NodeParamViewItemBody::PlaceWidgetsFromBridge(QGridLayout* layout, NodeParamViewWidgetBridge* bridge, int row) {
  // Add widgets for this parameter to the layout
  for (int i = 0; i < bridge->widgets().size(); i++) {
    QWidget* w = bridge->widgets().at(i);

    int col = i + kWidgetStartColumn;

    int colspan;
    if (i == bridge->widgets().size() - 1) {
      // Span this widget among remaining columns
      colspan = kMaxWidgetColumn - col;
    } else {
      colspan = 1;
    }

    layout->addWidget(w, row, col, 1, colspan);
  }
}

void NodeParamViewItemBody::InputArraySizeChangedInternal(Node* node, const QString& input, int size) {
  NodeInputPair nip = {node, input};

  if (!array_ui_.contains(nip)) {
    return;
  }

  ArrayUI& array_ui = array_ui_[nip];

  if (size != array_ui.count) {
    auto* grid = dynamic_cast<QGridLayout*>(array_ui.widget->layout());

    if (array_ui.count < size) {
      // Our UI count is smaller than the size, create more
      grid->addWidget(array_ui.append_btn, size, kArrayInsertColumn);

      for (int i = array_ui.count; i < size; i++) {
        CreateWidgets(grid, node, input, i, i);
      }
    } else {
      for (int i = array_ui.count - 1; i >= size; i--) {
        // Our UI count is larger than the size, delete
        InputUI input_ui = input_ui_map_.take({node, input, i});
        delete input_ui.main_label;
        qDeleteAll(input_ui.widget_bridge->widgets());
        delete input_ui.widget_bridge;
        delete input_ui.connected_label;
        delete input_ui.key_control;
        delete input_ui.array_insert_btn;
        delete input_ui.array_remove_btn;
      }

      grid->addWidget(array_ui.append_btn, size, kArrayInsertColumn);
    }

    array_ui.count = size;

    Retranslate();
  }
}

void NodeParamViewItemBody::ArrayCollapseBtnPressed(bool checked) {
  const NodeInputPair& input = array_collapse_buttons_.key(dynamic_cast<CollapseButton*>(sender()));

  array_ui_.value(input).widget->setVisible(checked);
  if (checked) {
    // Ensure widgets are created (the signal will be ignored if they are)
    NodeInput resolved = NodeGroup::ResolveInput(NodeInput(input.node, input.input));
    InputArraySizeChangedInternal(input.node, input.input, resolved.GetArraySize());
  }

  emit ArrayExpandedChanged(checked);
}

void NodeParamViewItemBody::InputArraySizeChanged(const QString& input, int old_sz, int size) {
  Q_UNUSED(old_sz)

  NodeInputPair nip = input_group_lookup_.value({dynamic_cast<Node*>(sender()), input});

  InputArraySizeChangedInternal(nip.node, nip.input, size);
}

void NodeParamViewItemBody::ArrayAppendClicked() {
  for (auto it = array_ui_.cbegin(); it != array_ui_.cend(); it++) {
    if (it.value().append_btn == sender()) {
      NodeInput real_input = NodeGroup::ResolveInput(NodeInput(it.key().node, it.key().input));
      Core::instance()->undo_stack()->push(
          new NodeArrayInsertCommand(real_input.node(), real_input.input(), real_input.GetArraySize()),
          tr("Appended Array Element In %1 - %2").arg(real_input.node()->GetLabelAndName(), real_input.GetInputName()));
      break;
    }
  }
}

void NodeParamViewItemBody::ArrayInsertClicked() {
  for (auto it = input_ui_map_.cbegin(); it != input_ui_map_.cend(); it++) {
    if (it.value().array_insert_btn == sender()) {
      // Found our input and element
      NodeInput ic = NodeGroup::ResolveInput(it.key());
      Core::instance()->undo_stack()->push(
          new NodeArrayInsertCommand(ic.node(), ic.input(), ic.element()),
          tr("Inserted Array Element In %1 - %2").arg(ic.node()->GetLabelAndName(), ic.GetInputName()));
      break;
    }
  }
}

void NodeParamViewItemBody::ArrayRemoveClicked() {
  for (auto it = input_ui_map_.cbegin(); it != input_ui_map_.cend(); it++) {
    if (it.value().array_remove_btn == sender()) {
      // Found our input and element
      NodeInput ic = NodeGroup::ResolveInput(it.key());
      Core::instance()->undo_stack()->push(
          new NodeArrayRemoveCommand(ic.node(), ic.input(), ic.element()),
          tr("Removed Array Element In %1 - %2").arg(ic.node()->GetLabelAndName(), ic.GetInputName()));
      break;
    }
  }
}

void NodeParamViewItemBody::ToggleArrayExpanded() {
  auto* bridge = dynamic_cast<NodeParamViewWidgetBridge*>(sender());

  for (auto it = input_ui_map_.cbegin(); it != input_ui_map_.cend(); it++) {
    if (it.value().widget_bridge == bridge) {
      CollapseButton* b = array_collapse_buttons_.value(it.key().input_pair());
      b->setChecked(!b->isChecked());
      return;
    }
  }
}

void NodeParamViewItemBody::SetTimebase(const rational& timebase) {
  timebase_ = timebase;

  foreach (const InputUI& ui_obj, input_ui_map_) {
    SetTimebaseOnInputUI(ui_obj);
  }
}

void NodeParamViewItemBody::SetTimebaseOnInputUI(const InputUI& ui_obj) {
  ui_obj.widget_bridge->SetTimebase(timebase_);
}

void NodeParamViewItemBody::SetInputChecked(const NodeInput& input, bool e) {
  if (input_ui_map_.contains(input)) {
    QCheckBox* cb = input_ui_map_.value(input).optional_checkbox;
    if (cb) {
      cb->setChecked(e);
    }
  }
}

void NodeParamViewItemBody::ReplaceWidgets(const NodeInput& input) {
  InputUI ui = input_ui_map_.value(input);
  PlaceWidgetsFromBridge(ui.layout, ui.widget_bridge, ui.row);
}

void NodeParamViewItemBody::ShowSpeedDurationDialogForNode() {
  // We should only get there if the node is a clip, determined by the dynamic_cast in CreateWidgets
  SpeedDurationDialog sdd({dynamic_cast<ClipBlock*>(node_)}, timebase_, this);
  sdd.exec();
}

void NodeParamViewItemBody::OptionalCheckBoxClicked(bool e) {
  auto* cb = dynamic_cast<QCheckBox*>(sender());

  for (auto it = input_ui_map_.cbegin(); it != input_ui_map_.cend(); it++) {
    if (it.value().optional_checkbox == cb) {
      emit InputCheckedChanged(it.key(), e);
      break;
    }
  }
}

NodeParamViewItemBody::InputUI::InputUI()
    : main_label(nullptr),
      widget_bridge(nullptr),
      connected_label(nullptr),
      key_control(nullptr),
      extra_btn(nullptr),
      optional_checkbox(nullptr),
      array_insert_btn(nullptr),
      array_remove_btn(nullptr) {}

}  // namespace olive
