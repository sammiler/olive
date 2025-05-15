

#ifndef RIPPLETIMELINETOOL_H
#define RIPPLETIMELINETOOL_H

#include "pointer.h"

namespace olive {

class RippleTool : public PointerTool {
 public:
  explicit RippleTool(TimelineWidget* parent);

 protected:
  void FinishDrag(TimelineViewMouseEvent* event) override;

  void InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers) override;
};

}  // namespace olive

#endif  // RIPPLETIMELINETOOL_H
