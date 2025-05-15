#ifndef MAINSTATUSBAR_H
#define MAINSTATUSBAR_H

#include <QProgressBar>
#include <QStatusBar>

#include "task/taskmanager.h"

namespace olive {

/**
 * @brief Shows abbreviated information from a TaskManager object
 */
class MainStatusBar : public QStatusBar {
  Q_OBJECT
 public:
  explicit MainStatusBar(QWidget* parent = nullptr);

  void ConnectTaskManager(TaskManager* manager);

 signals:
  void DoubleClicked();

 protected:
  void mouseDoubleClickEvent(QMouseEvent* e) override;

 private slots:
  void UpdateStatus();

  void SetProgressBarValue(double d);

  void ConnectedTaskDeleted();

 private:
  TaskManager* manager_;

  QProgressBar* bar_;

  Task* connected_task_;
};

}  // namespace olive

#endif  // MAINSTATUSBAR_H
