#ifndef TASKMANAGER_PANEL_H
#define TASKMANAGER_PANEL_H

#include "panel/panel.h"
#include "widget/taskview/taskview.h"

namespace olive {

/**
 * @brief A PanelWidget wrapper around a TaskView widget
 */
class TaskManagerPanel : public PanelWidget {
  Q_OBJECT
 public:
  TaskManagerPanel();

 private:
  void Retranslate() override;

  TaskView* view_;
};

}  // namespace olive

#endif  // TASKMANAGER_H
