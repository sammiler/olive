#include "clitaskdialog.h"

namespace olive {

CLITaskDialog::CLITaskDialog(Task* task, QObject* parent) : CLIProgressDialog(task->GetTitle(), parent), task_(task) {
  connect(task_, &Task::ProgressChanged, this, &CLITaskDialog::SetProgress);
}

bool CLITaskDialog::Run() { return task_->Start(); }

}  // namespace olive
