#include "edit.h"
#include "widget/timelinewidget/timelinewidget.h"

namespace olive {

EditTool::EditTool(TimelineWidget *parent) : BeamTool(parent) {}

void EditTool::MousePress(TimelineViewMouseEvent *event) {
  if (!(event->GetModifiers() & Qt::ShiftModifier)) {
    parent()->DeselectAll();
  }
}

void EditTool::MouseMove(TimelineViewMouseEvent *event) {
  if (dragging_) {
    rational end_frame = event->GetFrame(true);

    if (Core::instance()->snapping()) {
      rational movement;
      parent()->SnapPoint({end_frame}, &movement);
      if (!movement.isNull()) {
        end_frame += movement;
      }
    }

    parent()->SetSelections(start_selections_, false);
    parent()->AddSelection(TimeRange(start_coord_.GetFrame(), end_frame), start_coord_.GetTrack());
  } else {
    start_selections_ = parent()->GetSelections();

    dragging_ = true;

    start_coord_ = event->GetCoordinates(true);

    // Snap if we're snapping
    if (Core::instance()->snapping()) {
      rational movement;
      parent()->SnapPoint({start_coord_.GetFrame()}, &movement);
      if (!movement.isNull()) {
        start_coord_.SetFrame(start_coord_.GetFrame() + movement);
      }
    }

    dragging_ = true;
  }
}

void EditTool::MouseRelease(TimelineViewMouseEvent *event) {
  auto current_sel = parent()->GetSelections();
  parent()->SetSelections(start_selections_, false);
  parent()->SetSelections(current_sel, true);

  dragging_ = false;
}

void EditTool::MouseDoubleClick(TimelineViewMouseEvent *event) {
  Block *item = parent()->GetItemAtScenePos(event->GetCoordinates());

  if (item && !item->track()->IsLocked()) {
    parent()->AddSelection(item);
  }
}

}  // namespace olive
