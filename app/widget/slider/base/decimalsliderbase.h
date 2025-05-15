#ifndef DECIMALSLIDERBASE_H
#define DECIMALSLIDERBASE_H

#include "numericsliderbase.h"

namespace olive {

class DecimalSliderBase : public NumericSliderBase {
 public:
  explicit DecimalSliderBase(QWidget* parent = nullptr);

  [[nodiscard]] int GetDecimalPlaces() const { return decimal_places_; }
  void SetDecimalPlaces(int i);

  [[nodiscard]] bool GetAutoTrimDecimalPlaces() const { return autotrim_decimal_places_; };
  void SetAutoTrimDecimalPlaces(bool e);

  static QString FloatToString(double val, int decimal_places, bool autotrim_decimal_places);

 private:
  int decimal_places_;

  bool autotrim_decimal_places_;
};

}  // namespace olive

#endif  // DECIMALSLIDERBASE_H
