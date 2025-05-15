#include "trackselect.h"

#include "node/block/gap/gap.h"
#include "node/output/track/track.h"
#include "widget/timelinewidget/timelinewidget.h"

namespace olive {

TrackSelectTool::TrackSelectTool(TimelineWidget *parent) : PointerTool(parent) {}

void TrackSelectTool::MousePress(TimelineViewMouseEvent *event) {
  QVector<Block *> blocks;
  bool forward = !(event->GetModifiers() & Qt::ControlModifier);

  parent()->DeselectAll();

  if (event->GetModifiers() & Qt::ShiftModifier) {
    // Track only
    Track *track = parent()->GetTrackFromReference(event->GetTrack());
    if (track) {
      SelectBlocksOnTrack(track, event, &blocks, forward);
    }
  } else {
    // All tracks
    foreach (Track *track, parent()->sequence()->GetTracks()) {
      SelectBlocksOnTrack(track, event, &blocks, forward);
    }
  }

  if (!blocks.isEmpty()) {
    parent()->SignalSelectedBlocks(blocks);
    set_drag_movement_mode(Timeline::kMove);
    SetClickedItem(blocks.first());
    drag_start_ = event->GetCoordinates();
  } else {
    set_drag_movement_mode(Timeline::kNone);
  }
}

void TrackSelectTool::SelectBlocksOnTrack(Track *track, TimelineViewMouseEvent *event, QVector<Block *> *blocks,
                                          bool forward) {
  Block *b = track->NearestBlockBeforeOrAt(event->GetFrame());

  if (!b && !track->Blocks().isEmpty() && !forward) {
    // Fallback to first or last block in track
    b = track->Blocks().last();
  }

  while (b) {
    if (!dynamic_cast<GapBlock *>(b)) {
      if (!blocks->contains(b)) {
        parent()->AddSelection(b);
        blocks->append(b);
      }

      if (!(event->GetModifiers() & Qt::AltModifier)) {
        if (auto *clip = dynamic_cast<ClipBlock *>(b)) {
          foreach (Block *link, clip->block_links()) {
            if (!blocks->contains(link)) {
              parent()->AddSelection(link);
              blocks->append(link);
            }
          }
        }
      }
    }

    b = forward ? b->next() : b->previous();
  }
}

}  // namespace olive
