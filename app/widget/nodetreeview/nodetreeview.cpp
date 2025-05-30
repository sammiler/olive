#include "nodetreeview.h"

#include <QEvent>

namespace olive {

NodeTreeView::NodeTreeView(QWidget* parent)
    : QTreeWidget(parent),
      only_show_keyframable_(false),
      show_keyframe_tracks_as_rows_(false),
      checkboxes_enabled_(false) {
  connect(this, &NodeTreeView::itemChanged, this, &NodeTreeView::ItemCheckStateChanged);
  connect(this, &NodeTreeView::itemSelectionChanged, this, &NodeTreeView::SelectionChanged);

  Retranslate();
}

bool NodeTreeView::IsNodeEnabled(Node* n) const { return !disabled_nodes_.contains(n); }

bool NodeTreeView::IsInputEnabled(const NodeKeyframeTrackReference& ref) const {
  return !disabled_inputs_.contains(ref);
}

void NodeTreeView::SetKeyframeTrackColor(const NodeKeyframeTrackReference& ref, const QColor& color) {
  // Insert into hashmap
  keyframe_colors_.insert(ref, color);

  // If we currently have an item for this, set it
  QTreeWidgetItem* item = item_map_.value(ref);
  if (item) {
    item->setForeground(0, color);
  }
}

void NodeTreeView::SetNodes(const QVector<Node*>& nodes) {
  nodes_ = nodes;

  this->clear();
  item_map_.clear();

  foreach (Node* n, nodes_) {
    auto* node_item = new QTreeWidgetItem();
    node_item->setText(0, n->Name());
    if (checkboxes_enabled_) {
      node_item->setCheckState(0, disabled_nodes_.contains(n) ? Qt::Unchecked : Qt::Checked);
    }
    node_item->setData(0, kItemType, kItemTypeNode);
    node_item->setData(0, kItemNodePointer, QtUtils::PtrToValue(n));

    foreach (const QString& input, n->inputs()) {
      if (n->IsInputHidden(input) || (only_show_keyframable_ && !n->IsInputKeyframable(input))) {
        continue;
      }

      QTreeWidgetItem* input_item = nullptr;

      int arr_sz = n->InputArraySize(input);
      for (int i = -1; i < arr_sz; i++) {
        NodeInput input_ref(n, input, i);
        const QVector<NodeKeyframeTrack>& key_tracks = n->GetKeyframeTracks(input_ref);

        int this_element_track;

        if (show_keyframe_tracks_as_rows_ && (key_tracks.size() == 1 || (i == -1 && n->InputIsArray(input)))) {
          this_element_track = 0;
        } else {
          this_element_track = -1;
        }

        QTreeWidgetItem* element_item;

        if (input_item) {
          element_item = CreateItem(input_item, NodeKeyframeTrackReference(input_ref, this_element_track));
        } else {
          input_item = CreateItem(node_item, NodeKeyframeTrackReference(input_ref, this_element_track));
          element_item = input_item;
        }

        if (show_keyframe_tracks_as_rows_ && key_tracks.size() > 1 && (!n->InputIsArray(input) || i >= 0)) {
          CreateItemsForTracks(element_item, input_ref, key_tracks.size());
        }
      }
    }

    // Add at the end to prevent unnecessary signalling while we're setting these objects up
    if (node_item->childCount() > 0) {
      this->addTopLevelItem(node_item);
    } else {
      delete node_item;
    }
  }

  expandAll();
}

void NodeTreeView::changeEvent(QEvent* e) {
  QTreeWidget::changeEvent(e);

  if (e->type() == QEvent::LanguageChange) {
    Retranslate();
  }
}

void NodeTreeView::mouseDoubleClickEvent(QMouseEvent* e) {
  QTreeWidget::mouseDoubleClickEvent(e);

  NodeKeyframeTrackReference ref = GetSelectedInput();

  if (ref.input().IsValid()) {
    emit InputDoubleClicked(ref);
  }
}

void NodeTreeView::Retranslate() { setHeaderLabel(tr("Nodes")); }

NodeKeyframeTrackReference NodeTreeView::GetSelectedInput() {
  QList<QTreeWidgetItem*> sel = selectedItems();

  NodeKeyframeTrackReference selected_ref;

  if (!sel.isEmpty()) {
    QTreeWidgetItem* item = sel.first();

    if (item->data(0, kItemType).toInt() == kItemTypeInput) {
      selected_ref = item->data(0, kItemInputReference).value<NodeKeyframeTrackReference>();
    } else {
      selected_ref =
          NodeKeyframeTrackReference(NodeInput(QtUtils::ValueToPtr<Node>(item->data(0, kItemNodePointer)), QString()));
    }
  }

  return selected_ref;
}

QTreeWidgetItem* NodeTreeView::CreateItem(QTreeWidgetItem* parent, const NodeKeyframeTrackReference& ref) {
  auto* input_item = new QTreeWidgetItem(parent);

  QString item_name;
  if (ref.track() == -1 || NodeValue::get_number_of_keyframe_tracks(ref.input().GetDataType()) == 1 ||
      (ref.input().IsArray() && ref.input().element() == -1)) {
    if (ref.input().element() == -1) {
      item_name = ref.input().name();
    } else {
      item_name = QString::number(ref.input().element());
    }
  } else {
    switch (ref.track()) {
      case 0:
        item_name = UseRGBAOverXYZW(ref) ? tr("R") : tr("X");
        break;
      case 1:
        item_name = UseRGBAOverXYZW(ref) ? tr("G") : tr("Y");
        break;
      case 2:
        item_name = UseRGBAOverXYZW(ref) ? tr("B") : tr("Z");
        break;
      case 3:
        item_name = UseRGBAOverXYZW(ref) ? tr("A") : tr("W");
        break;
      default:
        item_name = QString::number(ref.track());
    }
  }
  input_item->setText(0, item_name);

  if (checkboxes_enabled_) {
    input_item->setCheckState(0, disabled_inputs_.contains(ref) ? Qt::Unchecked : Qt::Checked);
  }
  input_item->setData(0, kItemType, kItemTypeInput);
  input_item->setData(0, kItemInputReference, QVariant::fromValue(ref));

  if (keyframe_colors_.contains(ref)) {
    input_item->setForeground(0, keyframe_colors_.value(ref));
  }

  item_map_.insert(ref, input_item);

  return input_item;
}

void NodeTreeView::CreateItemsForTracks(QTreeWidgetItem* parent, const NodeInput& input, int track_count) {
  for (int j = 0; j < track_count; j++) {
    CreateItem(parent, NodeKeyframeTrackReference(input, j));
  }
}

bool NodeTreeView::UseRGBAOverXYZW(const NodeKeyframeTrackReference& ref) {
  return ref.input().GetDataType() == NodeValue::kColor;
}

void NodeTreeView::ItemCheckStateChanged(QTreeWidgetItem* item, int column) {
  Q_UNUSED(column)

  switch (item->data(0, kItemType).toInt()) {
    case kItemTypeNode: {
      Node* n = QtUtils::ValueToPtr<Node>(item->data(0, kItemNodePointer));

      if (item->checkState(0) == Qt::Checked) {
        if (disabled_nodes_.contains(n)) {
          disabled_nodes_.removeOne(n);
          emit NodeEnableChanged(n, true);
        }
      } else if (!disabled_nodes_.contains(n)) {
        disabled_nodes_.append(n);
        emit NodeEnableChanged(n, false);
      }
      break;
    }
    case kItemTypeInput: {
      auto i = item->data(0, kItemInputReference).value<NodeKeyframeTrackReference>();

      if (item->checkState(0) == Qt::Checked) {
        if (disabled_inputs_.contains(i)) {
          disabled_inputs_.removeOne(i);
          emit InputEnableChanged(i, true);
        }
      } else if (!disabled_inputs_.contains(i)) {
        disabled_inputs_.append(i);
        emit InputEnableChanged(i, false);
      }
      break;
    }
  }
}

void NodeTreeView::SelectionChanged() { emit InputSelectionChanged(GetSelectedInput()); }

}  // namespace olive
