

#ifndef LIBOLIVECORE_TIMECODEFUNCTIONS_H
#define LIBOLIVECORE_TIMECODEFUNCTIONS_H

#include "rational.h"

namespace olive::core {

/**
 * @brief Functions for converting times/timecodes/timestamps
 *
 * Olive uses the following terminology through its code:
 *
 * `time` - time in seconds presented in a rational form
 * `timebase` - the base time unit of an audio/video stream in seconds
 * `timestamp` - an integer representation of a time in timebase units (in many cases is used like a frame number)
 * `timecode` a user-friendly string representation of a time according to Timecode::Display
 */
class Timecode {
 public:
  enum Display { kTimecodeDropFrame, kTimecodeNonDropFrame, kTimecodeSeconds, kFrames, kMilliseconds };

  enum Rounding { kCeil, kFloor, kRound };

  /**
   * @brief Convert a timestamp (according to a rational timebase) to a user-friendly string representation
   */
  static std::string time_to_timecode(const rational& time, const rational& timebase, const Display& display,
                                      bool show_plus_if_positive = false);
  static rational timecode_to_time(std::string timecode, const rational& timebase, const Display& display,
                                   bool* ok = nullptr);

  static std::string time_to_string(int64_t ms);

  static rational snap_time_to_timebase(const rational& time, const rational& timebase, Rounding floor = kRound);

  static int64_t time_to_timestamp(const rational& time, const rational& timebase, Rounding floor = kRound);
  static int64_t time_to_timestamp(const double& time, const rational& timebase, Rounding floor = kRound);

  static int64_t rescale_timestamp(const int64_t& ts, const rational& source, const rational& dest);
  static int64_t rescale_timestamp_ceil(const int64_t& ts, const rational& source, const rational& dest);

  static rational timestamp_to_time(const int64_t& timestamp, const rational& timebase);

  static bool timebase_is_drop_frame(const rational& timebase);
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_TIMECODEFUNCTIONS_H
