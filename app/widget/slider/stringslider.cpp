

#include "stringslider.h"

namespace olive {

#define super SliderBase

StringSlider::StringSlider(QWidget *parent) : super(parent) {
  SetValue(QString());

  connect(label(), &SliderLabel::LabelReleased, this, &SliderBase::ShowEditor);
}

QString StringSlider::GetValue() const { return GetValueInternal().toString(); }

void StringSlider::SetValue(const QString &v) { SetValueInternal(v); }

void StringSlider::SetDefaultValue(const QString &v) { super::SetDefaultValue(v); }

QString StringSlider::ValueToString(const QVariant &v) const {
  QString vstr = v.toString();
  return (vstr.isEmpty()) ? tr("(none)") : vstr;
}

QVariant StringSlider::StringToValue(const QString &s, bool *ok) const {
  *ok = true;
  return s;
}

void StringSlider::ValueSignalEvent(const QVariant &value) { emit ValueChanged(value.toString()); }

}  // namespace olive
