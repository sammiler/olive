

#include "text.h"

#include "core.h"
#include "undo/undocommand.h"

namespace olive {

TextGizmo::TextGizmo(QObject *parent) : NodeGizmo{parent}, valign_(Qt::AlignTop) {}

void TextGizmo::SetRect(const QRectF &r) {
  rect_ = r;
  emit RectChanged(rect_);
}

void TextGizmo::UpdateInputHtml(const QString &s, const rational &time) {
  if (input_.IsValid()) {
    auto *command = new MultiUndoCommand();
    Node::SetValueAtTime(input_.input(), time, s, input_.track(), command, true);
    Core::instance()->undo_stack()->push(command, tr("Edit Text"));
  }
}

void TextGizmo::SetVerticalAlignment(Qt::Alignment va) {
  valign_ = va;
  emit VerticalAlignmentChanged(valign_);
}

}  // namespace olive
