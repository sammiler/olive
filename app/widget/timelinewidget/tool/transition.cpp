#include "widget/timelinewidget/timelinewidget.h"

#include "node/block/transition/crossdissolve/crossdissolvetransition.h"
#include "node/block/transition/transition.h"
#include "node/factory.h"
#include "node/nodeundo.h"
#include "timeline/timelineundopointer.h"
#include "transition.h"

namespace olive {

TransitionTool::TransitionTool(TimelineWidget *parent) : AddTool(parent) {}

void TransitionTool::HoverMove(TimelineViewMouseEvent *event) {
  ClipBlock *primary = nullptr;
  ClipBlock *secondary = nullptr;
  Timeline::MovementMode trim_mode = Timeline::kNone;
  rational transition_start_point;

  GetBlocksAtCoord(event->GetCoordinates(), &primary, &secondary, &trim_mode, &transition_start_point);

  if (trim_mode == Timeline::kTrimIn) {
    std::swap(primary, secondary);
  }

  parent()->SetViewTransitionOverlay(primary, secondary);
}

void TransitionTool::MousePress(TimelineViewMouseEvent *event) {
  ClipBlock *primary, *secondary;
  Timeline::MovementMode trim_mode;
  rational transition_start_point;
  if (!GetBlocksAtCoord(event->GetCoordinates(), &primary, &secondary, &trim_mode, &transition_start_point)) {
    return;
  }

  // Create ghost
  ghost_ = new TimelineViewGhostItem();
  ghost_->SetTrack(event->GetTrack());
  ghost_->SetIn(transition_start_point);
  ghost_->SetOut(transition_start_point);
  ghost_->SetMode(trim_mode);
  ghost_->SetData(TimelineViewGhostItem::kAttachedBlock, QtUtils::PtrToValue(primary));

  dual_transition_ = (secondary);
  if (secondary) ghost_->SetData(TimelineViewGhostItem::kReferenceBlock, QtUtils::PtrToValue(secondary));

  parent()->AddGhost(ghost_);

  snap_points_.push_back(transition_start_point);

  // Set the drag start point
  drag_start_point_ = event->GetFrame();
}

void TransitionTool::MouseMove(TimelineViewMouseEvent *event) {
  if (!ghost_) {
    return;
  }

  MouseMoveInternal(event->GetFrame(), dual_transition_);
}

void TransitionTool::MouseRelease(TimelineViewMouseEvent *event) {
  const Track::Reference &track = ghost_->GetTrack();

  if (ghost_) {
    if (!ghost_->GetAdjustedLength().isNull()) {
      TransitionBlock *transition;

      if (Core::instance()->GetSelectedTransition().isEmpty()) {
        // Fallback if the user hasn't selected one yet
        transition = new CrossDissolveTransition();
      } else {
        transition =
            dynamic_cast<TransitionBlock *>(NodeFactory::CreateFromID(Core::instance()->GetSelectedTransition()));
      }

      // Set transition length
      rational len = ghost_->GetAdjustedLength();
      transition->set_length_and_media_out(len);

      auto *command = new MultiUndoCommand();

      // Place transition in place
      command->add_child(new NodeAddCommand(parent()->GetConnectedNode()->parent(), transition));

      command->add_child(new NodeSetPositionCommand(transition, transition, Node::Position(QPointF(0, 0))));

      command->add_child(new TrackPlaceBlockCommand(sequence()->track_list(track.type()), track.index(), transition,
                                                    ghost_->GetAdjustedIn()));

      if (dual_transition_) {
        // Block mouse is hovering over
        auto *active_block = QtUtils::ValueToPtr<Block>(ghost_->GetData(TimelineViewGhostItem::kAttachedBlock));

        // Block mouse is next to
        auto *friend_block = QtUtils::ValueToPtr<Block>(ghost_->GetData(TimelineViewGhostItem::kReferenceBlock));

        // Use ghost mode to determine which block is which
        Block *out_block = (ghost_->GetMode() == Timeline::kTrimIn) ? friend_block : active_block;
        Block *in_block = (ghost_->GetMode() == Timeline::kTrimIn) ? active_block : friend_block;

        // Connect block to transition
        command->add_child(new NodeEdgeAddCommand(out_block, NodeInput(transition, TransitionBlock::kOutBlockInput)));

        command->add_child(new NodeEdgeAddCommand(in_block, NodeInput(transition, TransitionBlock::kInBlockInput)));

        command->add_child(new NodeSetPositionCommand(out_block, transition, Node::Position(QPointF(-1, -0.5))));
        command->add_child(new NodeSetPositionCommand(in_block, transition, Node::Position(QPointF(-1, 0.5))));
      } else {
        auto *block_to_transition = QtUtils::ValueToPtr<Block>(ghost_->GetData(TimelineViewGhostItem::kAttachedBlock));
        QString transition_input_to_connect;

        if (ghost_->GetMode() == Timeline::kTrimIn) {
          transition_input_to_connect = TransitionBlock::kInBlockInput;
        } else {
          transition_input_to_connect = TransitionBlock::kOutBlockInput;
        }

        // Connect block to transition
        command->add_child(
            new NodeEdgeAddCommand(block_to_transition, NodeInput(transition, transition_input_to_connect)));

        command->add_child(new NodeSetPositionCommand(block_to_transition, transition, Node::Position(QPointF(-1, 0))));
      }

      Core::instance()->undo_stack()->push(command, qApp->translate("TransitionTool", "Created Transition"));

      parent()->SetViewTransitionOverlay(nullptr, nullptr);
    }

    parent()->ClearGhosts();
    snap_points_.clear();
    ghost_ = nullptr;
  }
}

bool TransitionTool::GetBlocksAtCoord(const TimelineCoordinate &coord, ClipBlock **primary, ClipBlock **secondary,
                                      Timeline::MovementMode *ptrim_mode, rational *start_point) {
  const Track::Reference &track = coord.GetTrack();
  Track *t = parent()->GetTrackFromReference(track);
  rational cursor_frame = coord.GetFrame();

  if (!t || t->IsLocked()) {
    return false;
  }

  Block *block_at_time = t->NearestBlockBeforeOrAt(coord.GetFrame());
  if (!dynamic_cast<ClipBlock *>(block_at_time)) {
    return false;
  }

  // Determine which side of the clip the transition belongs to
  rational transition_start_point;
  Timeline::MovementMode trim_mode;
  rational tenth_point = block_at_time->length() / rational(10);
  Block *other_block = nullptr;
  if (cursor_frame < (block_at_time->in() + block_at_time->length() / rational(2))) {
    if (dynamic_cast<ClipBlock *>(block_at_time)->in_transition()) {
      // This clip already has a transition here
      return false;
    }

    auto *adjacent = dynamic_cast<ClipBlock *>(block_at_time->previous());
    if (adjacent) {
      tenth_point = std::min(tenth_point, adjacent->length() / rational(10));
    }

    transition_start_point = block_at_time->in();
    trim_mode = Timeline::kTrimIn;

    if (cursor_frame < (block_at_time->in() + tenth_point) && adjacent) {
      other_block = adjacent;
    }
  } else {
    if (dynamic_cast<ClipBlock *>(block_at_time)->out_transition()) {
      // This clip already has a transition here
      return false;
    }

    auto *adjacent = dynamic_cast<ClipBlock *>(block_at_time->next());
    if (adjacent) {
      tenth_point = std::min(tenth_point, adjacent->length() / rational(10));
    }

    transition_start_point = block_at_time->out();
    trim_mode = Timeline::kTrimOut;

    if (cursor_frame > block_at_time->out() - tenth_point && adjacent) {
      other_block = block_at_time->next();
    }
  }

  *primary = dynamic_cast<ClipBlock *>(block_at_time);
  *secondary = dynamic_cast<ClipBlock *>(other_block);
  *ptrim_mode = trim_mode;
  *start_point = transition_start_point;

  return true;
}

}  // namespace olive
