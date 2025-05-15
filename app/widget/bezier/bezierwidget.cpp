#include "bezierwidget.h"

#include <QGridLayout>
#include <QGroupBox>

namespace olive {

BezierWidget::BezierWidget(QWidget *parent) : QWidget{parent} {
  auto *layout = new QGridLayout(this);

  int row = 0;

  layout->addWidget(new QLabel(tr("Center:")), row, 0);

  x_slider_ = new FloatSlider();
  connect(x_slider_, &FloatSlider::ValueChanged, this, &BezierWidget::ValueChanged);
  layout->addWidget(x_slider_, row, 1);

  y_slider_ = new FloatSlider();
  connect(y_slider_, &FloatSlider::ValueChanged, this, &BezierWidget::ValueChanged);
  layout->addWidget(y_slider_, row, 2);

  row++;

  auto *bezier_group = new QGroupBox(tr("Bezier"));
  layout->addWidget(bezier_group, row, 0, 1, 3);

  auto *bezier_layout = new QGridLayout(bezier_group);

  row = 0;

  bezier_layout->addWidget(new QLabel(tr("In:")), row, 0);

  cp1_x_slider_ = new FloatSlider();
  connect(cp1_x_slider_, &FloatSlider::ValueChanged, this, &BezierWidget::ValueChanged);
  bezier_layout->addWidget(cp1_x_slider_, row, 1);

  cp1_y_slider_ = new FloatSlider();
  connect(cp1_y_slider_, &FloatSlider::ValueChanged, this, &BezierWidget::ValueChanged);
  bezier_layout->addWidget(cp1_y_slider_, row, 2);

  row++;

  bezier_layout->addWidget(new QLabel(tr("Out:")), row, 0);

  cp2_x_slider_ = new FloatSlider();
  connect(cp2_x_slider_, &FloatSlider::ValueChanged, this, &BezierWidget::ValueChanged);
  bezier_layout->addWidget(cp2_x_slider_, row, 1);

  cp2_y_slider_ = new FloatSlider();
  connect(cp2_y_slider_, &FloatSlider::ValueChanged, this, &BezierWidget::ValueChanged);
  bezier_layout->addWidget(cp2_y_slider_, row, 2);
}

Bezier BezierWidget::GetValue() const {
  Bezier b;

  b.set_x(x_slider_->GetValue());
  b.set_y(y_slider_->GetValue());
  b.set_cp1_x(cp1_x_slider_->GetValue());
  b.set_cp1_y(cp1_y_slider_->GetValue());
  b.set_cp2_x(cp2_x_slider_->GetValue());
  b.set_cp2_y(cp2_y_slider_->GetValue());

  return b;
}

void BezierWidget::SetValue(const Bezier &b) {
  x_slider_->SetValue(b.x());
  y_slider_->SetValue(b.y());
  cp1_x_slider_->SetValue(b.cp1_x());
  cp1_y_slider_->SetValue(b.cp1_y());
  cp2_x_slider_->SetValue(b.cp2_x());
  cp2_y_slider_->SetValue(b.cp2_y());
}

}  // namespace olive
