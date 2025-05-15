

#include "draggable.h"

namespace olive {

DraggableGizmo::DraggableGizmo(QObject *parent) : NodeGizmo{parent}, drag_value_behavior_(kAbsolute) {}

void DraggableGizmo::DragStart(const NodeValueRow &row, double abs_x, double abs_y, const rational &time) {
  for (int i = 0; i < draggers_.size(); i++) {
    draggers_[i].Start(inputs_[i], time);
  }

  emit HandleStart(row, abs_x, abs_y, time);
}

void DraggableGizmo::DragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) {
  emit HandleMovement(x, y, modifiers);
}

void DraggableGizmo::DragEnd(MultiUndoCommand *command) {
  for (auto &dragger : draggers_) {
    dragger.End(command);
  }
}

}  // namespace olive
