#ifndef DRAGGABLEGIZMO_H
#define DRAGGABLEGIZMO_H

#include "gizmo.h"
#include "node/inputdragger.h"
#include "undo/undocommand.h"

namespace olive {

class DraggableGizmo : public NodeGizmo {
  Q_OBJECT
 public:
  /// Changes what the X/Y coordinates emitted from HandleMovement specify
  enum DragValueBehavior {
    /// X/Y will be the exact mouse coordinates (in sequence pixels)
    kAbsolute,

    /// X/Y will be the movement since the last time HandleMovement was called
    kDeltaFromPrevious,

    /// X/Y will be the movement from the start of the drag
    kDeltaFromStart
  };

  explicit DraggableGizmo(QObject *parent = nullptr);

  void DragStart(const NodeValueRow &row, double abs_x, double abs_y, const olive::core::rational &time);

  void DragMove(double x, double y, const Qt::KeyboardModifiers &modifiers);

  void DragEnd(olive::MultiUndoCommand *command);

  void AddInput(const NodeKeyframeTrackReference &input) {
    inputs_.append(input);
    draggers_.append(NodeInputDragger());
  }

  QVector<NodeInputDragger> &GetDraggers() { return draggers_; }

  [[nodiscard]] DragValueBehavior GetDragValueBehavior() const { return drag_value_behavior_; }
  void SetDragValueBehavior(DragValueBehavior d) { drag_value_behavior_ = d; }

 signals:
  void HandleStart(const olive::NodeValueRow &row, double x, double y, const olive::core::rational &time);

  void HandleMovement(double x, double y, const Qt::KeyboardModifiers &modifiers);

 private:
  QVector<NodeKeyframeTrackReference> inputs_;

  QVector<NodeInputDragger> draggers_;

  DragValueBehavior drag_value_behavior_;
};

}  // namespace olive

#endif  // DRAGGABLEGIZMO_H
