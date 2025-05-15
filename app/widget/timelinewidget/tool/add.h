

#ifndef ADDTIMELINETOOL_H
#define ADDTIMELINETOOL_H

#include "beam.h"

namespace olive {

class AddTool : public BeamTool {
 public:
  explicit AddTool(TimelineWidget *parent);

  void MousePress(TimelineViewMouseEvent *event) override;
  void MouseMove(TimelineViewMouseEvent *event) override;
  void MouseRelease(TimelineViewMouseEvent *event) override;

  static Node *CreateAddableClip(MultiUndoCommand *command, Sequence *sequence, const Track::Reference &track,
                                 const rational &in, const rational &length, const QRectF &rect = QRectF());

 protected:
  void MouseMoveInternal(const rational &cursor_frame, bool outwards);

  TimelineViewGhostItem *ghost_;

  rational drag_start_point_;
};

}  // namespace olive

#endif  // ADDTIMELINETOOL_H
