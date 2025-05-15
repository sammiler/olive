

#include "focusablelineedit.h"

#include <QKeyEvent>

namespace olive {

FocusableLineEdit::FocusableLineEdit(QWidget *parent) : QLineEdit(parent) {}

void FocusableLineEdit::keyPressEvent(QKeyEvent *e) {
  switch (e->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
      emit Confirmed();
      break;
    case Qt::Key_Escape:
      emit Cancelled();
      break;
    default:
      QLineEdit::keyPressEvent(e);
  }
}

void FocusableLineEdit::focusOutEvent(QFocusEvent *e) {
  QLineEdit::focusOutEvent(e);

  emit Confirmed();
}

}  // namespace olive
