

#ifndef FLOATSLIDER_H
#define FLOATSLIDER_H

#include "base/decimalsliderbase.h"

namespace olive {

class FloatSlider : public DecimalSliderBase {
  Q_OBJECT
 public:
  explicit FloatSlider(QWidget* parent = nullptr);

  enum DisplayType { kNormal, kDecibel, kPercentage };

  [[nodiscard]] double GetValue() const;

  void SetValue(const double& d);

  void SetDefaultValue(const double& d);

  void SetMinimum(const double& d);

  void SetMaximum(const double& d);

  void SetDisplayType(const DisplayType& type);

  static double TransformValueToDisplay(double val, DisplayType display);

  static double TransformDisplayToValue(double val, DisplayType display);

  static QString ValueToString(double val, DisplayType display, int decimal_places, bool autotrim_decimal_places);

 protected:
  [[nodiscard]] QString ValueToString(const QVariant& v) const override;

  QVariant StringToValue(const QString& s, bool* ok) const override;

  [[nodiscard]] QVariant AdjustDragDistanceInternal(const QVariant& start, const double& drag) const override;

  void ValueSignalEvent(const QVariant& value) override;

 signals:
  void ValueChanged(double);

 private:
  DisplayType display_type_;
};

}  // namespace olive

#endif  // FLOATSLIDER_H
