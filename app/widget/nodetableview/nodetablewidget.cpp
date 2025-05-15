

#include "nodetablewidget.h"

#include <QVBoxLayout>

namespace olive {

NodeTableWidget::NodeTableWidget(QWidget* parent) : TimeBasedWidget(parent) {
  auto* layout = new QVBoxLayout(this);
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  view_ = new NodeTableView();
  layout->addWidget(view_);
}

}  // namespace olive
