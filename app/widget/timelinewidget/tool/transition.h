

#ifndef TRANSITIONTIMELINETOOL_H
#define TRANSITIONTIMELINETOOL_H

#include "add.h"

namespace olive {

class TransitionTool : public AddTool {
 public:
  explicit TransitionTool(TimelineWidget *parent);

  void HoverMove(TimelineViewMouseEvent *event) override;

  void MousePress(TimelineViewMouseEvent *event) override;
  void MouseMove(TimelineViewMouseEvent *event) override;
  void MouseRelease(TimelineViewMouseEvent *event) override;

 private:
  bool GetBlocksAtCoord(const TimelineCoordinate &coord, ClipBlock **primary, ClipBlock **secondary,
                        Timeline::MovementMode *trim_mode, rational *start_point);

  bool dual_transition_{};
};

}  // namespace olive

#endif  // TRANSITIONTIMELINETOOL_H
