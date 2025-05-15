#ifndef TIMELINESCALEDOBJECT_H
#define TIMELINESCALEDOBJECT_H

#include <olive/core/core.h>
#include <QWidget>

#include "node/block/block.h"

namespace olive {

/**
 * @brief Provides base functionality for any object that uses time and scale
 */
class TimeScaledObject {
 public:
  TimeScaledObject();
  virtual ~TimeScaledObject() = default;

  void SetTimebase(const rational& timebase);

  [[nodiscard]] const rational& timebase() const;
  [[nodiscard]] const double& timebase_dbl() const;

  static rational SceneToTime(const double& x, const double& x_scale, const rational& timebase, bool round = false);
  static rational SceneToTimeNoGrid(const double& x, const double& x_scale);

  [[nodiscard]] const double& GetScale() const;
  [[nodiscard]] const double& GetMaximumScale() const { return max_scale_; }

  void SetScale(const double& scale);

  void SetScaleFromDimensions(double viewport_width, double content_width);
  static double CalculateScaleFromDimensions(double viewport_sz, double content_sz);
  static double CalculatePaddingFromDimensionScale(double viewport_sz);

  [[nodiscard]] double TimeToScene(const rational& time) const;
  [[nodiscard]] rational SceneToTime(const double& x, bool round = false) const;
  [[nodiscard]] rational SceneToTimeNoGrid(const double& x) const;

 protected:
  virtual void TimebaseChangedEvent(const rational&) {}

  virtual void ScaleChangedEvent(const double&) {}

  void SetMaximumScale(const double& max);

  void SetMinimumScale(const double& min);

 private:
  rational timebase_;

  double timebase_dbl_{};

  double scale_;

  double min_scale_;

  double max_scale_;

  static const int kCalculateDimensionsPadding;
};

class TimelineScaledWidget : public QWidget, public TimeScaledObject {
  Q_OBJECT
 public:
  explicit TimelineScaledWidget(QWidget* parent = nullptr);
};

}  // namespace olive

#endif  // TIMELINESCALEDOBJECT_H
