#ifndef VIEWERPLAYBACKTIMER_H
#define VIEWERPLAYBACKTIMER_H

#include <QtGlobal>

#include "common/define.h"

namespace olive {

/**
 * @brief 查看器播放计时器类。
 * 用于管理和计算查看器的播放时间。
 */
class ViewerPlaybackTimer {
 public:
  /**
   * @brief 启动计时器。
   * @param start_timestamp 播放开始的时间戳。
   * @param playback_speed 播放速度。
   * @param timebase 时间基准，用于将毫秒转换为时间戳单位。
   */
  void Start(const int64_t& start_timestamp, const int& playback_speed, const double& timebase);

  /**
   * @brief 获取当前的时间戳。
   * 根据开始时间、已过时间和播放速度计算当前的时间戳。
   * @return 返回计算得到的当前时间戳。
   */
  [[nodiscard]] int64_t GetTimestampNow() const;

 private:
  /**
   * @brief 计时器启动时的毫秒数（基于 QTime::elapsed()）。
   */
  qint64 start_msec_;
  /**
   * @brief 播放开始时的初始时间戳。
   */
  int64_t start_timestamp_;

  /**
   * @brief 播放速度。
   * 例如，1 表示正常速度，2 表示两倍速。
   */
  int playback_speed_;

  /**
   * @brief 时间基准。
   * 用于将毫秒转换为与时间戳相同的单位。
   */
  double timebase_;
};

}  // namespace olive

#endif  // VIEWERPLAYBACKTIMER_H