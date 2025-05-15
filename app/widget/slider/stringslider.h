#ifndef STRINGSLIDER_H
#define STRINGSLIDER_H

#include "base/sliderbase.h"

namespace olive {

class StringSlider : public SliderBase {
  Q_OBJECT
 public:
  explicit StringSlider(QWidget* parent = nullptr);

  void SetDragMultiplier(const double& d) = delete;

  [[nodiscard]] QString GetValue() const;

  void SetValue(const QString& v);

  void SetDefaultValue(const QString& v);

 signals:
  void ValueChanged(const QString& str);

 protected:
  [[nodiscard]] QString ValueToString(const QVariant& value) const override;

  QVariant StringToValue(const QString& s, bool* ok) const override;

  void ValueSignalEvent(const QVariant& value) override;
};

}  // namespace olive

#endif  // STRINGSLIDER_H
