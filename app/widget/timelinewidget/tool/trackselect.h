#ifndef TRACKSELECTTOOL_H
#define TRACKSELECTTOOL_H

#include "pointer.h"

namespace olive {

class TrackSelectTool : public PointerTool {
 public:
  explicit TrackSelectTool(TimelineWidget *parent);

  void MousePress(TimelineViewMouseEvent *event) override;

 private:
  void SelectBlocksOnTrack(Track *track, TimelineViewMouseEvent *event, QVector<Block *> *blocks, bool forward);
};

}  // namespace olive

#endif  // TRACKSELECTTOOL_H
