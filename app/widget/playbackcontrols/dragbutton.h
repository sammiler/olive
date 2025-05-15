#ifndef DRAGBUTTON_H
#define DRAGBUTTON_H

#include <QPushButton>

#include "common/define.h"

namespace olive {

class DragButton : public QPushButton {
  Q_OBJECT
 public:
  explicit DragButton(QWidget* parent = nullptr);

 signals:
  void DragStarted();

 protected:
  void mousePressEvent(QMouseEvent* event) override;

  void mouseMoveEvent(QMouseEvent* event) override;

  void mouseReleaseEvent(QMouseEvent* event) override;

 private:
  bool dragging_;
};

}  // namespace olive

#endif  // DRAGBUTTON_H
