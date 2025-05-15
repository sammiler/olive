

#ifndef TASKVIEWITEM_H
#define TASKVIEWITEM_H

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QStackedWidget>
#include <QWidget>

#include "elapsedcounterwidget.h"
#include "task/task.h"

namespace olive {

/**
 * @brief A widget that visually represents the status of a Task
 *
 * The TaskViewItem widget shows a description of the Task (Task::text(), a progress bar (updated by
 * Task::ProgressChanged), the Task's status (text generated from Task::status() or Task::error()), and provides
 * a cancel button (triggering Task::Cancel()) for cancelling a Task before it finishes.
 *
 * The main entry point is SetTask() after a Task and TaskViewItem objects are created.
 */
class TaskViewItem : public QFrame {
  Q_OBJECT
 public:
  explicit TaskViewItem(Task* task, QWidget* parent = nullptr);

  void Failed();

 signals:
  void TaskCancelled(Task* t);

 private:
  QLabel* task_name_lbl_;
  QProgressBar* progress_bar_;
  QPushButton* cancel_btn_;

  QStackedWidget* status_stack_;
  ElapsedCounterWidget* elapsed_timer_lbl_;
  QLabel* task_error_lbl_;

  Task* task_;

 private slots:
  void UpdateProgress(double d);
};

}  // namespace olive

#endif  // TASKVIEWITEM_H
