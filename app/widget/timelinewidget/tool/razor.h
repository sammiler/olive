#ifndef RAZORTIMELINETOOL_H
#define RAZORTIMELINETOOL_H

#include "beam.h"

namespace olive {

class RazorTool : public BeamTool {
 public:
  explicit RazorTool(TimelineWidget *parent);

  void MousePress(TimelineViewMouseEvent *event) override;
  void MouseMove(TimelineViewMouseEvent *event) override;
  void MouseRelease(TimelineViewMouseEvent *event) override;

 private:
  QVector<Track::Reference> split_tracks_;
};

}  // namespace olive

#endif  // RAZORTIMELINETOOL_H
