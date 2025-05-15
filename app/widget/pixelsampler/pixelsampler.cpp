

#include "pixelsampler.h"

#include <QVBoxLayout>

namespace olive {

PixelSamplerWidget::PixelSamplerWidget(QWidget *parent) : QGroupBox(parent) {
  auto *layout = new QHBoxLayout(this);

  box_ = new ColorPreviewBox();
  QFontMetrics fm = fontMetrics();
  int box_sz = fm.height() * 2;
  box_->setFixedSize(box_sz, box_sz);
  layout->addWidget(box_);

  label_ = new QLabel();
  layout->addWidget(label_);

  setTitle(tr("Color"));

  UpdateLabelInternal();
}

void PixelSamplerWidget::SetValues(const Color &color) {
  color_ = color;
  UpdateLabelInternal();
}

void PixelSamplerWidget::UpdateLabelInternal() {
  box_->SetColor(color_);

  label_->setText(tr("<html>"
                     "<font color='#FF8080'>R: %1 (%5)</font><br>"
                     "<font color='#80FF80'>G: %2 (%6)</font><br>"
                     "<font color='#8080FF'>B: %3 (%7)</font><br>"
                     "A: %4 (%8)"
                     "</html>")
                      .arg(QString::number(color_.red()), QString::number(color_.green()),
                           QString::number(color_.blue()), QString::number(color_.alpha()),
                           QString::number(int(color_.red() * 255.0)), QString::number(int(color_.green() * 255.0)),
                           QString::number(int(color_.blue() * 255.0)), QString::number(int(color_.alpha() * 255.0))));
}

ManagedPixelSamplerWidget::ManagedPixelSamplerWidget(QWidget *parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  display_view_ = new PixelSamplerWidget();
  display_view_->setTitle(tr("Display"));
  layout->addWidget(display_view_);

  reference_view_ = new PixelSamplerWidget();
  reference_view_->setTitle(tr("Reference"));
  layout->addWidget(reference_view_);
}

void ManagedPixelSamplerWidget::SetValues(const Color &reference, const Color &display) {
  reference_view_->SetValues(reference);
  display_view_->SetValues(display);
}

}  // namespace olive
