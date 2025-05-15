

#include "taskmanager.h"

#include "task/taskmanager.h"

namespace olive {

TaskManagerPanel::TaskManagerPanel() : PanelWidget(QStringLiteral("TaskManagerPanel")) {
  // Create task view
  view_ = new TaskView(this);

  // Set it as the main widget
  setWidget(view_);

  // Connect task view to the task manager
  connect(TaskManager::instance(), &TaskManager::TaskAdded, view_, &TaskView::AddTask);
  connect(TaskManager::instance(), &TaskManager::TaskRemoved, view_, &TaskView::RemoveTask);
  connect(TaskManager::instance(), &TaskManager::TaskFailed, view_, &TaskView::TaskFailed);
  connect(view_, &TaskView::TaskCancelled, TaskManager::instance(), &TaskManager::CancelTask);

  // Set strings
  Retranslate();
}

void TaskManagerPanel::Retranslate() { SetTitle(tr("Task Manager")); }

}  // namespace olive
