#ifndef HANDMOVABLEVIEW_H
#define HANDMOVABLEVIEW_H

#include <QGraphicsView>
#include <QMenu>

#include "tool/tool.h"

namespace olive {

class HandMovableView : public QGraphicsView {
  Q_OBJECT
 public:
  explicit HandMovableView(QWidget* parent = nullptr);

  static bool WheelEventIsAZoomEvent(QWheelEvent* event);

  static qreal GetScrollZoomMultiplier(QWheelEvent* event);

  virtual void CatchUpScrollEvent() {}

 protected:
  virtual void ToolChangedEvent(Tool::Item tool) { Q_UNUSED(tool) }

  bool HandPress(QMouseEvent* event);
  bool HandMove(QMouseEvent* event);
  bool HandRelease(QMouseEvent* event);

  void SetDefaultDragMode(DragMode mode);
  [[nodiscard]] const DragMode& GetDefaultDragMode() const;

  void wheelEvent(QWheelEvent* event) override;

  virtual void ZoomIntoCursorPosition(QWheelEvent* event, double multiplier, const QPointF& cursor_pos);

  void SetIsTimelineAxes(bool e) { is_timeline_axes_ = e; }

 private:
  bool dragging_hand_;
  DragMode pre_hand_drag_mode_;

  DragMode default_drag_mode_;

  QPointF transformed_pos_;

  bool is_timeline_axes_;

 private slots:
  void ApplicationToolChanged(Tool::Item tool);
};

}  // namespace olive

#endif  // HANDMOVABLEVIEW_H
