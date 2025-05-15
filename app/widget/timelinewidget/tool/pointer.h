#ifndef POINTERTIMELINETOOL_H
#define POINTERTIMELINETOOL_H

#include "tool.h"

namespace olive {

class PointerTool : public TimelineTool {
 public:
  explicit PointerTool(TimelineWidget* parent);

  void MousePress(TimelineViewMouseEvent* event) override;
  void MouseMove(TimelineViewMouseEvent* event) override;
  void MouseRelease(TimelineViewMouseEvent* event) override;

  void HoverMove(TimelineViewMouseEvent* event) override;

 protected:
  virtual void FinishDrag(TimelineViewMouseEvent* event);

  virtual void InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers);

  TimelineViewGhostItem* GetExistingGhostFromBlock(Block* block);

  TimelineViewGhostItem* AddGhostFromBlock(Block* block, Timeline::MovementMode mode, bool check_if_exists = false);

  TimelineViewGhostItem* AddGhostFromNull(const rational& in, const rational& out, const Track::Reference& track,
                                          Timeline::MovementMode mode);

  /**
   * @brief Validates Ghosts that are getting their in points trimmed
   *
   * Assumes ghost->data() is a Block. Ensures no Ghost's in point becomes a negative timecode. Also ensures no
   * Ghost's length becomes 0 or negative.
   */
  rational ValidateInTrimming(rational movement);

  /**
   * @brief Validates Ghosts that are getting their out points trimmed
   *
   * Assumes ghost->data() is a Block. Ensures no Ghost's in point becomes a negative timecode. Also ensures no
   * Ghost's length becomes 0 or negative.
   */
  rational ValidateOutTrimming(rational movement);

  virtual void ProcessDrag(const TimelineCoordinate& mouse_pos);

  void InitiateDragInternal(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers,
                            bool dont_roll_trims, bool allow_nongap_rolling, bool slide_instead_of_moving);

  [[nodiscard]] const Timeline::MovementMode& drag_movement_mode() const { return drag_movement_mode_; }
  void set_drag_movement_mode(const Timeline::MovementMode& d) { drag_movement_mode_ = d; }

  static bool CanTransitionMove(TransitionBlock* transit, const QVector<Block*>& clips);

  void SetMovementAllowed(bool e) { movement_allowed_ = e; }

  void SetTrimmingAllowed(bool e) { trimming_allowed_ = e; }

  void SetTrackMovementAllowed(bool e) { track_movement_allowed_ = e; }

  void SetGapTrimmingAllowed(bool e) { gap_trimming_allowed_ = e; }

  void SetClickedItem(Block* b) { clicked_item_ = b; }

 private:
  Timeline::MovementMode IsCursorInTrimHandle(Block* block, qreal cursor_x);

  void AddGhostInternal(TimelineViewGhostItem* ghost, Timeline::MovementMode mode);

  static bool IsClipTrimmable(Block* clip, const QVector<Block*>& items, const Timeline::MovementMode& mode);

  void ProcessGhostsForSliding();

  void ProcessGhostsForRolling();

  bool movement_allowed_;
  bool trimming_allowed_;
  bool track_movement_allowed_;
  bool gap_trimming_allowed_;
  bool can_rubberband_select_;
  bool rubberband_selecting_;

  Track::Type drag_track_type_;
  Timeline::MovementMode drag_movement_mode_;

  Block* clicked_item_{};

  QPoint drag_global_start_;
};

}  // namespace olive

#endif  // POINTERTIMELINETOOL_H
