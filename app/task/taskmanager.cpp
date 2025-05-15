#include "taskmanager.h"

#include <QDebug>
#include <QThread>

namespace olive {

TaskManager* TaskManager::instance_ = nullptr;

TaskManager::TaskManager() { thread_pool_.setMaxThreadCount(1); }

TaskManager::~TaskManager() {
  thread_pool_.clear();

  foreach (Task* t, tasks_) {
    t->Cancel();
  }

  thread_pool_.waitForDone();

  foreach (Task* t, tasks_) {
    t->deleteLater();
  }
}

void TaskManager::CreateInstance() { instance_ = new TaskManager(); }

void TaskManager::DestroyInstance() {
  delete instance_;
  instance_ = nullptr;
}

TaskManager* TaskManager::instance() { return instance_; }

int TaskManager::GetTaskCount() const { return tasks_.size(); }

Task* TaskManager::GetFirstTask() const { return tasks_.begin().value(); }

void TaskManager::CancelTaskAndWait(Task* t) {
  t->Cancel();

  QFutureWatcher<bool>* w = tasks_.key(t);

  if (w) {
    w->waitForFinished();
  }
}

void TaskManager::AddTask(Task* t) {
  // Create a watcher for signalling
  auto* watcher = new QFutureWatcher<bool>();
  connect(watcher, &QFutureWatcher<bool>::finished, this, &TaskManager::TaskFinished);

  // Add the Task to the queue
  tasks_.insert(watcher, t);

  // Run task concurrently
  watcher->setFuture(
#if QT_VERSION_MAJOR >= 6
      QtConcurrent::run(&thread_pool_, &Task::Start, t)
#else
      QtConcurrent::run(&thread_pool_, t, &Task::Start)
#endif
  );

  // Emit signal that a Task was added
  emit TaskAdded(t);
  emit TaskListChanged();
}

void TaskManager::CancelTask(Task* t) {
  if (std::find(failed_tasks_.begin(), failed_tasks_.end(), t) != failed_tasks_.end()) {
    failed_tasks_.remove(t);
    emit TaskRemoved(t);
    t->deleteLater();
  } else {
    t->Cancel();
  }
}

void TaskManager::TaskFinished() {
  auto* watcher = dynamic_cast<QFutureWatcher<bool>*>(sender());
  Task* t = tasks_.value(watcher);

  tasks_.remove(watcher);

  if (watcher->result()) {
    // Task completed successfully
    emit TaskRemoved(t);
    t->deleteLater();
  } else {
    // Task failed, keep it so the user can see the error message
    emit TaskFailed(t);
    failed_tasks_.push_back(t);
  }

  watcher->deleteLater();

  emit TaskListChanged();
}

}  // namespace olive
