

#include "integerslider.h"

namespace olive {

#define super NumericSliderBase

IntegerSlider::IntegerSlider(QWidget *parent) : super(parent) { SetValue(0); }

int64_t IntegerSlider::GetValue() { return GetValueInternal().toLongLong(); }

void IntegerSlider::SetValue(const int64_t &v) { SetValueInternal(QVariant::fromValue(v)); }

void IntegerSlider::SetMinimum(const int64_t &d) { SetMinimumInternal(QVariant::fromValue(d)); }

void IntegerSlider::SetMaximum(const int64_t &d) { SetMaximumInternal(QVariant::fromValue(d)); }

void IntegerSlider::SetDefaultValue(const int64_t &d) { super::SetDefaultValue(QVariant::fromValue(d)); }

QString IntegerSlider::ValueToString(const QVariant &v) const {
  return QString::number(v.toLongLong() + GetOffset().toLongLong());
}

QVariant IntegerSlider::StringToValue(const QString &s, bool *ok) const {
  bool valid;

  // Allow both floats and integers for either modes
  double decimal_val = s.toDouble(&valid);

  if (ok) {
    *ok = valid;
  }

  decimal_val -= GetOffset().toLongLong();

  if (valid) {
    // But for an integer, we round it
    return qRound(decimal_val);
  }

  return {};
}

void IntegerSlider::ValueSignalEvent(const QVariant &value) { emit ValueChanged(value.toInt()); }

QVariant IntegerSlider::AdjustDragDistanceInternal(const QVariant &start, const double &drag) const {
  return qRound64(super::AdjustDragDistanceInternal(start, drag).toDouble());
}

}  // namespace olive
