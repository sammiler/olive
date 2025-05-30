#include "nodetableview.h"

#include <QCheckBox>
#include <QHeaderView>

#include "node/traverser.h"

namespace olive {

NodeTableView::NodeTableView(QWidget* parent) : QTreeWidget(parent) {
  setColumnCount(3);
  setHeaderLabels({tr("Type"), tr("Source"), tr("R/X"), tr("G/Y"), tr("B/Z"), tr("A/W")});
}

void NodeTableView::SelectNodes(const QVector<Node*>& nodes) {
  foreach (Node* n, nodes) {
    auto* top_item = new QTreeWidgetItem();
    top_item->setText(0, n->GetLabelAndName());
    top_item->setFirstColumnSpanned(true);
    this->addTopLevelItem(top_item);
    top_level_item_map_.insert(n, top_item);
  }

  SetTime(last_time_);
}

void NodeTableView::DeselectNodes(const QVector<Node*>& nodes) {
  foreach (Node* n, nodes) {
    delete top_level_item_map_.take(n);
  }
}

void NodeTableView::SetTime(const rational& time) {
  last_time_ = time;

  NodeTraverser traverser;

  for (auto i = top_level_item_map_.constBegin(); i != top_level_item_map_.constEnd(); i++) {
    Node* node = i.key();
    QTreeWidgetItem* item = i.value();

    // Generate a value database for this node at this time
    NodeValueDatabase db = traverser.GenerateDatabase(node, TimeRange(time, time));

    // Delete any children of this item that aren't in this database
    for (int j = 0; j < item->childCount(); j++) {
      if (!db.contains(item->child(j)->data(0, Qt::UserRole).toString())) {
        delete item->takeChild(j);
        j--;
      }
    }

    // Update all inputs
    for (auto l = db.begin(); l != db.end(); l++) {
      const NodeValueTable& table = l.value();

      if (!node->HasInputWithID(l.key())) {
        // Filters out table entries that aren't inputs (like "global")
        continue;
      }

      QTreeWidgetItem* input_item = nullptr;

      for (int j = 0; j < item->childCount(); j++) {
        QTreeWidgetItem* compare = item->child(j);

        if (compare->data(0, Qt::UserRole).toString() == l.key()) {
          input_item = compare;
          break;
        }
      }

      if (!input_item) {
        input_item = new QTreeWidgetItem();
        input_item->setText(0, node->GetInputName(l.key()));
        input_item->setData(0, Qt::UserRole, l.key());
        input_item->setFirstColumnSpanned(true);
        item->addChild(input_item);
      }

      // Create children if necessary
      while (input_item->childCount() < table.Count()) {
        input_item->addChild(new QTreeWidgetItem());
      }

      // Remove children if necessary
      while (input_item->childCount() > table.Count()) {
        delete input_item->takeChild(input_item->childCount() - 1);
      }

      for (int j = 0; j < table.Count(); j++) {
        const NodeValue& value = table.at(table.Count() - 1 - j);

        // Create item
        QTreeWidgetItem* sub_item = input_item->child(j);

        // Set data type name
        sub_item->setText(0, NodeValue::GetPrettyDataTypeName(value.type()));

        // Determine source
        QString source_name;
        if (value.source()) {
          source_name = value.source()->GetLabelAndName();
        } else {
          source_name = tr("(unknown)");
        }
        sub_item->setText(1, source_name);

        switch (value.type()) {
          case NodeValue::kVideoParams:
          case NodeValue::kAudioParams:
            // These types have no string representation
            break;
          case NodeValue::kTexture: {
            // NodeTraverser puts video params in here
            for (int k = 0; k < VideoParams::kRGBAChannelCount; k++) {
              this->setItemWidget(sub_item, 2 + k, new QCheckBox());
            }
            break;
          }
          default: {
            QVector<QVariant> split_values = value.to_split_value();
            for (int k = 0; k < split_values.size(); k++) {
              sub_item->setText(2 + k, NodeValue::ValueToString(value.type(), split_values.at(k), true));
            }
          }
        }
      }
    }
  }
}

}  // namespace olive
