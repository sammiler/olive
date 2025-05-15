

#include "decimalsliderbase.h"

namespace olive {

#define super NumericSliderBase

DecimalSliderBase::DecimalSliderBase(QWidget *parent)
    : super(parent), decimal_places_(2), autotrim_decimal_places_(false) {}

void DecimalSliderBase::SetAutoTrimDecimalPlaces(bool e) {
  autotrim_decimal_places_ = e;

  UpdateLabel();
}

QString DecimalSliderBase::FloatToString(double val, int decimal_places, bool autotrim_decimal_places) {
  QString s = QString::number(val, 'f', decimal_places);

  if (autotrim_decimal_places) {
    while (s.endsWith('0') && s.at(s.size() - 2).isDigit()) {
      s = s.left(s.size() - 1);
    }
  }

  return s;
}

void DecimalSliderBase::SetDecimalPlaces(int i) {
  decimal_places_ = i;

  UpdateLabel();
}

}  // namespace olive
