#ifndef TIMELINEUNDOWORKAREA_H
#define TIMELINEUNDOWORKAREA_H

#include "node/project.h"

namespace olive {

class WorkareaSetEnabledCommand : public UndoCommand {
 public:
  WorkareaSetEnabledCommand(Project* project, TimelineWorkArea* points, bool enabled)
      : project_(project), points_(points), old_enabled_(points_->enabled()), new_enabled_(enabled) {}

  [[nodiscard]] Project* GetRelevantProject() const override { return project_; }

 protected:
  void redo() override { points_->set_enabled(new_enabled_); }

  void undo() override { points_->set_enabled(old_enabled_); }

 private:
  Project* project_;

  TimelineWorkArea* points_;

  bool old_enabled_;

  bool new_enabled_;
};

class WorkareaSetRangeCommand : public UndoCommand {
 public:
  WorkareaSetRangeCommand(TimelineWorkArea* workarea, const TimeRange& range, const TimeRange& old_range)
      : workarea_(workarea), old_range_(old_range), new_range_(range) {}

  WorkareaSetRangeCommand(TimelineWorkArea* workarea, const TimeRange& range)
      : WorkareaSetRangeCommand(workarea, range, workarea->range()) {}

  [[nodiscard]] Project* GetRelevantProject() const override { return Project::GetProjectFromObject(workarea_); }

 protected:
  void redo() override { workarea_->set_range(new_range_); }

  void undo() override { workarea_->set_range(old_range_); }

 private:
  TimelineWorkArea* workarea_;

  TimeRange old_range_;

  TimeRange new_range_;
};

}  // namespace olive

#endif  // TIMELINEUNDOWORKAREA_H
