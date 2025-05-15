

#ifndef VIEWERSAFEMARGININFO_H
#define VIEWERSAFEMARGININFO_H

#include <QtMath>

#include "common/define.h"

namespace olive {

class ViewerSafeMarginInfo {
 public:
  ViewerSafeMarginInfo() : enabled_(false) {}

  explicit ViewerSafeMarginInfo(bool enabled, double ratio = 0) : enabled_(enabled), ratio_(ratio) {}

  [[nodiscard]] bool is_enabled() const { return enabled_; }

  [[nodiscard]] bool custom_ratio() const { return (ratio_ > 0); }

  [[nodiscard]] double ratio() const { return ratio_; }

  bool operator==(const ViewerSafeMarginInfo& rhs) const {
    return (enabled_ == rhs.enabled_ && qFuzzyCompare(ratio_, rhs.ratio_));
  }

  bool operator!=(const ViewerSafeMarginInfo& rhs) const {
    return (enabled_ != rhs.enabled_ || !qFuzzyCompare(ratio_, rhs.ratio_));
  }

 private:
  bool enabled_;

  double ratio_{};
};

}  // namespace olive

#endif  // VIEWERSAFEMARGININFO_H
