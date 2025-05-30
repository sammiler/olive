#include "rationalslider.h"

#include "core.h"
#include "widget/menu/menu.h"
#include "widget/menu/menushared.h"

namespace olive {

#define super DecimalSliderBase

RationalSlider::RationalSlider(QWidget *parent) : super(parent), lock_display_type_(false) {
  connect(Core::instance(), &Core::TimecodeDisplayChanged, this, &RationalSlider::UpdateLabel);
  connect(SliderBase::label(), &SliderLabel::customContextMenuRequested, this, &RationalSlider::ShowDisplayTypeMenu);

  SetDisplayType(kFloat);

  SetValue(rational(0, 0));
}

rational RationalSlider::GetValue() { return GetValueInternal().value<rational>(); }

void RationalSlider::SetValue(const rational &d) { SetValueInternal(QVariant::fromValue(d)); }

void RationalSlider::SetDefaultValue(const rational &r) { super::SetDefaultValue(QVariant::fromValue(r)); }

void RationalSlider::SetMinimum(const rational &d) { SetMinimumInternal(QVariant::fromValue(d)); }

void RationalSlider::SetMaximum(const rational &d) { SetMaximumInternal(QVariant::fromValue(d)); }

void RationalSlider::SetTimebase(const rational &timebase) {
  timebase_ = timebase;

  // Refresh label since we have a new timebase to generate a timecode with
  UpdateLabel();
}

void RationalSlider::SetDisplayType(const RationalSlider::DisplayType &type) {
  display_type_ = type;

  UpdateLabel();
}

void RationalSlider::SetLockDisplayType(bool e) { lock_display_type_ = e; }

bool RationalSlider::GetLockDisplayType() const { return lock_display_type_; }

void RationalSlider::DisableDisplayType(RationalSlider::DisplayType type) { disabled_.append(type); }

QString RationalSlider::ValueToString(const QVariant &v) const {
  auto r = v.value<rational>();

  if (r.isNaN()) {
    return tr("NaN");
  } else {
    double val = r.toDouble() + GetOffset().value<rational>().toDouble();

    switch (display_type_) {
      case kTime:
        return QString::fromStdString(Timecode::time_to_timecode(r, timebase_, Core::instance()->GetTimecodeDisplay()));
      case kFloat:
        return FloatToString(val, GetDecimalPlaces(), GetAutoTrimDecimalPlaces());
      case kRational:
        return QString::fromStdString(v.value<rational>().toString());
    }

    return v.toString();
  }
}

QVariant RationalSlider::StringToValue(const QString &s, bool *ok) const {
  rational r;
  *ok = false;

  switch (display_type_) {
    case kTime: {
      r = Timecode::timecode_to_time(s.toUtf8().constData(), timebase_, Core::instance()->GetTimecodeDisplay(), ok);
      break;
    }
    case kFloat: {
      // First, convert to a double
      double d = s.toDouble(ok);
      if (!(*ok)) {
        break;
      }

      // If double conversion succeeded, convert to a rational
      r = rational::fromDouble(d, ok);
      break;
    }
    case kRational:
      r = rational::fromString(s.toUtf8().constData(), ok);
      break;
  }

  // return QVariant::fromValue(r - GetOffset().value<rational>());
  return QVariant::fromValue(r);
}

QVariant RationalSlider::AdjustDragDistanceInternal(const QVariant &start, const double &drag) const {
  // Assume we want smallest increment to be timebase or 1 frame
  return QVariant::fromValue(start.value<rational>() + rational::fromDouble(drag) * timebase_);
}

void RationalSlider::ValueSignalEvent(const QVariant &v) { emit ValueChanged(v.value<rational>()); }

bool RationalSlider::ValueGreaterThan(const QVariant &lhs, const QVariant &rhs) const {
  return lhs.value<rational>() > rhs.value<rational>();
}

bool RationalSlider::ValueLessThan(const QVariant &lhs, const QVariant &rhs) const {
  return lhs.value<rational>() < rhs.value<rational>();
}

void RationalSlider::ShowDisplayTypeMenu() {
  Menu m(this);

  if (!GetLockDisplayType()) {
    if (!disabled_.contains(kFloat)) {
      QAction *float_action = m.addAction(tr("Float"));
      float_action->setData(kFloat);
      connect(float_action, &QAction::triggered, this, &RationalSlider::SetDisplayTypeFromMenu);
    }

    if (!disabled_.contains(kRational)) {
      QAction *rational_action = m.addAction(tr("Rational"));
      rational_action->setData(kRational);
      connect(rational_action, &QAction::triggered, this, &RationalSlider::SetDisplayTypeFromMenu);
    }

    if (!disabled_.contains(kTime)) {
      QAction *time_action = m.addAction(tr("Time"));
      time_action->setData(kTime);
      connect(time_action, &QAction::triggered, this, &RationalSlider::SetDisplayTypeFromMenu);
    }
  }

  if (display_type_ == kTime) {
    if (!m.actions().isEmpty()) {
      m.addSeparator();
    }
    MenuShared::instance()->AddItemsForTimeRulerMenu(&m);
    MenuShared::instance()->AboutToShowTimeRulerActions(timebase_);
  }

  if (!m.actions().isEmpty()) {
    m.exec(QCursor::pos());
    UpdateLabel();
  }
}

void RationalSlider::SetDisplayTypeFromMenu() {
  auto *action = dynamic_cast<QAction *>(sender());

  DisplayType type = static_cast<DisplayType>(action->data().toInt());

  SetDisplayType(type);
}

}  // namespace olive
