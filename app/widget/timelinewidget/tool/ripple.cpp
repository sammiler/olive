#include "widget/timelinewidget/timelinewidget.h"

#include "node/block/gap/gap.h"
#include "ripple.h"
#include "timeline/timelineundoripple.h"

namespace olive {

RippleTool::RippleTool(TimelineWidget* parent) : PointerTool(parent) {
  SetMovementAllowed(false);
  SetGapTrimmingAllowed(true);
}

void RippleTool::InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers) {
  InitiateDragInternal(clicked_item, trim_mode, modifiers, true, true, false);

  if (!parent()->HasGhosts()) {
    return;
  }

  // Find the earliest ripple
  rational earliest_ripple = RATIONAL_MAX;

  foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
    rational ghost_ripple_point;

    if (trim_mode == Timeline::kTrimIn) {
      ghost_ripple_point = ghost->GetIn();
    } else {
      ghost_ripple_point = ghost->GetOut();
    }

    earliest_ripple = qMin(earliest_ripple, ghost_ripple_point);
  }

  // For each track that does NOT have a ghost, we need to make one for Gaps
  foreach (Track* track, sequence()->GetTracks()) {
    if (track->IsLocked()) {
      continue;
    }

    // Determine if we've already created a ghost on this track
    bool ghost_on_this_track_exists = false;

    foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
      if (parent()->GetTrackFromReference(ghost->GetTrack()) == track) {
        ghost_on_this_track_exists = true;
        break;
      }
    }

    // If there's no ghost on this track, create one
    if (!ghost_on_this_track_exists) {
      // Find the block that starts just after or at the ripple point
      Block* block_after_ripple = track->NearestBlockAfterOrAt(earliest_ripple);

      // Exception for out-transitions, do not create a gap between them
      if (block_after_ripple) {
        if (auto* prev_clip = dynamic_cast<ClipBlock*>(block_after_ripple->previous())) {
          if (prev_clip->out_transition() == block_after_ripple) {
            block_after_ripple = block_after_ripple->next();
          }
        }
      }

      // If block is null, there will be no blocks after to ripple
      if (block_after_ripple) {
        TimelineViewGhostItem* ghost;

        if (dynamic_cast<GapBlock*>(block_after_ripple)) {
          // If this Block is already a Gap, ghost it now
          ghost = AddGhostFromBlock(block_after_ripple, trim_mode);
        } else {
          // Well we need to ripple SOMETHING, it'll either be the previous block if it's a gap
          // or we'll have to create a new gap ourselves
          Block* previous = block_after_ripple->previous();

          if (dynamic_cast<GapBlock*>(previous)) {
            // Previous is a gap, that'll make a fine substitute
            ghost = AddGhostFromBlock(previous, trim_mode);
          } else {
            // Previous is not a gap, we'll have to insert one there ourselves
            ghost =
                AddGhostFromNull(block_after_ripple->in(), block_after_ripple->in(), track->ToReference(), trim_mode);
            ghost->SetData(TimelineViewGhostItem::kReferenceBlock, QtUtils::PtrToValue(block_after_ripple));
          }
        }
      }
    }
  }
}

void RippleTool::FinishDrag(TimelineViewMouseEvent* event) {
  Q_UNUSED(event)

  if (parent()->HasGhosts()) {
    QVector<QHash<Track*, TrackListRippleToolCommand::RippleInfo> > info_list(Track::kCount);

    foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
      if (!ghost->HasBeenAdjusted()) {
        continue;
      }

      Track* track = parent()->GetTrackFromReference(ghost->GetTrack());

      TrackListRippleToolCommand::RippleInfo info{};
      auto* b = QtUtils::ValueToPtr<Block>(ghost->GetData(TimelineViewGhostItem::kAttachedBlock));

      if (b) {
        info.block = b;
        info.append_gap = false;
      } else {
        info.block = QtUtils::ValueToPtr<Block>(ghost->GetData(TimelineViewGhostItem::kReferenceBlock));
        info.append_gap = true;
      }

      info_list[track->type()].insert(track, info);
    }

    auto* command = new MultiUndoCommand();

    rational movement;

    if (drag_movement_mode() == Timeline::kTrimOut) {
      movement = parent()->GetGhostItems().first()->GetOutAdjustment();
    } else {
      movement = parent()->GetGhostItems().first()->GetInAdjustment();
    }

    for (int i = 0; i < info_list.size(); i++) {
      if (!info_list.at(i).isEmpty()) {
        command->add_child(new TrackListRippleToolCommand(sequence()->track_list(static_cast<Track::Type>(i)),
                                                          info_list.at(i), movement, drag_movement_mode()));
      }
    }

    if (command->child_count() > 0) {
      TimelineWidgetSelections new_sel = parent()->GetSelections();
      TimelineViewGhostItem* reference_ghost = parent()->GetGhostItems().first();
      if (drag_movement_mode() == Timeline::kTrimIn) {
        new_sel.TrimOut(-reference_ghost->GetInAdjustment());
      } else {
        new_sel.TrimOut(reference_ghost->GetOutAdjustment());
      }
      command->add_child(new TimelineWidget::SetSelectionsCommand(parent(), new_sel, parent()->GetSelections(), false));

      Core::instance()->undo_stack()->push(command, qApp->translate("RippleTool", "Rippled Clips"));
    } else {
      delete command;
    }
  }
}

}  // namespace olive
