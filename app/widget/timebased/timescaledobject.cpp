#include "timescaledobject.h"

#include <QtMath>
#include <cfloat>

#include "audio/audiovisualwaveform.h"

namespace olive {

const int TimeScaledObject::kCalculateDimensionsPadding = 10;

TimeScaledObject::TimeScaledObject()
    : scale_(1.0), min_scale_(0), max_scale_(AudioVisualWaveform::kMaximumSampleRate.toDouble()) {}

void TimeScaledObject::SetTimebase(const rational &timebase) {
  timebase_ = timebase;
  timebase_dbl_ = timebase_.toDouble();

  TimebaseChangedEvent(timebase);
}

const rational &TimeScaledObject::timebase() const { return timebase_; }

const double &TimeScaledObject::timebase_dbl() const { return timebase_dbl_; }

rational TimeScaledObject::SceneToTime(const double &x, const double &x_scale, const rational &timebase, bool round) {
  double unscaled_time = x / x_scale / timebase.toDouble();

  // Adjust screen point by scale and timebase
  qint64 rounded_x_mvmt;

  if (round) {
    rounded_x_mvmt = qRound64(unscaled_time);
  } else if (unscaled_time < 0) {
    // "floor" to zero
    rounded_x_mvmt = qCeil(unscaled_time);
  } else {
    rounded_x_mvmt = qFloor(unscaled_time);
  }

  // Return a time in the timebase
  return rational(rounded_x_mvmt * timebase.numerator(), timebase.denominator());
}

rational TimeScaledObject::SceneToTimeNoGrid(const double &x, const double &x_scale) {
  double unscaled_time = x / x_scale;

  return rational::fromDouble(unscaled_time);
}

double TimeScaledObject::TimeToScene(const rational &time) const { return time.toDouble() * scale_; }

rational TimeScaledObject::SceneToTime(const double &x, bool round) const {
  return SceneToTime(x, scale_, timebase_, round);
}

rational TimeScaledObject::SceneToTimeNoGrid(const double &x) const { return SceneToTimeNoGrid(x, scale_); }

void TimeScaledObject::SetMaximumScale(const double &max) {
  max_scale_ = max;

  if (GetScale() > max_scale_) {
    SetScale(max_scale_);
  }
}

void TimeScaledObject::SetMinimumScale(const double &min) {
  min_scale_ = min;

  if (GetScale() < min_scale_) {
    SetScale(min_scale_);
  }
}

const double &TimeScaledObject::GetScale() const { return scale_; }

void TimeScaledObject::SetScale(const double &scale) {
  Q_ASSERT(scale > 0);

  scale_ = std::clamp(scale, min_scale_, max_scale_);

  ScaleChangedEvent(scale_);
}

void TimeScaledObject::SetScaleFromDimensions(double viewport_width, double content_width) {
  SetScale(CalculateScaleFromDimensions(viewport_width, content_width));
}

double TimeScaledObject::CalculateScaleFromDimensions(double viewport_sz, double content_sz) {
  return static_cast<double>(viewport_sz / kCalculateDimensionsPadding * (kCalculateDimensionsPadding - 1)) /
         static_cast<double>(content_sz);
}

double TimeScaledObject::CalculatePaddingFromDimensionScale(double viewport_sz) {
  return (viewport_sz / (kCalculateDimensionsPadding * 2));
}

TimelineScaledWidget::TimelineScaledWidget(QWidget *parent) : QWidget(parent) {}

}  // namespace olive
