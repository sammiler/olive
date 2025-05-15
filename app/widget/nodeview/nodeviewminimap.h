

#ifndef NODEVIEWMINIMAP_H
#define NODEVIEWMINIMAP_H

#include <QGraphicsView>

#include "nodeviewscene.h"

namespace olive {

class NodeViewMiniMap : public QGraphicsView {
  Q_OBJECT
 public:
  explicit NodeViewMiniMap(NodeViewScene *scene, QWidget *parent = nullptr);

 public slots:
  void SetViewportRect(const QPolygonF &rect);

 signals:
  void Resized();

  void MoveToScenePoint(const QPointF &pos);

 protected:
  void drawForeground(QPainter *painter, const QRectF &rect) override;

  void resizeEvent(QResizeEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override {}

 private slots:
  void SceneChanged(const QRectF &bounding);

  void SetDefaultSize();

 private:
  bool MouseInsideResizeTriangle(QMouseEvent *event) const;

  void EmitMoveSignal(QMouseEvent *event);

  int resize_triangle_sz_;

  QPolygonF viewport_rect_;

  bool resizing_;

  QPoint resize_anchor_;
};

}  // namespace olive

#endif  // NODEVIEWMINIMAP_H
