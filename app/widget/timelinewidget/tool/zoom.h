

#ifndef ZOOMTIMELINETOOL_H
#define ZOOMTIMELINETOOL_H

#include "tool.h"

namespace olive {

class ZoomTool : public TimelineTool {
 public:
  explicit ZoomTool(TimelineWidget *parent);

  void MousePress(TimelineViewMouseEvent *event) override;
  void MouseMove(TimelineViewMouseEvent *event) override;
  void MouseRelease(TimelineViewMouseEvent *event) override;

 private:
  QPoint drag_global_start_;
};

}  // namespace olive

#endif  // ZOOMTIMELINETOOL_H
