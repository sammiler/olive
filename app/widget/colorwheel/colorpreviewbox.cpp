

#include "colorpreviewbox.h"

#include <QPainter>
#include <utility>

#include "common/qtutils.h"

namespace olive {

ColorPreviewBox::ColorPreviewBox(QWidget *parent)
    : QWidget(parent), to_ref_processor_(nullptr), to_display_processor_(nullptr) {}

void ColorPreviewBox::SetColorProcessor(ColorProcessorPtr to_ref, ColorProcessorPtr to_display) {
  to_ref_processor_ = std::move(to_ref);
  to_display_processor_ = std::move(to_display);

  update();
}

void ColorPreviewBox::SetColor(const Color &c) {
  color_ = c;
  update();
}

void ColorPreviewBox::paintEvent(QPaintEvent *e) {
  QWidget::paintEvent(e);

  QColor c;

  // Color management
  if (to_ref_processor_ && to_display_processor_) {
    c = QtUtils::toQColor(to_display_processor_->ConvertColor(to_ref_processor_->ConvertColor(color_)));
  } else {
    c = QtUtils::toQColor(color_);
  }

  QPainter p(this);

  QRect draw_rect = rect().adjusted(0, 0, -1, -1);

  p.setPen(Qt::black);

  if (color_.alpha() < 1.0) {
    // Draw black background so the background isn't the window color
    p.setBrush(Qt::black);
    p.drawRect(draw_rect);
  }

  // Draw with color over the top
  p.setBrush(c);
  p.drawRect(draw_rect);
}

}  // namespace olive
