

#ifndef BEZIERWIDGET_H
#define BEZIERWIDGET_H

#include <olive/core/core.h>
#include <QCheckBox>
#include <QWidget>

#include "widget/slider/floatslider.h"

namespace olive {

using namespace core;

class BezierWidget : public QWidget {
  Q_OBJECT
 public:
  explicit BezierWidget(QWidget *parent = nullptr);

  [[nodiscard]] Bezier GetValue() const;

  void SetValue(const Bezier &b);

  [[nodiscard]] FloatSlider *x_slider() const { return x_slider_; }

  [[nodiscard]] FloatSlider *y_slider() const { return y_slider_; }

  [[nodiscard]] FloatSlider *cp1_x_slider() const { return cp1_x_slider_; }

  [[nodiscard]] FloatSlider *cp1_y_slider() const { return cp1_y_slider_; }

  [[nodiscard]] FloatSlider *cp2_x_slider() const { return cp2_x_slider_; }

  [[nodiscard]] FloatSlider *cp2_y_slider() const { return cp2_y_slider_; }

 signals:
  void ValueChanged();

 private:
  FloatSlider *x_slider_;

  FloatSlider *y_slider_;

  FloatSlider *cp1_x_slider_;

  FloatSlider *cp1_y_slider_;

  FloatSlider *cp2_x_slider_;

  FloatSlider *cp2_y_slider_;
};

}  // namespace olive

#endif  // BEZIERWIDGET_H
