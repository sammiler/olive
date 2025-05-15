#ifndef PROJECTLOADBASETASK_H
#define PROJECTLOADBASETASK_H

#include "node/project.h"
#include "task/task.h"

namespace olive {

class ProjectLoadBaseTask : public Task {
  Q_OBJECT
 public:
  explicit ProjectLoadBaseTask(const QString& filename);

  [[nodiscard]] Project* GetLoadedProject() const { return project_; }

  [[nodiscard]] const QString& GetFilename() const { return filename_; }

  [[nodiscard]] const MainWindowLayoutInfo& GetLoadedLayout() const { return layout_; }

 protected:
  Project* project_;

  MainWindowLayoutInfo layout_;

 private:
  QString filename_;
};

}  // namespace olive

#endif  // LOADBASETASK_H
