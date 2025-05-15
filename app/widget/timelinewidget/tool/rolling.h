#ifndef ROLLINGTIMELINETOOL_H
#define ROLLINGTIMELINETOOL_H

#include "pointer.h"

namespace olive {

class RollingTool : public PointerTool {
 public:
  explicit RollingTool(TimelineWidget* parent);

 protected:
  void InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers) override;
};

}  // namespace olive

#endif  // ROLLINGTIMELINETOOL_H
