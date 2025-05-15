#ifndef RESIZABLESCROLLBAR_H
#define RESIZABLESCROLLBAR_H

#include <QScrollBar>

#include "common/define.h"

namespace olive {

class ResizableScrollBar : public QScrollBar {
  Q_OBJECT
 public:
  explicit ResizableScrollBar(QWidget* parent = nullptr);
  explicit ResizableScrollBar(Qt::Orientation orientation, QWidget* parent = nullptr);

 signals:
  void ResizeBegan(int old_bar_width, bool top_handle);

  void ResizeMoved(int movement);

  void ResizeEnded();

 protected:
  void mousePressEvent(QMouseEvent* event) override;

  void mouseMoveEvent(QMouseEvent* event) override;

  void mouseReleaseEvent(QMouseEvent* event) override;

 private:
  QRect GetScrollBarRect();

  static const int kHandleWidth;

  enum MouseHandleState { kNotInHandle, kInTopHandle, kInBottomHandle };

  void Init();

  int GetActiveMousePos(QMouseEvent* event);

  int GetActiveBarSize();

  MouseHandleState mouse_handle_state_;

  bool dragging_{};

  int drag_start_point_{};
};

}  // namespace olive

#endif  // RESIZABLESCROLLBAR_H
