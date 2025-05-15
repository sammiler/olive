#ifndef PROJECTSAVEMANAGER_H
#define PROJECTSAVEMANAGER_H

#include "node/project.h"
#include "task/task.h"

namespace olive {

class ProjectSaveTask : public Task {
  Q_OBJECT
 public:
  ProjectSaveTask(Project* project, bool use_compression);

  [[nodiscard]] Project* GetProject() const { return project_; }

  void SetOverrideFilename(const QString& filename) { override_filename_ = filename; }

  void SetLayout(const MainWindowLayoutInfo& layout) { layout_ = layout; }

 protected:
  bool Run() override;

 private:
  Project* project_;

  QString override_filename_;

  bool use_compression_;

  MainWindowLayoutInfo layout_;
};

}  // namespace olive

#endif  // PROJECTSAVEMANAGER_H
