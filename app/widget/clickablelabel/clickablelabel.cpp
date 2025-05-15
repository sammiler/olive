#include "clickablelabel.h"

#include <QMouseEvent>

namespace olive {

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent) : QLabel(text, parent) {}

ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent) {}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton && underMouse()) {
    emit MouseClicked();
  }
}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit MouseDoubleClicked();
  }
}

}  // namespace olive
