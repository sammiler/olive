

#include "viewerplaybacktimer.h"

#include <QDateTime>
#include <QtMath>

namespace olive {

void ViewerPlaybackTimer::Start(const int64_t &start_timestamp, const int &playback_speed, const double &timebase) {
  start_msec_ = QDateTime::currentMSecsSinceEpoch();
  start_timestamp_ = start_timestamp;
  playback_speed_ = playback_speed;
  timebase_ = timebase * 1000;
}

int64_t ViewerPlaybackTimer::GetTimestampNow() const {
  int64_t real_time = QDateTime::currentMSecsSinceEpoch() - start_msec_;

  int64_t frames_since_start = qFloor(static_cast<double>(real_time) / (timebase_));

  return start_timestamp_ + frames_since_start * playback_speed_;
}

}  // namespace olive
