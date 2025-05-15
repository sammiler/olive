#ifndef TASKVIEW_H
#define TASKVIEW_H

#include <QScrollArea>
#include <QVBoxLayout>

#include "widget/taskview/taskviewitem.h"

namespace olive {

/**
 * @brief A widget that shows a list of Tasks
 *
 * TaskView is a fairly simple widget for showing TaskViewItem widgets that each represent a Task object. The main
 * entry point is the slot AddTask() which should be connected to a TaskManager's TaskAdded() signal. No more connecting
 * is necessary since TaskViewItem will automatically delete itself (thus removing itself from the TaskView) when the
 * Task finishes.
 */
class TaskView : public QScrollArea {
  Q_OBJECT
 public:
  explicit TaskView(QWidget* parent);

 signals:
  void TaskCancelled(Task* t);

 public slots:
  /**
   * @brief Creates a TaskViewItem, connects it to a Task, and adds it to this widget
   *
   * Connect this to TaskManager::TaskAdded().
   */
  void AddTask(Task* t);

  void TaskFailed(Task* t);

  void RemoveTask(Task* t);

 private:
  QWidget* central_widget_;

  QVBoxLayout* layout_;

  QHash<Task*, TaskViewItem*> items_;
};

}  // namespace olive

#endif  // TASKVIEW_H
