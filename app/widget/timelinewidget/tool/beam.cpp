#include "beam.h"
#include "widget/timelinewidget/timelinewidget.h"

namespace olive {

BeamTool::BeamTool(TimelineWidget *parent) : TimelineTool(parent) {}

void BeamTool::HoverMove(TimelineViewMouseEvent *event) {
  parent()->SetViewBeamCursor(ValidatedCoordinate(event->GetCoordinates(true)));
}

TimelineCoordinate BeamTool::ValidatedCoordinate(TimelineCoordinate coord) {
  if (Core::instance()->snapping()) {
    rational movement;
    parent()->SnapPoint({coord.GetFrame()}, &movement);
    if (!movement.isNull()) {
      coord.SetFrame(coord.GetFrame() + movement);
    }
  }

  return coord;
}

}  // namespace olive
