#include "nodeparamviewitemtitlebar.h"

#include <QHBoxLayout>
#include <QPainter>

#include "ui/icons/icons.h"

namespace olive {

NodeParamViewItemTitleBar::NodeParamViewItemTitleBar(QWidget *parent) : QWidget(parent), draw_border_(true) {
  auto *layout = new QHBoxLayout(this);

  collapse_btn_ = new CollapseButton(this);
  connect(collapse_btn_, &QPushButton::clicked, this, &NodeParamViewItemTitleBar::ExpandedStateChanged);
  layout->addWidget(collapse_btn_);

  lbl_ = new QLabel(this);
  layout->addWidget(lbl_);

  // Place next buttons on the far side
  layout->addStretch();

  add_fx_btn_ = new QPushButton(this);
  add_fx_btn_->setIcon(icon::AddEffect);
  add_fx_btn_->setFixedSize(add_fx_btn_->sizeHint().height(), add_fx_btn_->sizeHint().height());
  add_fx_btn_->setVisible(false);
  layout->addWidget(add_fx_btn_);
  connect(add_fx_btn_, &QPushButton::clicked, this, &NodeParamViewItemTitleBar::AddEffectButtonClicked);

  pin_btn_ = new QPushButton(QStringLiteral("P"), this);
  pin_btn_->setCheckable(true);
  pin_btn_->setFixedSize(pin_btn_->sizeHint().height(), pin_btn_->sizeHint().height());
  pin_btn_->setVisible(false);
  layout->addWidget(pin_btn_);
  connect(pin_btn_, &QPushButton::clicked, this, &NodeParamViewItemTitleBar::PinToggled);

  enabled_checkbox_ = new QCheckBox(this);
  enabled_checkbox_->setVisible(false);
  layout->addWidget(enabled_checkbox_);
  connect(enabled_checkbox_, &QCheckBox::clicked, this, &NodeParamViewItemTitleBar::EnabledCheckBoxClicked);
}

void NodeParamViewItemTitleBar::SetExpanded(bool e) {
  draw_border_ = e;
  collapse_btn_->setChecked(e);

  update();
}

void NodeParamViewItemTitleBar::paintEvent(QPaintEvent *event) {
  QWidget::paintEvent(event);

  if (draw_border_) {
    QPainter p(this);

    // Draw bottom border using text color
    int bottom = height() - 1;
    p.setPen(palette().text().color());
    p.drawLine(0, bottom, width(), bottom);
  }
}

void NodeParamViewItemTitleBar::mousePressEvent(QMouseEvent *event) {
  QWidget::mousePressEvent(event);

  emit Clicked();
}

void NodeParamViewItemTitleBar::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseDoubleClickEvent(event);

  collapse_btn_->click();
}

}  // namespace olive
