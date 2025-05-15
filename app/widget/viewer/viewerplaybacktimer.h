

#ifndef VIEWERPLAYBACKTIMER_H
#define VIEWERPLAYBACKTIMER_H

#include <QtGlobal>

#include "common/define.h"

namespace olive {

class ViewerPlaybackTimer {
 public:
  void Start(const int64_t& start_timestamp, const int& playback_speed, const double& timebase);

  [[nodiscard]] int64_t GetTimestampNow() const;

 private:
  qint64 start_msec_;
  int64_t start_timestamp_;

  int playback_speed_;

  double timebase_;
};

}  // namespace olive

#endif  // VIEWERPLAYBACKTIMER_H
