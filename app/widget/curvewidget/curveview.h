#ifndef CURVEVIEW_H
#define CURVEVIEW_H

#include "node/keyframe.h"
#include "widget/keyframeview/keyframeview.h"
#include "widget/slider/floatslider.h"

namespace olive {

class CurveView : public KeyframeView {
  Q_OBJECT
 public:
  explicit CurveView(QWidget *parent = nullptr);

  void ConnectInput(const NodeKeyframeTrackReference &ref);

  void DisconnectInput(const NodeKeyframeTrackReference &ref);

  void SelectKeyframesOfInput(const NodeKeyframeTrackReference &ref);

  void SetKeyframeTrackColor(const NodeKeyframeTrackReference &ref, const QColor &color);

  [[nodiscard]] const QHash<NodeKeyframeTrackReference, KeyframeViewInputConnection *> &GetConnections() const {
    return track_connections_;
  }

 public slots:
  void ZoomToFit();

  void ZoomToFitSelected();

  void ResetZoom();

 protected:
  void drawBackground(QPainter *painter, const QRectF &rect) override;
  void drawForeground(QPainter *painter, const QRectF &rect) override;

  void ContextMenuEvent(Menu &m) override;

  void SceneRectUpdateEvent(QRectF &r) override;

  qreal GetKeyframeSceneY(KeyframeViewInputConnection *track, NodeKeyframe *key) override;

  void DrawKeyframe(QPainter *painter, NodeKeyframe *key, KeyframeViewInputConnection *track,
                    const QRectF &key_rect) override;

  bool FirstChanceMousePress(QMouseEvent *event) override;
  void FirstChanceMouseMove(QMouseEvent *event) override;
  void FirstChanceMouseRelease(QMouseEvent *event) override;

  void KeyframeDragStart(QMouseEvent *event) override;
  void KeyframeDragMove(QMouseEvent *event, QString &tip) override;
  void KeyframeDragRelease(QMouseEvent *event, MultiUndoCommand *command) override;

 private:
  void ZoomToFitInternal(bool selected_only);

  qreal GetItemYFromKeyframeValue(NodeKeyframe *key);
  static qreal GetUnscaledItemYFromKeyframeValue(NodeKeyframe *key);

  QPointF ScalePoint(const QPointF &point);

  static FloatSlider::DisplayType GetFloatDisplayTypeFromKeyframe(NodeKeyframe *key);

  static double GetOffsetFromKeyframe(NodeKeyframe *key);

  void AdjustLines();

  QPointF GetKeyframePosition(NodeKeyframe *key);

  static QPointF GenerateBezierControlPosition(NodeKeyframe::BezierType mode, const QPointF &start_point,
                                               const QPointF &scaled_cursor_diff);

  QPointF GetScaledCursorPos(const QPointF &cursor_pos);

  QHash<NodeKeyframeTrackReference, QColor> keyframe_colors_;
  QHash<NodeKeyframeTrackReference, KeyframeViewInputConnection *> track_connections_;

  int text_padding_;

  int minimum_grid_space_;

  QVector<NodeKeyframeTrackReference> connected_inputs_;

  struct BezierPoint {
    QRectF rect;
    NodeKeyframe *keyframe{};
    NodeKeyframe::BezierType type;
  };

  QVector<BezierPoint> bezier_pts_;
  const BezierPoint *dragging_bezier_pt_;

  QPointF dragging_bezier_point_start_;
  QPointF dragging_bezier_point_opposing_start_;
  QPointF drag_start_;

  QVector<QVariant> drag_keyframe_values_;
};

}  // namespace olive

#endif  // CURVEVIEW_H
