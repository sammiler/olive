

#ifndef COLORGRADIENTGLWIDGET_H
#define COLORGRADIENTGLWIDGET_H

#include "colorswatchwidget.h"

namespace olive {

class ColorGradientWidget : public ColorSwatchWidget {
  Q_OBJECT
 public:
  explicit ColorGradientWidget(Qt::Orientation orientation, QWidget* parent = nullptr);

 protected:
  [[nodiscard]] Color GetColorFromScreenPos(const QPoint& p) const override;

  void paintEvent(QPaintEvent* e) override;

  void SelectedColorChangedEvent(const Color& c, bool external) override;

 private:
  static Color LerpColor(const Color& a, const Color& b, int i, int max);

  QPixmap cached_gradient_;

  Qt::Orientation orientation_;

  Color start_;

  Color end_;

  float val_;
};

}  // namespace olive

#endif  // COLORGRADIENTGLWIDGET_H
