#include "historywidget.h"

#include "core.h"

namespace olive {

HistoryWidget::HistoryWidget(QWidget *parent) : QTreeView(parent) {
  stack_ = Core::instance()->undo_stack();

  this->setModel(stack_);
  this->setRootIsDecorated(false);
  connect(stack_, &UndoStack::indexChanged, this, &HistoryWidget::indexChanged);
  connect(this->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &HistoryWidget::currentRowChanged);
}

void HistoryWidget::indexChanged(int i) {
  this->selectionModel()->select(this->model()->index(i - 1, 0),
                                 QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void HistoryWidget::currentRowChanged(const QModelIndex &current, const QModelIndex &previous) {
  size_t jump_to = (current.row() + 1);
  stack_->jump(jump_to);
}

}  // namespace olive
