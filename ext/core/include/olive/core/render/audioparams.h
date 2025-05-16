#ifndef LIBOLIVECORE_AUDIOPARAMS_H
#define LIBOLIVECORE_AUDIOPARAMS_H

// 以 C 链接方式包含 FFmpeg libavutil 库中的声道布局头文件。
// extern "C"确保C++编译器以C语言的方式处理这个头文件中的声明，
// 这对于链接C库是必需的。
extern "C" {
#include <libavutil/channel_layout.h>
}

#include <assert.h> // 引入 assert.h 用于断言调试
#include <vector>   // 引入 std::vector 用于存储支持的声道布局和采样率列表

#include "../util/rational.h" // 引入 rational 类，用于精确表示时间基准和时间相关的计算
#include "sampleformat.h"     // 引入 SampleFormat 枚举的定义，表示音频采样格式

namespace olive::core { // Olive 核心功能命名空间

/**
 * @brief 封装音频流的各种参数和相关计算。
 *
 * AudioParams 类用于描述音频流的特性，如采样率、声道布局、采样格式、
 * 时长、时间基准等。它还提供了一系列工具函数，用于在时间、
 * 采样数和数据字节大小之间进行转换。
 */
class AudioParams {
 public:
  /**
   * @brief 默认构造函数。
   *
   * 初始化音频参数为无效或默认状态（采样率0，无声道布局，无效格式）。
   * 调用 set_default_footage_parameters() 初始化素材相关参数。
   * 并计算声道数量。
   */
  AudioParams() : sample_rate_(0), channel_layout_(0), format_(SampleFormat::INVALID) {
    set_default_footage_parameters(); // 设置素材相关的默认参数

    // 缓存声道数量
    calculate_channel_count();
  }

  /**
   * @brief 构造函数，使用指定的采样率、声道布局和采样格式初始化。
   * @param sample_rate 音频采样率 (Hz)。
   * @param channel_layout 声道布局的位掩码 (例如 AV_CH_LAYOUT_STEREO)。
   * @param format 音频采样格式 (SampleFormat 枚举)。
   */
  AudioParams(const int& sample_rate, const uint64_t& channel_layout, const SampleFormat& format)
      : sample_rate_(sample_rate), channel_layout_(channel_layout), format_(format) {
    set_default_footage_parameters();         // 设置素材相关的默认参数
    timebase_ = sample_rate_as_time_base(); // 根据采样率设置时间基准

    // 缓存声道数量
    calculate_channel_count();
  }

  /**
   * @brief 获取采样率。
   * @return 返回采样率 (Hz) 的整数值。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int sample_rate() const { return sample_rate_; }

  /**
   * @brief 设置采样率。
   * @param sample_rate 新的采样率 (Hz)。
   */
  void set_sample_rate(int sample_rate) { sample_rate_ = sample_rate; }

  /**
   * @brief 获取声道布局。
   * @return 返回表示声道布局的 uint64_t 位掩码。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] uint64_t channel_layout() const { return channel_layout_; }

  /**
   * @brief 设置声道布局。
   *
   * 设置后会重新计算并缓存声道数量。
   * @param channel_layout 新的声道布局位掩码。
   */
  void set_channel_layout(uint64_t channel_layout) {
    channel_layout_ = channel_layout;
    calculate_channel_count(); // 更新声道数量缓存
  }

  /**
   * @brief 获取音频流的时间基准。
   * @return 返回 rational 类型的时间基准 (通常是 1/采样率)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] rational time_base() const { return timebase_; }

  /**
   * @brief 设置音频流的时间基准。
   * @param timebase 新的时间基准。
   */
  void set_time_base(const rational& timebase) { timebase_ = timebase; }

  /**
   * @brief 将采样率转换为时间基准格式 (1/采样率)。
   * @return 返回 rational 类型的采样率对应的时间基准。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] rational sample_rate_as_time_base() const { return rational(1, sample_rate()); }

  /**
   * @brief 获取音频采样格式。
   * @return 返回 SampleFormat 枚举值。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] SampleFormat format() const { return format_; }

  /**
   * @brief 设置音频采样格式。
   * @param format 新的 SampleFormat 枚举值。
   */
  void set_format(SampleFormat format) { format_ = format; }

  /**
   * @brief 检查此音频流是否启用。
   * @return 如果启用则返回 true，否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool enabled() const { return enabled_; }

  /**
   * @brief 设置音频流的启用状态。
   * @param e true 表示启用，false 表示禁用。
   */
  void set_enabled(bool e) { enabled_ = e; }

  /**
   * @brief 获取音频流在其容器中的索引。
   * @return 返回流索引的整数值。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int stream_index() const { return stream_index_; }

  /**
   * @brief 设置音频流的索引。
   * @param s 新的流索引。
   */
  void set_stream_index(int s) { stream_index_ = s; }

  /**
   * @brief 获取音频流的持续时长（通常以时间基准为单位的采样数或其他媒体特定单位）。
   * @return 返回 int64_t 类型的时长。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int64_t duration() const { return duration_; }

  /**
   * @brief 设置音频流的持续时长。
   * @param duration 新的时长。
   */
  void set_duration(int64_t duration) { duration_ = duration; }

  /**
   * @brief 将时间长度（秒）转换为对应的字节大小。
   * @param time 时间长度 (double 类型，秒)。
   * @return 返回对应的字节数 (int64_t)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int64_t time_to_bytes(const double& time) const;

  /**
   * @brief 将时间长度（rational 类型）转换为对应的字节大小。
   * @param time 时间长度 (rational 类型)。
   * @return 返回对应的字节数 (int64_t)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int64_t time_to_bytes(const rational& time) const;

  /**
   * @brief 将时间长度（秒）转换为每个声道的对应字节大小。
   * @param time 时间长度 (double 类型，秒)。
   * @return 返回每个声道的字节数 (int64_t)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int64_t time_to_bytes_per_channel(const double& time) const;

  /**
   * @brief 将时间长度（rational 类型）转换为每个声道的对应字节大小。
   * @param time 时间长度 (rational 类型)。
   * @return 返回每个声道的字节数 (int64_t)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int64_t time_to_bytes_per_channel(const rational& time) const;

  /**
   * @brief 将时间长度（秒）转换为对应的采样数。
   * @param time 时间长度 (double 类型，秒)。
   * @return 返回对应的采样数 (int64_t)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int64_t time_to_samples(const double& time) const;

  /**
   * @brief 将时间长度（rational 类型）转换为对应的采样数。
   * @param time 时间长度 (rational 类型)。
   * @return 返回对应的采样数 (int64_t)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int64_t time_to_samples(const rational& time) const;

  /**
   * @brief 将采样数转换为对应的总字节大小（所有声道）。
   * @param samples 采样数量。
   * @return 返回对应的字节数 (int64_t)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int64_t samples_to_bytes(const int64_t& samples) const;

  /**
   * @brief 将采样数转换为每个声道的对应字节大小。
   * @param samples 采样数量。
   * @return 返回每个声道的字节数 (int64_t)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int64_t samples_to_bytes_per_channel(const int64_t& samples) const;

  /**
   * @brief 将采样数转换为对应的时间长度。
   * @param samples 采样数量。
   * @return 返回 rational 类型的时间长度。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] rational samples_to_time(const int64_t& samples) const;

  /**
   * @brief 将字节大小转换为对应的采样数。
   * @param bytes 字节数量。
   * @return 返回对应的采样数 (int64_t)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int64_t bytes_to_samples(const int64_t& bytes) const;

  /**
   * @brief 将字节大小转换为对应的时间长度。
   * @param bytes 字节数量。
   * @return 返回 rational 类型的时间长度。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] rational bytes_to_time(const int64_t& bytes) const;

  /**
   * @brief 将每个声道的字节大小转换为对应的时间长度。
   * @param bytes 每个声道的字节数量。
   * @return 返回 rational 类型的时间长度。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] rational bytes_per_channel_to_time(const int64_t& bytes) const;

  /**
   * @brief 获取声道数量。
   * @return 返回声道的数量 (int)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int channel_count() const;

  /**
   * @brief 获取每个采样点在单个声道中所占的字节数。
   * @return 返回每个采样点每个声道的字节数 (int)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int bytes_per_sample_per_channel() const;

  /**
   * @brief 获取每个采样点的位数（例如 16-bit, 24-bit, 32-bit float）。
   * @return 返回每个采样点的位数 (int)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int bits_per_sample() const;

  /**
   * @brief 检查当前音频参数是否有效。
   *
   * 有效性通常取决于采样率是否大于0，采样格式是否有效等。
   * @return 如果参数有效则返回 true，否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool is_valid() const;

  /**
   * @brief 等于运算符重载。
   * @param other 要比较的另一个 AudioParams 对象。
   * @return 如果两个对象的参数都相同，则返回 true。
   */
  bool operator==(const AudioParams& other) const;

  /**
   * @brief 不等于运算符重载。
   * @param other 要比较的另一个 AudioParams 对象。
   * @return 如果两个对象的参数至少有一个不同，则返回 true。
   */
  bool operator!=(const AudioParams& other) const;

  /**
   * @brief 支持的声道布局列表。
   *
   * 静态常量成员，包含应用程序支持的声道布局的 uint64_t 位掩码。
   */
  static const std::vector<uint64_t> kSupportedChannelLayouts;

  /**
   * @brief 支持的采样率列表。
   *
   * 静态常量成员，包含应用程序支持的采样率（Hz）的整数列表。
   */
  static const std::vector<int> kSupportedSampleRates;

 private:
  /**
   * @brief 设置素材相关的默认参数。
   *
   * 将 enabled_ 设为 true，stream_index_ 设为 0，duration_ 设为 0。
   */
  void set_default_footage_parameters() {
    enabled_ = true;      // 默认启用
    stream_index_ = 0;    // 默认流索引为0
    duration_ = 0;        // 默认时长为0
  }

  /**
   * @brief 根据当前的 channel_layout_ 计算并缓存声道数量到 channel_count_。
   */
  void calculate_channel_count();

  int sample_rate_; ///< 音频采样率 (单位: Hz)。
  uint64_t channel_layout_; ///< 声道布局的位掩码 (例如使用 FFmpeg 的 AV_CH_LAYOUT_* 定义)。
  int channel_count_{}; ///< 缓存的声道数量，根据 channel_layout_ 计算得出。
  SampleFormat format_; ///< 音频采样格式 (例如，16位整数，32位浮点数等)。

  // Footage-specific / 素材特定的参数
  // 注意：enabled_ 改为 int 类型是为了修复 GCC 11 的 stringop-overflow 问题，推测是字节对齐问题。
  int enabled_{};       ///< 标记此音频流是否启用。使用 int 而非 bool 可能为了解决特定的编译器/对齐问题。
  int stream_index_{};  ///< 音频流在其媒体容器中的索引。
  int64_t duration_{};  ///< 音频流的总时长 (单位通常由 timebase_ 决定，例如总采样数)。
  rational timebase_;   ///< 音频流的时间基准 (例如，对于采样率为48000Hz的音频，时间基准可以是1/48000)。
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_AUDIOPARAMS_H
