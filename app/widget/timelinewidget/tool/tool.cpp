#include "widget/timelinewidget/timelinewidget.h"

#include "node/block/transition/transition.h"

namespace olive {

const int TimelineTool::kDefaultDistanceFromOutput = -4;

TimelineTool::TimelineTool(TimelineWidget *parent) : dragging_(false), parent_(parent) {}

TimelineTool::~TimelineTool() = default;

TimelineWidget *TimelineTool::parent() { return parent_; }

Sequence *TimelineTool::sequence() { return parent_->sequence(); }

Timeline::MovementMode TimelineTool::FlipTrimMode(const Timeline::MovementMode &trim_mode) {
  if (trim_mode == Timeline::kTrimIn) {
    return Timeline::kTrimOut;
  }

  if (trim_mode == Timeline::kTrimOut) {
    return Timeline::kTrimIn;
  }

  return trim_mode;
}

rational TimelineTool::SnapMovementToTimebase(const rational &start, rational movement, const rational &timebase) {
  rational proposed_position = start + movement;
  rational snapped = Timecode::snap_time_to_timebase(proposed_position, timebase);

  if (proposed_position != snapped) {
    movement += snapped - proposed_position;
  }

  return movement;
}

rational TimelineTool::ValidateTimeMovement(rational movement) {
  bool first_ghost = true;

  foreach (TimelineViewGhostItem *ghost, parent()->GetGhostItems()) {
    if (ghost->GetMode() != Timeline::kMove) {
      continue;
    }

    // Prevents any ghosts from going below 0:00:00 time
    if (ghost->GetIn() + movement < rational(0)) {
      movement = -ghost->GetIn();
    } else if (first_ghost) {
      // Ensure ghost is snapped to a grid
      movement =
          SnapMovementToTimebase(ghost->GetIn(), movement, parent()->GetTimebaseForTrackType(ghost->GetTrack().type()));

      first_ghost = false;
    }
  }

  return movement;
}

int TimelineTool::ValidateTrackMovement(int movement, const QVector<TimelineViewGhostItem *> &ghosts) {
  foreach (TimelineViewGhostItem *ghost, ghosts) {
    if (ghost->GetMode() != Timeline::kMove) {
      continue;
    }

    if (!ghost->GetCanMoveTracks()) {
      return 0;

    } else if (ghost->GetTrack().index() + movement < 0) {
      // Prevents any ghosts from going to a non-existent negative track
      movement = -ghost->GetTrack().index();
    }
  }

  return movement;
}

void TimelineTool::GetGhostData(rational *earliest_point, rational *latest_point) {
  rational ep = RATIONAL_MAX;
  rational lp = RATIONAL_MIN;

  foreach (TimelineViewGhostItem *ghost, parent()->GetGhostItems()) {
    ep = qMin(ep, ghost->GetAdjustedIn());
    lp = qMax(lp, ghost->GetAdjustedOut());
  }

  if (earliest_point) {
    *earliest_point = ep;
  }

  if (latest_point) {
    *latest_point = lp;
  }
}

void TimelineTool::InsertGapsAtGhostDestination(olive::MultiUndoCommand *command) {
  rational earliest_point, latest_point;

  GetGhostData(&earliest_point, &latest_point);

  parent()->InsertGapsAt(earliest_point, latest_point - earliest_point, command);
}

}  // namespace olive
