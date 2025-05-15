#ifndef PROJECTLOADMANAGER_H
#define PROJECTLOADMANAGER_H

#include "loadbasetask.h"
#include "window/mainwindow/mainwindowlayoutinfo.h"

namespace olive {

class ProjectLoadTask : public ProjectLoadBaseTask {
  Q_OBJECT
 public:
  explicit ProjectLoadTask(const QString& filename);

 protected:
  bool Run() override;
};

}  // namespace olive

#endif  // PROJECTLOADMANAGER_H
