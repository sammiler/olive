#include "collapsebutton.h"

#include "ui/icons/icons.h"

namespace olive {

CollapseButton::CollapseButton(QWidget *parent) : QPushButton(parent) {
  setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  setStyleSheet("border: none; background: none;");
  setCheckable(true);
  setChecked(true);
  setIconSize(QSize(fontMetrics().height() / 2, fontMetrics().height() / 2));

  connect(this, &CollapseButton::toggled, this, &CollapseButton::UpdateIcon);

  UpdateIcon(isChecked());
}

void CollapseButton::UpdateIcon(bool e) {
  if (e) {
    setIcon(icon::TriDown);
  } else {
    setIcon(icon::TriRight);
  }
}

}  // namespace olive
