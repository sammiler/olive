

#include "slip.h"

#include <QToolTip>

#include "config/config.h"
#include "timeline/timelineundogeneral.h"
#include "widget/timelinewidget/timelinewidget.h"

namespace olive {

SlipTool::SlipTool(TimelineWidget* parent) : PointerTool(parent) {
  SetTrimmingAllowed(false);
  SetTrackMovementAllowed(false);
}

void SlipTool::ProcessDrag(const TimelineCoordinate& mouse_pos) {
  // Determine frame movement
  rational time_movement = drag_start_.GetFrame() - mouse_pos.GetFrame();

  // Validate slip (enforce all ghosts moving in legal ways)
  foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
    if (ghost->GetMediaIn() + time_movement < rational(0)) {
      time_movement = -ghost->GetMediaIn();
    }
  }

  // Perform slip
  foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
    ghost->SetMediaInAdjustment(time_movement);
  }

  // Generate tooltip and force it to to update (otherwise the tooltip won't move as written in the
  // documentation, and could get in the way of the cursor)
  rational tooltip_timebase = parent()->GetTimebaseForTrackType(drag_start_.GetTrack().type());
  QToolTip::hideText();
  QToolTip::showText(QCursor::pos(),
                     QString::fromStdString(Timecode::time_to_timecode(time_movement, tooltip_timebase,
                                                                       Core::instance()->GetTimecodeDisplay(), true)),
                     parent());
}

void SlipTool::FinishDrag(TimelineViewMouseEvent* event) {
  Q_UNUSED(event)

  auto* command = new MultiUndoCommand();

  // Find earliest point to ripple around
  foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
    auto* b = QtUtils::ValueToPtr<Block>(ghost->GetData(TimelineViewGhostItem::kAttachedBlock));

    auto* cb = dynamic_cast<ClipBlock*>(b);
    if (cb) {
      command->add_child(new BlockSetMediaInCommand(cb, ghost->GetAdjustedMediaIn()));
    }
  }

  Core::instance()->undo_stack()->push(
      command, qApp->translate("SlipTool", "Slipped %1 Clip(s)").arg(parent()->GetGhostItems().size()));
}

}  // namespace olive
