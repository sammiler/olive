#ifndef TEXTGIZMO_H
#define TEXTGIZMO_H

#include "gizmo.h"
#include "node/param.h"

namespace olive {

class TextGizmo : public NodeGizmo {
  Q_OBJECT
 public:
  explicit TextGizmo(QObject *parent = nullptr);

  [[nodiscard]] const QRectF &GetRect() const { return rect_; }
  void SetRect(const QRectF &r);

  [[nodiscard]] const QString &GetHtml() const { return text_; }
  void SetHtml(const QString &t) { text_ = t; }

  void SetInput(const NodeKeyframeTrackReference &input) { input_ = input; }

  void UpdateInputHtml(const QString &s, const rational &time);

  [[nodiscard]] Qt::Alignment GetVerticalAlignment() const { return valign_; }
  void SetVerticalAlignment(Qt::Alignment va);

 signals:
  void Activated();
  void Deactivated();
  void VerticalAlignmentChanged(Qt::Alignment va);
  void RectChanged(const QRectF &r);

 private:
  QRectF rect_;

  QString text_;

  NodeKeyframeTrackReference input_;

  Qt::Alignment valign_;
};

}  // namespace olive

#endif  // TEXTGIZMO_H
