

#include "sliderlabel.h"

#include <QApplication>
#include <QDebug>
#include <QMouseEvent>

namespace olive {

SliderLabel::SliderLabel(QWidget *parent) : QLabel(parent), override_color_enabled_(false) {
  QPalette p = palette();

  p.setColor(QPalette::Disabled, QPalette::Highlight, p.color(QPalette::Disabled, QPalette::ButtonText));

  setPalette(p);

  // Use highlight color as font color
  setForegroundRole(QPalette::Link);

  // Set underlined
  QFont f = font();
  f.setUnderline(true);
  setFont(f);

  // Allow users to tab to this widget
  setFocusPolicy(Qt::TabFocus);

  // Add custom context menu
  setContextMenuPolicy(Qt::CustomContextMenu);
}

void SliderLabel::SetColor(const QColor &c) {
  // Prevent infinite loop in changeEvent when we set the stylesheet
  override_color_enabled_ = false;
  override_color_ = c;

  // Different colors will look different depending on the theme (light/dark mode). We abstract
  // that away here so that other classes can simply choose a color and we will handle making it
  // more legible based on the background
  QColor adjusted;
  if (palette().window().color().lightness() < 128) {
    adjusted = override_color_.lighter(150);
  } else {
    adjusted = override_color_.darker(150);
  }

  setStyleSheet(QStringLiteral("color: %1").arg(adjusted.name()));
  override_color_enabled_ = true;
}

void SliderLabel::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    if (e->modifiers() & Qt::AltModifier) {
      emit RequestReset();
    } else {
      emit LabelPressed();
    }
  }
}

void SliderLabel::mouseReleaseEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    if (!(e->modifiers() & Qt::AltModifier)) {
      emit LabelReleased();
    }
  }
}

void SliderLabel::focusInEvent(QFocusEvent *event) {
  QLabel::focusInEvent(event);

  if (event->reason() == Qt::TabFocusReason) {
    emit focused();
  }
}

void SliderLabel::changeEvent(QEvent *event) {
  QLabel::changeEvent(event);

  if (override_color_enabled_ && event->type() == QEvent::StyleChange) {
    SetColor(override_color_);
  }
}

}  // namespace olive
