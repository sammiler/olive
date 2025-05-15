

#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QTreeView>

#include "undo/undostack.h"

namespace olive {

class HistoryWidget : public QTreeView {
  Q_OBJECT
 public:
  explicit HistoryWidget(QWidget *parent = nullptr);

 private:
  UndoStack *stack_;

  size_t current_row_{};

 private slots:
  void indexChanged(int i);

  void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
};

}  // namespace olive

#endif  // HISTORYWIDGET_H
