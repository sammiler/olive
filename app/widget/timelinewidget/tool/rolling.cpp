#include "widget/timelinewidget/timelinewidget.h"

#include "node/block/gap/gap.h"
#include "node/nodeundo.h"
#include "rolling.h"

namespace olive {

RollingTool::RollingTool(TimelineWidget* parent) : PointerTool(parent) {
  SetMovementAllowed(false);
  SetGapTrimmingAllowed(true);
}

void RollingTool::InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers) {
  InitiateDragInternal(clicked_item, trim_mode, modifiers, false, true, false);
}

}  // namespace olive
