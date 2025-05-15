

#ifndef NODETABLEPANEL_H
#define NODETABLEPANEL_H

#include "panel/timebased/timebased.h"
#include "widget/nodetableview/nodetablewidget.h"

namespace olive {

class NodeTablePanel : public TimeBasedPanel {
  Q_OBJECT
 public:
  NodeTablePanel();

 public slots:
  void SelectNodes(const QVector<Node*>& nodes) {
    dynamic_cast<NodeTableWidget*>(GetTimeBasedWidget())->SelectNodes(nodes);
  }

  void DeselectNodes(const QVector<Node*>& nodes) {
    dynamic_cast<NodeTableWidget*>(GetTimeBasedWidget())->DeselectNodes(nodes);
  }

 private:
  void Retranslate() override;
};

}  // namespace olive

#endif  // NODETABLEPANEL_H
