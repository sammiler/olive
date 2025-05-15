

#include "taskview.h"

#include <QPushButton>

namespace olive {

TaskView::TaskView(QWidget *parent) : QScrollArea(parent) {
  // Allow scroll area to resize widget to fit
  setWidgetResizable(true);

  // Create central widget
  central_widget_ = new QWidget(this);
  setWidget(central_widget_);

  // Create layout for central widget
  layout_ = new QVBoxLayout(central_widget_);
  layout_->setSpacing(0);
  layout_->setContentsMargins(0, 0, 0, 0);

  // Add a "stretch" so that TaskViewItems don't try to expand all the way to the bottom
  layout_->addStretch();
}

void TaskView::AddTask(Task *t) {
  // Create TaskViewItem (UI representation of a Task) and connect it
  auto *item = new TaskViewItem(t);
  connect(item, &TaskViewItem::TaskCancelled, this, &TaskView::TaskCancelled);
  items_.insert(t, item);
  layout_->insertWidget(layout_->count() - 1, item);
}

void TaskView::TaskFailed(Task *t) { items_.value(t)->Failed(); }

void TaskView::RemoveTask(Task *t) {
  items_.value(t)->deleteLater();
  items_.remove(t);
}

}  // namespace olive
