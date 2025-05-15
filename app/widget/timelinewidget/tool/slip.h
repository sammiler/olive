#ifndef SLIPTIMELINETOOL_H
#define SLIPTIMELINETOOL_H

#include "pointer.h"

namespace olive {

class SlipTool : public PointerTool {
 public:
  explicit SlipTool(TimelineWidget *parent);

 protected:
  void ProcessDrag(const TimelineCoordinate &mouse_pos) override;
  void FinishDrag(TimelineViewMouseEvent *event) override;
};

}  // namespace olive

#endif  // SLIPTIMELINETOOL_H
