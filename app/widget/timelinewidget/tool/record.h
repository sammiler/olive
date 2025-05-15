#ifndef RECORDTIMELINETOOL_H
#define RECORDTIMELINETOOL_H

#include "beam.h"

namespace olive {

class RecordTool : public BeamTool {
 public:
  explicit RecordTool(TimelineWidget* parent);

  void MousePress(TimelineViewMouseEvent* event) override;
  void MouseMove(TimelineViewMouseEvent* event) override;
  void MouseRelease(TimelineViewMouseEvent* event) override;

 protected:
  void MouseMoveInternal(const rational& cursor_frame, bool outwards);

  TimelineViewGhostItem* ghost_;

  rational drag_start_point_;
};

}  // namespace olive

#endif  // RECORDTOOL_H
