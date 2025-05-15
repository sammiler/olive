

#include "dragbutton.h"

#include <QMouseEvent>

namespace olive {

DragButton::DragButton(QWidget *parent) : QPushButton(parent) {
  setCursor(Qt::OpenHandCursor);
  dragging_ = false;
}

void DragButton::mousePressEvent(QMouseEvent *event) { QPushButton::mousePressEvent(event); }

void DragButton::mouseMoveEvent(QMouseEvent *event) {
  QPushButton::mouseMoveEvent(event);

  if (event->buttons() && !dragging_) {
    emit DragStarted();
    dragging_ = true;
  }
}

void DragButton::mouseReleaseEvent(QMouseEvent *event) {
  QPushButton::mouseReleaseEvent(event);

  dragging_ = false;
}

}  // namespace olive
