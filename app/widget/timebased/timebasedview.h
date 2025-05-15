#ifndef TIMELINEVIEWBASE_H
#define TIMELINEVIEWBASE_H

#include <QGraphicsView>
#include <vector>

#include "core.h"
#include "timescaledobject.h"
#include "widget/handmovableview/handmovableview.h"

namespace olive {

class TimeBasedWidget;

class TimeBasedView : public HandMovableView, public TimeScaledObject {
  Q_OBJECT
 public:
  explicit TimeBasedView(QWidget *parent = nullptr);

  void EnableSnap(const std::vector<rational> &points);
  void DisableSnap();
  [[nodiscard]] bool IsSnapped() const { return snapped_; }

  [[nodiscard]] TimeBasedWidget *GetSnapService() const { return snap_service_; }
  void SetSnapService(TimeBasedWidget *service) { snap_service_ = service; }

  [[nodiscard]] const double &GetYScale() const;
  void SetYScale(const double &y_scale);

  [[nodiscard]] virtual bool IsDraggingPlayhead() const { return dragging_playhead_; }

  // To be called only by selection managers
  virtual void SelectionManagerSelectEvent(void *obj) {}
  virtual void SelectionManagerDeselectEvent(void *obj) {}

  [[nodiscard]] ViewerOutput *GetViewerNode() const { return viewer_; }

  void SetViewerNode(ViewerOutput *v);

  [[nodiscard]] QPointF ScalePoint(const QPointF &p) const;
  [[nodiscard]] QPointF UnscalePoint(const QPointF &p) const;

 public slots:
  void SetEndTime(const rational &length);

  /**
   * @brief Slot called whenever the view resizes or the scene contents change to enforce minimum scene sizes
   */
  void UpdateSceneRect();

 signals:
  void ScaleChanged(double scale);

 protected:
  void drawForeground(QPainter *painter, const QRectF &rect) override;

  void resizeEvent(QResizeEvent *event) override;

  void ScaleChangedEvent(const double &scale) override;

  virtual void SceneRectUpdateEvent(QRectF &) {}

  virtual void VerticalScaleChangedEvent(double scale);

  void ZoomIntoCursorPosition(QWheelEvent *event, double multiplier, const QPointF &cursor_pos) override;

  bool PlayheadPress(QMouseEvent *event);
  bool PlayheadMove(QMouseEvent *event);
  bool PlayheadRelease(QMouseEvent *event);

  void TimebaseChangedEvent(const rational &) override;

  [[nodiscard]] bool IsYAxisEnabled() const { return y_axis_enabled_; }

  void SetYAxisEnabled(bool e) { y_axis_enabled_ = e; }

 private:
  qreal GetPlayheadX();

  double playhead_scene_left_;
  double playhead_scene_right_;

  bool dragging_playhead_;

  QGraphicsScene scene_;

  bool snapped_;
  std::vector<rational> snap_time_;

  rational end_time_;

  TimeBasedWidget *snap_service_;

  bool y_axis_enabled_;

  double y_scale_;

  ViewerOutput *viewer_;
};

}  // namespace olive

#endif  // TIMELINEVIEWBASE_H
