#ifndef SLIDETIMELINETOOL_H
#define SLIDETIMELINETOOL_H

#include "pointer.h"

namespace olive {

class SlideTool : public PointerTool {
 public:
  explicit SlideTool(TimelineWidget* parent);

 protected:
  void InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers) override;
};

}  // namespace olive

#endif  // SLIDETIMELINETOOL_H
