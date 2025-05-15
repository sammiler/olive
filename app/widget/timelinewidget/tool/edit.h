#ifndef EDITTIMELINETOOL_H
#define EDITTIMELINETOOL_H

#include "beam.h"
#include "tool.h"
#include "widget/timelinewidget/timelinewidgetselections.h"

namespace olive {

class EditTool : public BeamTool {
 public:
  explicit EditTool(TimelineWidget *parent);

  void MousePress(TimelineViewMouseEvent *event) override;
  void MouseMove(TimelineViewMouseEvent *event) override;
  void MouseRelease(TimelineViewMouseEvent *event) override;
  void MouseDoubleClick(TimelineViewMouseEvent *event) override;

 private:
  TimelineWidgetSelections start_selections_;

  TimelineCoordinate start_coord_;
};

}  // namespace olive

#endif  // EDITTIMELINETOOL_H
