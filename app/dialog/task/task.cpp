

#include "task.h"

#include <QtConcurrent/QtConcurrent>

namespace olive {

#define super ProgressDialog

TaskDialog::TaskDialog(Task* task, const QString& title, QWidget* parent)
    : super(task->GetTitle(), title, parent), task_(task), destroy_on_close_(true), already_shown_(false) {
  // Clear task when this dialog is destroyed
  task_->setParent(this);

  // Connect the save manager progress signal to the progress bar update on the dialog
  connect(task_, &Task::ProgressChanged, this, &TaskDialog::SetProgress, Qt::QueuedConnection);

  // Connect cancel signal (must be a direct connection or it'll be queued after the task has
  // already finished)
  connect(this, &TaskDialog::Cancelled, task_, &Task::Cancel, Qt::DirectConnection);
}

void TaskDialog::showEvent(QShowEvent* e) {
  super::showEvent(e);

  if (!already_shown_) {
    // Create watcher for when the task finishes
    auto* task_watcher = new QFutureWatcher<bool>();

    // Listen for when the task finishes
    connect(task_watcher, &QFutureWatcher<bool>::finished, this, &TaskDialog::TaskFinished, Qt::QueuedConnection);

    // Run task in another thread with QtConcurrent
    task_watcher->setFuture(
#if QT_VERSION_MAJOR >= 6
        QtConcurrent::run(&Task::Start, task_)
#else
        QtConcurrent::run(task_, &Task::Start)
#endif
    );

    already_shown_ = true;
  }
}

void TaskDialog::closeEvent(QCloseEvent* e) {
  // Cancel task if it is running
  task_->Cancel();

  // Standard close function
  super::closeEvent(e);

  // Reset shown
  already_shown_ = false;

  // Clean up this task and dialog
  if (destroy_on_close_) {
    deleteLater();
  }
}

void TaskDialog::TaskFinished() {
  auto* task_watcher = dynamic_cast<QFutureWatcher<bool>*>(sender());

  if (task_watcher->result()) {
    emit TaskSucceeded(task_);
  } else {
    ShowErrorMessage(tr("Task Failed"), task_->GetError());
    emit TaskFailed(task_);
  }

  task_watcher->deleteLater();

  close();
}

}  // namespace olive
