

#ifndef NODETABLEVIEW_H
#define NODETABLEVIEW_H

#include <QTreeWidget>

#include "node/node.h"

namespace olive {

class NodeTableView : public QTreeWidget {
  Q_OBJECT
 public:
  explicit NodeTableView(QWidget* parent = nullptr);

  void SelectNodes(const QVector<Node*>& nodes);

  void DeselectNodes(const QVector<Node*>& nodes);

  void SetTime(const rational& time);

 private:
  QMap<Node*, QTreeWidgetItem*> top_level_item_map_;

  rational last_time_;
};

}  // namespace olive

#endif  // NODETABLEVIEW_H
