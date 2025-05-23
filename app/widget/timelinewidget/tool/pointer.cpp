#include "widget/timelinewidget/timelinewidget.h"

#include <QDebug>
#include <QToolTip>

#include "common/qtutils.h"
#include "common/range.h"
#include "config/config.h"
#include "core.h"
#include "node/block/gap/gap.h"
#include "node/block/transition/transition.h"
#include "node/nodeundo.h"
#include "pointer.h"
#include "timeline/timelineundopointer.h"
#include "widget/timeruler/timeruler.h"

namespace olive {

PointerTool::PointerTool(TimelineWidget* parent)
    : TimelineTool(parent),
      movement_allowed_(true),
      trimming_allowed_(true),
      track_movement_allowed_(true),
      gap_trimming_allowed_(false),
      can_rubberband_select_(false),
      rubberband_selecting_(false) {}

void PointerTool::MousePress(TimelineViewMouseEvent* event) {
  const Track::Reference& track_ref = event->GetTrack();

  // Determine if item clicked on is selectable
  clicked_item_ = parent()->GetItemAtScenePos(event->GetCoordinates());
  auto* clip_clicked_item = dynamic_cast<ClipBlock*>(clicked_item_);

  can_rubberband_select_ = false;

  bool selectable_item = (clicked_item_ && !parent()->GetTrackFromReference(track_ref)->IsLocked());

  if (selectable_item) {
    // Cache the clip's type for use later
    drag_track_type_ = track_ref.type();

    // If we haven't started dragging yet, we'll initiate a drag here
    // Record where the drag started in timeline coordinates
    drag_start_ = event->GetCoordinates();

    // Determine whether we're trimming or moving based on the position of the cursor
    drag_movement_mode_ = IsCursorInTrimHandle(clicked_item_, event->GetSceneX());

    // If we're not in a trim mode, we must be in a move mode (provided the tool allows movement and
    // the block is not a gap)
    if (drag_movement_mode_ == Timeline::kNone && movement_allowed_ && !dynamic_cast<GapBlock*>(clicked_item_)) {
      drag_movement_mode_ = Timeline::kMove;
    }

    // If this item is already selected, no further selection needs to be made
    if (parent()->IsBlockSelected(clicked_item_)) {
      // Collect item deselections
      QVector<Block*> deselected_blocks;

      // If shift is held, deselect it
      if (event->GetModifiers() & Qt::ShiftModifier) {
        parent()->RemoveSelection(clicked_item_);
        deselected_blocks.append(clicked_item_);

        // If not holding alt, deselect all links as well
        if (clip_clicked_item && !(event->GetModifiers() & Qt::AltModifier)) {
          parent()->SetBlockLinksSelected(clip_clicked_item, false);
          deselected_blocks.append(clip_clicked_item->block_links());
        }
      }

      parent()->SignalDeselectedBlocks(deselected_blocks);

      return;
    }
  }

  // If not holding shift, deselect all clips
  if (!(event->GetModifiers() & Qt::ShiftModifier)) {
    parent()->DeselectAll();
  }

  if (selectable_item) {
    // Collect item selections
    QVector<Block*> selected_blocks;

    // Select this item
    parent()->AddSelection(clicked_item_);
    selected_blocks.append(clicked_item_);

    // If not holding alt, select all links as well
    if (clip_clicked_item && !(event->GetModifiers() & Qt::AltModifier)) {
      parent()->SetBlockLinksSelected(clip_clicked_item, true);
      selected_blocks.append(clip_clicked_item->block_links());
    }

    parent()->SignalSelectedBlocks(selected_blocks);
  }

  can_rubberband_select_ =
      (event->GetButton() == Qt::LeftButton  // Only rubberband select from the primary mouse button
       && (!selectable_item ||
           drag_movement_mode_ == Timeline::kNone));  // And if no item was selected OR the item isn't draggable

  if (can_rubberband_select_) {
    drag_global_start_ = QCursor::pos();
  }

  // If we click anywhere other than a marker, deselect all markers
  parent()->ruler()->DeselectAllMarkers();
}

void PointerTool::MouseMove(TimelineViewMouseEvent* event) {
  if (can_rubberband_select_) {
    if (!rubberband_selecting_) {
      // If we clicked an item but are rubberband selecting anyway, deselect it now
      if (clicked_item_) {
        parent()->RemoveSelection(clicked_item_);
        parent()->SignalDeselectedBlocks({clicked_item_});
        clicked_item_ = nullptr;
      }

      parent()->StartRubberBandSelect(drag_global_start_);

      rubberband_selecting_ = true;
    }

    // Process rubberband select
    parent()->MoveRubberBandSelect(true, !(event->GetModifiers() & Qt::AltModifier));

  } else {
    // Process drag
    if (!dragging_) {
      // Now that the cursor has moved, we will assume the intention is to drag

      // Clear snap points
      snap_points_.clear();

      // If we're performing an action, we can initiate ghosts
      if (drag_movement_mode_ != Timeline::kNone) {
        InitiateDrag(clicked_item_, drag_movement_mode_, event->GetModifiers());
      }

      // Set dragging to true here so no matter what, the drag isn't re-initiated until it's completed
      dragging_ = true;
    }

    if (dragging_ && !parent()->GetGhostItems().isEmpty()) {
      // We're already dragging AND we have ghosts to work with
      ProcessDrag(event->GetCoordinates());
    }
  }
}

void PointerTool::MouseRelease(TimelineViewMouseEvent* event) {
  if (rubberband_selecting_) {
    // Finish rubberband select
    parent()->EndRubberBandSelect();
    rubberband_selecting_ = false;
    return;
  }

  if (dragging_) {
    // If we were dragging, process the end of the drag
    if (!parent()->GetGhostItems().isEmpty()) {
      FinishDrag(event);
    }

    // Clean up
    parent()->ClearGhosts();
    snap_points_.clear();

    dragging_ = false;
  }
}

void PointerTool::HoverMove(TimelineViewMouseEvent* event) {
  if (trimming_allowed_) {
    // No dragging, but we still want to process cursors
    Block* block_at_cursor = parent()->GetItemAtScenePos(event->GetCoordinates());

    if (block_at_cursor) {
      switch (IsCursorInTrimHandle(block_at_cursor, event->GetSceneX())) {
        case Timeline::kTrimIn:
          parent()->setCursor(Qt::SizeHorCursor);
          break;
        case Timeline::kTrimOut:
          parent()->setCursor(Qt::SizeHorCursor);
          break;
        default:
          parent()->unsetCursor();
      }
    } else {
      parent()->unsetCursor();
    }
  } else {
    parent()->unsetCursor();
  }
}

void SetGhostToSlideMode(TimelineViewGhostItem* g) {
  g->SetCanMoveTracks(false);
  g->SetData(TimelineViewGhostItem::kGhostIsSliding, true);
}

void PointerTool::InitiateDragInternal(Block* clicked_item, Timeline::MovementMode trim_mode,
                                       Qt::KeyboardModifiers modifiers, bool dont_roll_trims, bool allow_nongap_rolling,
                                       bool slide_instead_of_moving) {
  // Get list of selected blocks
  QVector<Block*> clips = parent()->GetSelectedBlocks();

  if (trim_mode == Timeline::kMove) {
    // Gaps are not allowed to move, and since we only allow moving one block type at a time,
    // dragging a gap is a no-op
    if (dynamic_cast<GapBlock*>(clicked_item)) {
      return;
    }

    bool sliding_due_to_transition = false;

    if (!slide_instead_of_moving) {
      // If the user tries to move a transition without moving the clip it belongs to, we turn
      // this into a slide
      foreach (Block* block, clips) {
        if (auto* transit = dynamic_cast<TransitionBlock*>(block)) {
          if (!CanTransitionMove(transit, clips)) {
            slide_instead_of_moving = true;
            break;
          }
        } else if (auto* clip = dynamic_cast<ClipBlock*>(block)) {
          if ((clip->in_transition() && !CanTransitionMove(clip->in_transition(), clips)) ||
              (clip->out_transition() && !CanTransitionMove(clip->out_transition(), clips))) {
            slide_instead_of_moving = true;
            break;
          }
        }
      }

      sliding_due_to_transition = slide_instead_of_moving;
    }

    if (slide_instead_of_moving) {
      // This is a slide. What we do here is move clips within their own track, between the clips
      // that they're already next to. We don't allow changing tracks or changing the order of
      // blocks.
      //
      // For slides to be legal, we make all blocks "contiguous". This means that only one series
      // of blocks can move at a time and prevents.

      QHash<Track*, Block*> earliest_block_on_track;
      QHash<Track*, Block*> latest_block_on_track;

      foreach (Block* this_block, clips) {
        Block* current_earliest = earliest_block_on_track.value(this_block->track(), nullptr);
        if (!current_earliest || this_block->in() < current_earliest->in()) {
          earliest_block_on_track.insert(this_block->track(), this_block);
        }

        Block* current_latest = latest_block_on_track.value(this_block->track(), nullptr);
        if (!current_latest || this_block->out() > current_earliest->out()) {
          latest_block_on_track.insert(this_block->track(), this_block);
        }
      }

      for (auto i = earliest_block_on_track.constBegin(); i != earliest_block_on_track.constEnd(); i++) {
        // Make a contiguous stream
        Track* track = i.key();
        Block* earliest = i.value();
        Block* latest = latest_block_on_track.value(i.key());

        // First we add the block that's out trimming, the one prior to the earliest
        {
          TimelineViewGhostItem* earliest_ghost;
          bool slide_with_earliest_previous = true;
          if (sliding_due_to_transition && earliest->previous()) {
            if (auto* transit = dynamic_cast<TransitionBlock*>(earliest)) {
              if (earliest->previous() != transit->connected_out_block()) {
                slide_with_earliest_previous = false;
              }
            } else if (auto* clip = dynamic_cast<ClipBlock*>(earliest)) {
              if (earliest->previous() != clip->in_transition()) {
                slide_with_earliest_previous = false;
              }
            }
          }

          if (earliest->previous() && slide_with_earliest_previous) {
            earliest_ghost = AddGhostFromBlock(earliest->previous(), Timeline::kTrimOut);
          } else {
            earliest_ghost = AddGhostFromNull(earliest->in(), earliest->in(), track->ToReference(), Timeline::kTrimOut);
          }
          SetGhostToSlideMode(earliest_ghost);
        }

        // Then we add the block that's in trimming, the one after the latest
        if (latest->next()) {
          TimelineViewGhostItem* latest_ghost;

          bool slide_with_latest_next = true;
          if (sliding_due_to_transition) {
            if (auto* transit = dynamic_cast<TransitionBlock*>(latest)) {
              if (latest->next() != transit->connected_in_block()) {
                slide_with_latest_next = false;
              }
            } else if (auto* clip = dynamic_cast<ClipBlock*>(latest)) {
              if (latest->next() != clip->out_transition()) {
                slide_with_latest_next = false;
              }
            }
          }

          if (slide_with_latest_next) {
            latest_ghost = AddGhostFromBlock(latest->next(), Timeline::kTrimIn);
          } else {
            latest_ghost = AddGhostFromNull(latest->out(), latest->out(), track->ToReference(), Timeline::kTrimIn);
          }
          SetGhostToSlideMode(latest_ghost);
        }

        // Finally, we add all of the moving blocks in between
        Block* b = nullptr;
        do {
          // On first run-through, set to earliest only. From then on, set to the next of the last
          // in the loop.
          if (b) {
            b = b->next();
          } else {
            b = earliest;
          }

          TimelineViewGhostItem* between_ghost = AddGhostFromBlock(b, Timeline::kMove);
          SetGhostToSlideMode(between_ghost);
        } while (b != latest);
      }
    } else {
      // Prepare for a standard pointer move by creating ghosts for them and any related blocks
      foreach (Block* block, clips) {
        if (dynamic_cast<GapBlock*>(block)) {
          continue;
        }

        // Create ghost for this block
        auto ghost = AddGhostFromBlock(block, trim_mode, true);
        Q_UNUSED(ghost)

        if (auto* clip = dynamic_cast<ClipBlock*>(block)) {
          if (clip->out_transition()) {
            AddGhostFromBlock(clip->out_transition(), trim_mode, true);
          }
          if (clip->in_transition()) {
            AddGhostFromBlock(clip->in_transition(), trim_mode, true);
          }
        }
      }
    }

  } else {
    // "Multi-trim" is trimming a clip on more than one track. Only the earliest (for in trimming)
    // or latest (for out trimming) clip on each track can be trimmed. Therefore, it's only enabled
    // if the clicked item is the earliest/latest on its track.
    bool multitrim_enabled = IsClipTrimmable(clicked_item, clips, trim_mode);

    // Create ghosts for trimming
    for (Block* clip_item : clips) {
      if (clip_item != clicked_item && (!multitrim_enabled || !IsClipTrimmable(clip_item, clips, trim_mode))) {
        // Either multitrim is disabled or this clip is NOT the earliest/latest in its track. We
        // won't include it.
        continue;
      }

      Block* block = clip_item;

      // Create ghost for this block
      TimelineViewGhostItem* ghost = AddGhostFromBlock(block, trim_mode);

      // If this side of the clip has a transition, we treat it more like a slide for that
      // transition than a trim/roll
      bool treat_trim_as_slide = false;

      auto* cb = dynamic_cast<ClipBlock*>(block);
      if (cb) {
        // See if this clip has a transition attached, and move it with the trim if so
        TransitionBlock* connected_transition;

        // Get appropriate transition for the side of the clip
        if (trim_mode == Timeline::kTrimIn) {
          connected_transition = cb->in_transition();
        } else {
          connected_transition = cb->out_transition();
        }

        if (connected_transition) {
          // We found a transition, we'll make this a "slide" action
          TimelineViewGhostItem* transition_ghost = AddGhostFromBlock(connected_transition, Timeline::kMove);

          // This will in effect be a slide with the transition moving between two other blocks
          SetGhostToSlideMode(ghost);
          SetGhostToSlideMode(transition_ghost);
          treat_trim_as_slide = true;

          // Further processing will apply to this transition rather than the clip
          block = connected_transition;
        }
      }

      // Standard pointer trimming in reality is a "roll" edit with an adjacent gap (one that may
      // or may not exist already)
      if (!dont_roll_trims) {
        Block* adjacent = nullptr;

        // Determine which block is adjacent
        if (trim_mode == Timeline::kTrimIn) {
          adjacent = block->previous();
        } else {
          adjacent = block->next();
        }

        // See if we can roll the adjacent or if we'll need to create our own gap
        if (!dynamic_cast<GapBlock*>(block) && !allow_nongap_rolling && adjacent &&
            !dynamic_cast<GapBlock*>(adjacent) &&
            !(dynamic_cast<TransitionBlock*>(block) &&
              ((trim_mode == Timeline::kTrimIn &&
                dynamic_cast<TransitionBlock*>(block)->connected_out_block() == adjacent) ||
               (trim_mode == Timeline::kTrimOut &&
                dynamic_cast<TransitionBlock*>(block)->connected_in_block() == adjacent)))) {
          adjacent = nullptr;
        }

        Timeline::MovementMode flipped_mode = FlipTrimMode(trim_mode);
        QVector<TimelineViewGhostItem*> adjacent_ghosts;

        if (adjacent) {
          adjacent_ghosts.append(AddGhostFromBlock(adjacent, flipped_mode));

          // Select adjacent's links if applicable
          // FIXME: The check for `clips.size() == 1` may not be necessary, but I don't know yet.
          //        I'm only including it to prevent any potentially unintended behavior.
          if (clips.size() == 1 && !(modifiers & Qt::AltModifier)) {
            if (auto* adjacent_clip = dynamic_cast<ClipBlock*>(adjacent)) {
              for (Block* adjacent_link : adjacent_clip->block_links()) {
                adjacent_ghosts.append(AddGhostFromBlock(adjacent_link, flipped_mode));
              }
            }
          }
        } else if (trim_mode == Timeline::kTrimIn || block->next()) {
          rational null_ghost_pos = (trim_mode == Timeline::kTrimIn) ? block->in() : block->out();

          adjacent_ghosts.append(
              AddGhostFromNull(null_ghost_pos, null_ghost_pos, clip_item->track()->ToReference(), flipped_mode));
        }

        // If we have an adjacent block (for any reason), this is a roll edit and the adjacent is
        // expected to fill the remaining space (no gap needs to be created)
        ghost->SetData(TimelineViewGhostItem::kTrimIsARollEdit, static_cast<bool>(adjacent));

        for (TimelineViewGhostItem* adjacent_ghost : adjacent_ghosts) {
          if (adjacent_ghost) {
            if (treat_trim_as_slide) {
              // We're sliding a transition rather than a pure trim/roll
              SetGhostToSlideMode(adjacent_ghost);
            } else if (dynamic_cast<GapBlock*>(block)) {
              ghost->SetData(TimelineViewGhostItem::kTrimShouldBeIgnored, true);
            } else {
              adjacent_ghost->SetData(TimelineViewGhostItem::kTrimShouldBeIgnored, true);
            }
          }
        }
      }
    }
  }
}

bool PointerTool::CanTransitionMove(TransitionBlock* transit, const QVector<Block*>& clips) {
  Block* out = transit->connected_out_block();
  Block* in = transit->connected_in_block();

  if ((out && !clips.contains(out)) || (in && !clips.contains(in))) {
    return false;
  }

  return true;
}

void PointerTool::ProcessDrag(const TimelineCoordinate& mouse_pos) {
  // Calculate track movement
  int track_movement = track_movement_allowed_ ? mouse_pos.GetTrack().index() - drag_start_.GetTrack().index() : 0;

  // Determine frame movement
  rational time_movement = mouse_pos.GetFrame() - drag_start_.GetFrame();

  // Validate movement (enforce all ghosts moving in legal ways)
  time_movement = ValidateTimeMovement(time_movement);
  time_movement = ValidateInTrimming(time_movement);
  time_movement = ValidateOutTrimming(time_movement);

  // Perform snapping if enabled (adjusts time_movement if it's close to any potential snap points)
  if (Core::instance()->snapping()) {
    parent()->SnapPoint(snap_points_, &time_movement);

    time_movement = ValidateTimeMovement(time_movement);
    time_movement = ValidateInTrimming(time_movement);
    time_movement = ValidateOutTrimming(time_movement);
  }

  // Validate ghosts that are being moved (clips from other track types do NOT get moved)
  if (track_movement != 0) {
    QVector<TimelineViewGhostItem*> validate_track_ghosts = parent()->GetGhostItems();
    for (int i = 0; i < validate_track_ghosts.size(); i++) {
      if (validate_track_ghosts.at(i)->GetTrack().type() != drag_track_type_) {
        validate_track_ghosts.removeAt(i);
        i--;
      }
    }
    track_movement = ValidateTrackMovement(track_movement, validate_track_ghosts);
  }

  // Perform movement
  foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
    switch (ghost->GetMode()) {
      case Timeline::kNone:
        break;
      case Timeline::kTrimIn:
        ghost->SetInAdjustment(time_movement);
        ghost->SetMediaInAdjustment(time_movement);
        break;
      case Timeline::kTrimOut:
        ghost->SetOutAdjustment(time_movement);
        break;
      case Timeline::kMove: {
        ghost->SetInAdjustment(time_movement);
        ghost->SetOutAdjustment(time_movement);

        // Track movement is only legal for moving, not for trimming
        // Also, we only move the clips on the same track type that the drag started from
        if (ghost->GetTrack().type() == drag_track_type_) {
          ghost->SetTrackAdjustment(track_movement);
        }
        break;
      }
    }
  }

  // Regenerate tooltip and force it to update (otherwise the tooltip won't move as written in the
  // documentation, and could get in the way of the cursor)
  rational tooltip_timebase = parent()->GetTimebaseForTrackType(drag_start_.GetTrack().type());
  QToolTip::hideText();
  QToolTip::showText(QCursor::pos(),
                     QString::fromStdString(Timecode::time_to_timecode(time_movement, tooltip_timebase,
                                                                       Core::instance()->GetTimecodeDisplay(), true)),
                     parent());
}

struct GhostBlockPair {
  TimelineViewGhostItem* ghost;
  Block* block;
};

void PointerTool::FinishDrag(TimelineViewMouseEvent* event) {
  QList<GhostBlockPair> blocks_moving;
  QList<GhostBlockPair> blocks_sliding;
  QList<GhostBlockPair> blocks_trimming;

  // Sort ghosts depending on which ones are trimming, which are moving, and which are sliding
  foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
    if (ghost->HasBeenAdjusted()) {
      auto* b = QtUtils::ValueToPtr<Block>(ghost->GetData(TimelineViewGhostItem::kAttachedBlock));

      if (ghost->GetData(TimelineViewGhostItem::kGhostIsSliding).toBool()) {
        blocks_sliding.append({ghost, b});
      } else if (ghost->GetMode() == Timeline::kMove) {
        blocks_moving.append({ghost, b});
      } else if (Timeline::IsATrimMode(ghost->GetMode())) {
        blocks_trimming.append({ghost, b});
      }
    }
  }

  if (blocks_moving.isEmpty() && blocks_trimming.isEmpty() && blocks_sliding.isEmpty()) {
    // No blocks were adjusted, so nothing to do
    return;
  }

  auto* command = new MultiUndoCommand();

  if (!blocks_trimming.isEmpty()) {
    foreach (const GhostBlockPair& p, blocks_trimming) {
      TimelineViewGhostItem* ghost = p.ghost;

      if (!ghost->GetData(TimelineViewGhostItem::kTrimShouldBeIgnored).toBool()) {
        // Must be an ordinary trim/roll
        auto* c = new BlockTrimCommand(parent()->GetTrackFromReference(ghost->GetAdjustedTrack()), p.block,
                                       ghost->GetAdjustedLength(), ghost->GetMode());

        if (event->GetModifiers() & Qt::ControlModifier) {
        }

        c->SetTrimIsARollEdit(ghost->GetData(TimelineViewGhostItem::kTrimIsARollEdit).toBool());

        command->add_child(c);
      }
    }

    if (blocks_moving.isEmpty() && blocks_sliding.isEmpty()) {
      // Trim selections (deferring to moving/sliding blocks when necessary)
      TimelineWidgetSelections new_sel = parent()->GetSelections();
      TimelineViewGhostItem* reference_ghost = blocks_trimming.first().ghost;
      if (reference_ghost->GetMode() == Timeline::kTrimIn) {
        new_sel.TrimIn(reference_ghost->GetInAdjustment());
      } else {
        new_sel.TrimOut(reference_ghost->GetOutAdjustment());
      }
      command->add_child(new TimelineWidget::SetSelectionsCommand(parent(), new_sel, parent()->GetSelections()));
    }
  }

  if (!blocks_moving.isEmpty()) {
    // See if we're duplicated because ALT is held (only moved blocks can duplicate)
    bool duplicate_clips = (event->GetModifiers() & Qt::AltModifier);
    bool inserting = (event->GetModifiers() & Qt::ControlModifier);

    // If we're not duplicating, "remove" the clips and replace them with gaps
    if (!duplicate_clips) {
      QVector<Block*> blocks_to_delete(blocks_moving.size());

      for (int i = 0; i < blocks_moving.size(); i++) {
        blocks_to_delete[i] = blocks_moving.at(i).block;
      }

      parent()->ReplaceBlocksWithGaps(blocks_to_delete, false, command, false);
    }

    if (inserting) {
      // If we're inserting, ripple everything at the destination with gaps
      InsertGapsAtGhostDestination(command);
    }

    QMap<Node*, Node*> relinks;

    // Now we can re-add each clip
    foreach (const GhostBlockPair& p, blocks_moving) {
      Block* block = p.block;

      if (duplicate_clips) {
        // Duplicate rather than move
        // Place the copy instead of the original block
        auto* new_block = dynamic_cast<Block*>(Node::CopyNodeInGraph(block, command));
        relinks.insert(block, new_block);
        block = new_block;

        if (auto* new_clip = dynamic_cast<ClipBlock*>(block)) {
          new_clip->AddCachePassthroughFrom(dynamic_cast<ClipBlock*>(p.block));
        }
      }

      const Track::Reference& track_ref = p.ghost->GetAdjustedTrack();
      command->add_child(new TrackPlaceBlockCommand(sequence()->track_list(track_ref.type()), track_ref.index(), block,
                                                    p.ghost->GetAdjustedIn()));
    }

    if (!relinks.empty()) {
      for (auto it = relinks.cbegin(); it != relinks.cend(); it++) {
        // Re-connect links on duplicate clips
        for (auto jt = it.key()->links().cbegin(); jt != it.key()->links().cend(); jt++) {
          Node* link = *jt;
          Node* copy_link = relinks.value(link);
          if (copy_link) {
            command->add_child(new NodeLinkCommand(it.value(), copy_link, true));
          }
        }

        // Re-connect transitions where applicable
        if (auto* og_clip = dynamic_cast<ClipBlock*>(it.key())) {
          auto* cp_clip = dynamic_cast<ClipBlock*>(it.value());

          TransitionBlock* og_in_transition = og_clip->in_transition();
          TransitionBlock* og_out_transition = og_clip->out_transition();

          if (og_in_transition && relinks.contains(og_in_transition)) {
            TransitionBlock* cp_in_transition = dynamic_cast<TransitionBlock*>(relinks.value(og_in_transition));
            command->add_child(
                new NodeEdgeAddCommand(cp_clip, NodeInput(cp_in_transition, TransitionBlock::kInBlockInput)));
          }

          if (og_out_transition && relinks.contains(og_out_transition)) {
            TransitionBlock* cp_out_transition = dynamic_cast<TransitionBlock*>(relinks.value(og_out_transition));
            command->add_child(
                new NodeEdgeAddCommand(cp_clip, NodeInput(cp_out_transition, TransitionBlock::kOutBlockInput)));
          }
        }
      }
    }

    // Adjust selections
    TimelineWidgetSelections new_sel = parent()->GetSelections();
    new_sel.ShiftTime(blocks_moving.first().ghost->GetInAdjustment());
    new_sel.ShiftTracks(drag_track_type_, blocks_moving.first().ghost->GetTrackAdjustment());
    command->add_child(new TimelineWidget::SetSelectionsCommand(parent(), new_sel, parent()->GetSelections()));
  }

  if (!blocks_sliding.isEmpty()) {
    // Assume that the blocks are contiguous per track as set up in InitiateGhostsInternal()

    // All we need to do is sort them by track and order them
    QHash<Track::Reference, QList<Block*> > slide_info;
    QHash<Track::Reference, Block*> in_adjacents;
    QHash<Track::Reference, Block*> out_adjacents;
    rational movement;

    foreach (const GhostBlockPair& p, blocks_sliding) {
      const Track::Reference& track = p.ghost->GetTrack();

      switch (p.ghost->GetMode()) {
        case Timeline::kNone:
          break;
        case Timeline::kMove: {
          // These all should have moved uniformly, so as long as this is set, it should be fine
          movement = p.ghost->GetInAdjustment();

          QList<Block*>& blocks_on_this_track = slide_info[track];
          bool inserted = false;

          for (int i = 0; i < blocks_on_this_track.size(); i++) {
            if (blocks_on_this_track.at(i)->in() > p.block->in()) {
              blocks_on_this_track.insert(i, p.block);
              inserted = true;
              break;
            }
          }

          if (!inserted) {
            blocks_on_this_track.append(p.block);
          }
          break;
        }
        case Timeline::kTrimIn:
          out_adjacents.insert(track, p.block);
          break;
        case Timeline::kTrimOut:
          in_adjacents.insert(track, p.block);
          break;
      }
    }

    if (!movement.isNull()) {
      for (auto i = slide_info.constBegin(); i != slide_info.constEnd(); i++) {
        command->add_child(new TrackSlideCommand(parent()->GetTrackFromReference(i.key()), i.value(),
                                                 in_adjacents.value(i.key()), out_adjacents.value(i.key()), movement));
      }

      // Adjust selections
      TimelineWidgetSelections new_sel = parent()->GetSelections();
      new_sel.ShiftTime(movement);
      command->add_child(new TimelineWidget::SetSelectionsCommand(parent(), new_sel, parent()->GetSelections()));
    }
  }

  Core::instance()->undo_stack()->push(command, qApp->translate("PointerTool", "Moved Clips"));
}

Timeline::MovementMode PointerTool::IsCursorInTrimHandle(Block* block, qreal cursor_x) {
  const double kTrimHandle = QtUtils::QFontMetricsWidth(parent()->fontMetrics(), "H");

  double block_left = parent()->TimeToScene(block->in());
  double block_right = parent()->TimeToScene(block->out());
  double block_width = block_right - block_left;

  // Block is too narrow, no trimming allowed
  if (block_width <= kTrimHandle * 2) {
    return Timeline::kNone;
  }

  if (trimming_allowed_ && cursor_x <= block_left + kTrimHandle) {
    return Timeline::kTrimIn;
  } else if (trimming_allowed_ && cursor_x >= block_right - kTrimHandle) {
    return Timeline::kTrimOut;
  } else {
    return Timeline::kNone;
  }
}

void PointerTool::InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers) {
  InitiateDragInternal(clicked_item, trim_mode, modifiers, false, false, false);
}

TimelineViewGhostItem* PointerTool::GetExistingGhostFromBlock(Block* block) {
  foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
    if (QtUtils::ValueToPtr<Block>(ghost->GetData(TimelineViewGhostItem::kAttachedBlock)) == block) {
      return ghost;
    }
  }

  return nullptr;
}

// #define HIDE_GAP_GHOSTS

TimelineViewGhostItem* PointerTool::AddGhostFromBlock(Block* block, Timeline::MovementMode mode, bool check_if_exists) {
  // Ignore null blocks or blocks that aren't attached to a track because there's nothing we can
  // do with either of those
  if (!block || !block->track()) {
    return nullptr;
  }

  TimelineViewGhostItem* ghost;

  // Check if we've already made a ghost for this block
  if (check_if_exists) {
    if ((ghost = GetExistingGhostFromBlock(block))) {
      return ghost;
    }
  }

  // Otherwise, it's time to make a ghost for this block
  ghost = TimelineViewGhostItem::FromBlock(block);

#ifdef HIDE_GAP_GHOSTS
  if (block->type() == Block::kGap) {
    ghost->SetInvisible(true);
  }
#endif

  AddGhostInternal(ghost, mode);

  return ghost;
}

TimelineViewGhostItem* PointerTool::AddGhostFromNull(const rational& in, const rational& out,
                                                     const Track::Reference& track, Timeline::MovementMode mode) {
  auto* ghost = new TimelineViewGhostItem();

  ghost->SetIn(in);
  ghost->SetOut(out);
  ghost->SetTrack(track);

#ifdef HIDE_GAP_GHOSTS
  ghost->SetInvisible(true);
#endif

  AddGhostInternal(ghost, mode);

  return ghost;
}

void PointerTool::AddGhostInternal(TimelineViewGhostItem* ghost, Timeline::MovementMode mode) {
  ghost->SetMode(mode);

  // Prepare snap points (optimizes snapping for later)
  switch (mode) {
    case Timeline::kMove:
      snap_points_.push_back(ghost->GetIn());
      snap_points_.push_back(ghost->GetOut());
      break;
    case Timeline::kTrimIn:
      snap_points_.push_back(ghost->GetIn());
      break;
    case Timeline::kTrimOut:
      snap_points_.push_back(ghost->GetOut());
      break;
    default:
      break;
  }

  parent()->AddGhost(ghost);
}

bool PointerTool::IsClipTrimmable(Block* clip, const QVector<Block*>& items, const Timeline::MovementMode& mode) {
  foreach (Block* compare, items) {
    if (clip->track() == compare->track() && clip != compare &&
        ((compare->in() < clip->in() && mode == Timeline::kTrimIn) ||
         (compare->out() > clip->out() && mode == Timeline::kTrimOut))) {
      return false;
    }
  }

  return true;
}

rational PointerTool::ValidateInTrimming(rational movement) {
  bool first_ghost = true;

  foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
    if (ghost->GetMode() != Timeline::kTrimIn) {
      continue;
    }

    rational earliest_in = RATIONAL_MIN;
    rational latest_in = ghost->GetOut();

    rational ghost_timebase = parent()->GetTimebaseForTrackType(ghost->GetTrack().type());

    // If the ghost must be at least one frame in size, limit the latest allowed in point
    if (!ghost->CanHaveZeroLength()) {
      latest_in -= ghost_timebase;
    }

    // Clamp adjusted value between the earliest and latest values
    rational adjusted = ghost->GetIn() + movement;
    rational clamped = std::clamp(adjusted, earliest_in, latest_in);

    if (clamped != adjusted) {
      movement = clamped - ghost->GetIn();
    }

    if (first_ghost) {
      movement = SnapMovementToTimebase(ghost->GetIn(), movement, ghost_timebase);
      first_ghost = false;
    }
  }

  return movement;
}

rational PointerTool::ValidateOutTrimming(rational movement) {
  bool first_ghost = true;

  foreach (TimelineViewGhostItem* ghost, parent()->GetGhostItems()) {
    if (ghost->GetMode() != Timeline::kTrimOut) {
      continue;
    }

    // Determine earliest and latest out points
    rational earliest_out = ghost->GetIn();

    rational ghost_timebase = parent()->GetTimebaseForTrackType(ghost->GetTrack().type());

    if (!ghost->CanHaveZeroLength()) {
      earliest_out += ghost_timebase;
    }

    rational latest_out = RATIONAL_MAX;

    // Clamp adjusted value between the earliest and latest values
    rational adjusted = ghost->GetOut() + movement;
    rational clamped = std::clamp(adjusted, earliest_out, latest_out);

    if (clamped != adjusted) {
      movement = clamped - ghost->GetOut();
    }

    if (first_ghost) {
      movement = SnapMovementToTimebase(ghost->GetOut(), movement, ghost_timebase);
      first_ghost = false;
    }
  }

  return movement;
}

}  // namespace olive
