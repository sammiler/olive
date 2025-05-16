#ifndef LIBOLIVECORE_TIMECODEFUNCTIONS_H
#define LIBOLIVECORE_TIMECODEFUNCTIONS_H

#include <string> // 引入 std::string 用于时间码字符串表示
#include <cstdint> // 引入 int64_t 等固定宽度整数类型

#include "rational.h" // 引入 rational 类，用于精确表示时间值和时间基准

namespace olive::core { // Olive 核心功能命名空间

// 前向声明 rational 类，如果 "rational.h" 中已完整定义则无需再次声明
// class rational;

/**
 * @brief 提供用于转换时间、时间码和时间戳的函数集合。
 *
 * Olive 在其代码中使用以下术语：
 *
 * `time`     - 以有理数形式表示的秒为单位的时间 (例如 rational(3,1) 表示3秒)。
 * `timebase` - 音频/视频流的基本时间单位（以秒为单位的有理数，例如 1/24 表示24fps视频的时间基准）。
 * `timestamp`- 以时间基准为单位的时间的整数表示 (在许多情况下类似于帧号)。
 * `timecode` - 根据 Timecode::Display 枚举定义的时间的用户友好字符串表示形式 (例如 "00:00:10:05")。
 */
class Timecode {
 public:
  /**
   * @brief 定义时间码的显示格式。
   */
  enum Display {
    kTimecodeDropFrame,   ///< 丢帧时间码格式 (例如 NTSC 视频常用的 29.97fps)。
    kTimecodeNonDropFrame,///< 非丢帧时间码格式。
    kTimecodeSeconds,     ///< 以纯秒数显示 (例如 "10.5s")。
    kFrames,              ///< 以帧数显示。
    kMilliseconds         ///< 以毫秒数显示。
  };

  /**
   * @brief 定义时间转换时的取整方式。
   */
  enum Rounding {
    kCeil,  ///< 向上取整 (取不小于目标值的最小整数)。
    kFloor, ///< 向下取整 (取不大于目标值的最大整数)。
    kRound  ///< 四舍五入到最近的整数。
  };

  /**
   * @brief 将以秒为单位的时间（根据有理数时间基准）转换为用户友好的时间码字符串表示。
   * @param time 要转换的时间 (rational 类型，单位秒)。
   * @param timebase 时间流的时间基准 (rational 类型)。
   * @param display 时间码的显示格式 (Display 枚举)。
   * @param show_plus_if_positive 如果为 true 且时间为正，则在结果字符串前添加 "+" 号，默认为 false。
   * @return 返回格式化后的时间码字符串 (std::string)。
   */
  static std::string time_to_timecode(const rational& time, const rational& timebase, const Display& display,
                                      bool show_plus_if_positive = false);

  /**
   * @brief 将用户友好的时间码字符串表示转换为以秒为单位的时间（rational 类型）。
   * @param timecode 要转换的时间码字符串。
   * @param timebase 时间流的时间基准 (rational 类型)。
   * @param display 时间码字符串的原始显示格式 (Display 枚举)。
   * @param ok (可选) 一个布尔输出参数，指示转换是否成功。如果转换失败，*ok 会被设置为 false。
   * @return 返回转换得到的以秒为单位的时间 (rational 类型)。如果转换失败，可能返回一个无效的 rational (如 NaN)。
   */
  static rational timecode_to_time(std::string timecode, const rational& timebase, const Display& display,
                                   bool* ok = nullptr);

  /**
   * @brief 将毫秒数转换为格式化的时间字符串 (例如 "HH:MM:SS.mmm")。
   * @param ms 以毫秒为单位的时间长度 (int64_t)。
   * @return 返回格式化的时间字符串 (std::string)。
   */
  static std::string time_to_string(int64_t ms);

  /**
   * @brief 将给定的时间值对齐（吸附）到指定时间基准的最近整数倍。
   * @param time 要对齐的时间 (rational 类型)。
   * @param timebase 目标时间基准 (rational 类型)。
   * @param floor 取整方式 (Rounding 枚举)，默认为 kRound (四舍五入)。
   * @return 返回对齐到时间基准后的时间值 (rational 类型)。
   */
  static rational snap_time_to_timebase(const rational& time, const rational& timebase, Rounding floor = kRound);

  /**
   * @brief 将以秒为单位的时间（rational）转换为以指定时间基准为单位的时间戳（整数）。
   * @param time 要转换的时间 (rational 类型，单位秒)。
   * @param timebase 时间基准 (rational 类型)。
   * @param floor 取整方式 (Rounding 枚举)，默认为 kRound。
   * @return 返回计算得到的时间戳 (int64_t)。
   */
  static int64_t time_to_timestamp(const rational& time, const rational& timebase, Rounding floor = kRound);

  /**
   * @brief 将以秒为单位的时间（double）转换为以指定时间基准为单位的时间戳（整数）。
   * @param time 要转换的时间 (double 类型，单位秒)。
   * @param timebase 时间基准 (rational 类型)。
   * @param floor 取整方式 (Rounding 枚举)，默认为 kRound。
   * @return 返回计算得到的时间戳 (int64_t)。
   */
  static int64_t time_to_timestamp(const double& time, const rational& timebase, Rounding floor = kRound);

  /**
   * @brief 将一个时间戳从源时间基准重新缩放到目标时间基准。
   * @param ts 原始时间戳。
   * @param source 源时间基准 (rational 类型)。
   * @param dest 目标时间基准 (rational 类型)。
   * @return 返回在目标时间基准下的新时间戳 (int64_t)，使用默认取整（通常是截断或向下取整）。
   */
  static int64_t rescale_timestamp(const int64_t& ts, const rational& source, const rational& dest);

  /**
   * @brief 将一个时间戳从源时间基准重新缩放到目标时间基准，并向上取整。
   * @param ts 原始时间戳。
   * @param source 源时间基准 (rational 类型)。
   * @param dest 目标时间基准 (rational 类型)。
   * @return 返回在目标时间基准下的新时间戳 (int64_t)，结果向上取整。
   */
  static int64_t rescale_timestamp_ceil(const int64_t& ts, const rational& source, const rational& dest);

  /**
   * @brief 将以时间基准为单位的时间戳（整数）转换回以秒为单位的时间（rational 类型）。
   * @param timestamp 要转换的时间戳。
   * @param timebase 时间基准 (rational 类型)。
   * @return 返回转换得到的以秒为单位的时间 (rational 类型)。
   */
  static rational timestamp_to_time(const int64_t& timestamp, const rational& timebase);

  /**
   * @brief 检查给定的时间基准是否为丢帧格式。
   *
   * 丢帧时间码用于某些特定帧率（如29.97fps NTSC）以使其时间码显示与实际时钟时间大致同步。
   * @param timebase 要检查的时间基准 (rational 类型)。
   * @return 如果时间基准是已知的丢帧格式，则返回 true；否则返回 false。
   */
  static bool timebase_is_drop_frame(const rational& timebase);
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_TIMECODEFUNCTIONS_H
