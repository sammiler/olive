

#ifndef INTEGERSLIDER_H
#define INTEGERSLIDER_H

#include "base/numericsliderbase.h"

namespace olive {

class IntegerSlider : public NumericSliderBase {
  Q_OBJECT
 public:
  explicit IntegerSlider(QWidget* parent = nullptr);

  int64_t GetValue();

  void SetValue(const int64_t& v);

  void SetMinimum(const int64_t& d);

  void SetMaximum(const int64_t& d);

  void SetDefaultValue(const int64_t& d);

 protected:
  [[nodiscard]] QString ValueToString(const QVariant& v) const override;

  QVariant StringToValue(const QString& s, bool* ok) const override;

  void ValueSignalEvent(const QVariant& value) override;

  [[nodiscard]] QVariant AdjustDragDistanceInternal(const QVariant& start, const double& drag) const override;

 signals:
  void ValueChanged(int64_t);
};

}  // namespace olive

#endif  // INTEGERSLIDER_H
