

#ifndef COLORWHEELWIDGET_H
#define COLORWHEELWIDGET_H

#include <QOpenGLWidget>

#include "colorswatchwidget.h"

namespace olive {

class ColorWheelWidget : public ColorSwatchWidget {
  Q_OBJECT
 public:
  explicit ColorWheelWidget(QWidget* parent = nullptr);

 signals:
  void DiameterChanged(int radius);

 protected:
  [[nodiscard]] Color GetColorFromScreenPos(const QPoint& p) const override;

  void resizeEvent(QResizeEvent* e) override;

  void paintEvent(QPaintEvent* e) override;

  void SelectedColorChangedEvent(const Color& c, bool external) override;

 private:
  [[nodiscard]] int GetDiameter() const;

  [[nodiscard]] qreal GetRadius() const;

  struct Triangle {
    qreal opposite;
    qreal adjacent;
    qreal hypotenuse;
  };

  [[nodiscard]] static Triangle GetTriangleFromCoords(const QPoint& center, const QPoint& p);
  [[nodiscard]] static Triangle GetTriangleFromCoords(const QPoint& center, qreal y, qreal x);

  [[nodiscard]] Color GetColorFromTriangle(const Triangle& tri) const;
  [[nodiscard]] QPoint GetCoordsFromColor(const Color& c) const;

  QPixmap cached_wheel_;

  float val_;

  bool force_redraw_;
};

}  // namespace olive

#endif  // COLORWHEELWIDGET_H
