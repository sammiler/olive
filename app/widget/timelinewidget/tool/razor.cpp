#include "razor.h"

#include "node/nodeundo.h"
#include "timeline/timelineundosplit.h"
#include "widget/timelinewidget/timelinewidget.h"

namespace olive {

RazorTool::RazorTool(TimelineWidget* parent) : BeamTool(parent) {}

void RazorTool::MousePress(TimelineViewMouseEvent* event) {
  split_tracks_.clear();

  MouseMove(event);
}

void RazorTool::MouseMove(TimelineViewMouseEvent* event) {
  if (!dragging_) {
    drag_start_ = ValidatedCoordinate(event->GetCoordinates(true));
    dragging_ = true;
  }

  // Split at the current cursor track
  Track::Reference split_track = event->GetTrack();

  if (!split_tracks_.contains(split_track)) {
    split_tracks_.append(split_track);
  }
}

void RazorTool::MouseRelease(TimelineViewMouseEvent* event) {
  Q_UNUSED(event)

  // Always split at the same time
  rational split_time = drag_start_.GetFrame();

  QVector<Block*> blocks_to_split;

  foreach (const Track::Reference& track_ref, split_tracks_) {
    Track* track = parent()->GetTrackFromReference(track_ref);

    if (track == nullptr || track->IsLocked()) {
      continue;
    }

    Block* block_at_time = track->NearestBlockBefore(split_time);

    // Ensure there's a valid block here
    ClipBlock* clip_at_time;
    if (block_at_time && block_at_time->out() != split_time &&
        (clip_at_time = dynamic_cast<ClipBlock*>(block_at_time)) && !blocks_to_split.contains(block_at_time)) {
      blocks_to_split.append(block_at_time);

      // Add links if no alt is held
      if (!(event->GetModifiers() & Qt::AltModifier)) {
        foreach (Block* link, clip_at_time->block_links()) {
          if (!blocks_to_split.contains(link)) {
            blocks_to_split.append(link);
          }
        }
      }
    }
  }

  split_tracks_.clear();

  if (!blocks_to_split.isEmpty()) {
    Core::instance()->undo_stack()->push(new BlockSplitPreservingLinksCommand(blocks_to_split, {split_time}),
                                         qApp->translate("RazorTool", "Split Clips"));
  }

  dragging_ = false;
}

}  // namespace olive
