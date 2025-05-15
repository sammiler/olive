#include "mainstatusbar.h"

#include <QCoreApplication>

namespace olive {

MainStatusBar::MainStatusBar(QWidget* parent) : QStatusBar(parent), manager_(nullptr), connected_task_(nullptr) {
  setSizeGripEnabled(false);

  bar_ = new QProgressBar();
  addPermanentWidget(bar_);

  bar_->setMinimum(0);
  bar_->setMaximum(100);
  bar_->setVisible(false);

  showMessage(tr("Welcome to %1 %2").arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion()),
              10000);
}

void MainStatusBar::ConnectTaskManager(TaskManager* manager) {
  if (manager_) {
    disconnect(manager_, &TaskManager::TaskListChanged, this, &MainStatusBar::UpdateStatus);
  }

  manager_ = manager;

  if (manager_) {
    connect(manager_, &TaskManager::TaskListChanged, this, &MainStatusBar::UpdateStatus);
  }
}

void MainStatusBar::UpdateStatus() {
  if (!manager_) {
    return;
  }

  if (manager_->GetTaskCount() == 0) {
    clearMessage();
    bar_->setVisible(false);
    bar_->setValue(0);
  } else {
    Task* t = manager_->GetFirstTask();

    if (manager_->GetTaskCount() == 1) {
      showMessage(t->GetTitle());
    } else {
      showMessage(tr("Running %n background task(s)", nullptr, manager_->GetTaskCount()));
    }

    bar_->setVisible(true);

    if (connected_task_) {
      disconnect(connected_task_, &Task::ProgressChanged, this, &MainStatusBar::SetProgressBarValue);
      disconnect(connected_task_, &Task::destroyed, this, &MainStatusBar::ConnectedTaskDeleted);
    }

    connected_task_ = t;
    connect(connected_task_, &Task::ProgressChanged, this, &MainStatusBar::SetProgressBarValue);
    connect(connected_task_, &Task::destroyed, this, &MainStatusBar::ConnectedTaskDeleted);
  }
}

void MainStatusBar::SetProgressBarValue(double d) { bar_->setValue(qRound(100.0 * d)); }

void MainStatusBar::ConnectedTaskDeleted() { connected_task_ = nullptr; }

void MainStatusBar::mouseDoubleClickEvent(QMouseEvent* e) {
  QStatusBar::mouseDoubleClickEvent(e);

  emit DoubleClicked();
}

}  // namespace olive
