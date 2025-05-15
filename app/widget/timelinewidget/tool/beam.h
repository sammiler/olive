

#ifndef BEAMTIMELINETOOL_H
#define BEAMTIMELINETOOL_H

#include "tool.h"

namespace olive {

class BeamTool : public TimelineTool {
 public:
  explicit BeamTool(TimelineWidget *parent);

  void HoverMove(TimelineViewMouseEvent *event) override;

 protected:
  TimelineCoordinate ValidatedCoordinate(TimelineCoordinate coord);
};

}  // namespace olive

#endif  // BEAMTIMELINETOOL_H
