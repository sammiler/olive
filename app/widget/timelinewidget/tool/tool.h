

#ifndef TIMELINETOOL_H
#define TIMELINETOOL_H

#include <QDragLeaveEvent>

#include "widget/timelinewidget/view/timelineviewghostitem.h"
#include "widget/timelinewidget/view/timelineviewmouseevent.h"

namespace olive {

class TimelineWidget;

class TimelineTool {
 public:
  explicit TimelineTool(TimelineWidget *parent);
  virtual ~TimelineTool();

  virtual void MousePress(TimelineViewMouseEvent *) {}
  virtual void MouseMove(TimelineViewMouseEvent *) {}
  virtual void MouseRelease(TimelineViewMouseEvent *) {}
  virtual void MouseDoubleClick(TimelineViewMouseEvent *) {}

  virtual void HoverMove(TimelineViewMouseEvent *) {}

  virtual void DragEnter(TimelineViewMouseEvent *) {}
  virtual void DragMove(TimelineViewMouseEvent *) {}
  virtual void DragLeave(QDragLeaveEvent *) {}
  virtual void DragDrop(TimelineViewMouseEvent *) {}

  TimelineWidget *parent();

  Sequence *sequence();

  static Timeline::MovementMode FlipTrimMode(const Timeline::MovementMode &trim_mode);

  static rational SnapMovementToTimebase(const rational &start, rational movement, const rational &timebase);

 protected:
  /**
   * @brief Validates Ghosts that are moving horizontally (time-based)
   *
   * Validation is the process of ensuring that whatever movements the user is making are "valid" and "legal". This
   * function's validation ensures that no Ghost's in point ends up in a negative timecode.
   */
  rational ValidateTimeMovement(rational movement);

  /**
   * @brief Validates Ghosts that are moving vertically (track-based)
   *
   * This function's validation ensures that no Ghost's track ends up in a negative (non-existent) track.
   */
  static int ValidateTrackMovement(int movement, const QVector<TimelineViewGhostItem *> &ghosts);

  void GetGhostData(rational *earliest_point, rational *latest_point);

  void InsertGapsAtGhostDestination(MultiUndoCommand *command);

  std::vector<rational> snap_points_;

  bool dragging_;

  TimelineCoordinate drag_start_;

  static const int kDefaultDistanceFromOutput;

 private:
  TimelineWidget *parent_;
};

}  // namespace olive

#endif  // TIMELINETOOL_H
