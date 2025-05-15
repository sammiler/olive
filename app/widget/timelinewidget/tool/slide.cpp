#include "widget/timelinewidget/timelinewidget.h"

#include "node/block/gap/gap.h"
#include "node/nodeundo.h"
#include "slide.h"

namespace olive {

SlideTool::SlideTool(TimelineWidget* parent) : PointerTool(parent) {
  SetTrimmingAllowed(false);
  SetTrackMovementAllowed(false);
  SetGapTrimmingAllowed(true);
}

void SlideTool::InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers) {
  InitiateDragInternal(clicked_item, trim_mode, modifiers, false, true, true);
}

}  // namespace olive
