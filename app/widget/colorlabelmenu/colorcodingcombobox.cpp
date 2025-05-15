#include "colorcodingcombobox.h"

#include "ui/colorcoding.h"

namespace olive {

ColorCodingComboBox::ColorCodingComboBox(QWidget* parent) : QComboBox(parent) { SetColor(0); }

void ColorCodingComboBox::showPopup() {
  ColorLabelMenu menu(this);

  menu.setMinimumWidth(width());

  QAction* a = menu.exec(parentWidget()->mapToGlobal(pos()));

  if (a) {
    SetColor(a->data().toInt());
  }
}

void ColorCodingComboBox::SetColor(int index) {
  clear();
  addItem(ColorCoding::GetColorName(index));
  index_ = index;
}

}  // namespace olive
