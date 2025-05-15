#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include "dialog/progress/progress.h"
#include "task/task.h"

namespace olive {

class TaskDialog : public ProgressDialog {
  Q_OBJECT
 public:
  /**
   * @brief TaskDialog Constructor
   *
   * Creates a TaskDialog. The TaskDialog takes ownership of the Task and will destroy it on close.
   * Connect to the Task::Succeeded() if you want to retrieve information from the task before it
   * gets destroyed.
   */
  TaskDialog(Task* task, const QString& title, QWidget* parent = nullptr);

  /**
   * @brief Set whether TaskDialog should destroy itself (and the task) when it's closed
   *
   * This is TRUE by default.
   */
  void SetDestroyOnClose(bool e) { destroy_on_close_ = e; }

  /**
   * @brief Returns this dialog's task
   */
  [[nodiscard]] Task* GetTask() const { return task_; }

 protected:
  void showEvent(QShowEvent* e) override;

  void closeEvent(QCloseEvent* e) override;

 signals:
  void TaskSucceeded(Task* task);

  void TaskFailed(Task* task);

 private:
  Task* task_;

  bool destroy_on_close_;

  bool already_shown_;

 private slots:
  void TaskFinished();
};

}  // namespace olive

#endif  // TASKDIALOG_H
