

#ifndef NODETABLEWIDGET_H
#define NODETABLEWIDGET_H

#include "nodetableview.h"
#include "widget/timebased/timebasedwidget.h"

namespace olive {

class NodeTableWidget : public TimeBasedWidget {
 public:
  explicit NodeTableWidget(QWidget* parent = nullptr);

  void SelectNodes(const QVector<Node*>& nodes) { view_->SelectNodes(nodes); }

  void DeselectNodes(const QVector<Node*>& nodes) { view_->DeselectNodes(nodes); }

 protected:
  void TimeChangedEvent(const rational& time) override { view_->SetTime(time); }

 private:
  NodeTableView* view_;
};

}  // namespace olive

#endif  // NODETABLEWIDGET_H
