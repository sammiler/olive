#ifndef LIBOLIVECORE_SAMPLEBUFFER_H
#define LIBOLIVECORE_SAMPLEBUFFER_H

#include <memory>  // 引入智能指针等内存管理工具 (虽然在此文件中未直接使用，但可能是大型项目的一部分)
#include <vector>  // 引入 std::vector，用于存储每个声道的采样数据

#include "../util/rational.h"  // 引入 rational 类，用于精确表示时间长度
#include "audioparams.h"       // 引入 AudioParams 类的定义，SampleBuffer 与音频参数紧密相关

namespace olive::core {  // Olive 核心功能命名空间

/**
 * @brief 存储音频采样的缓冲区。
 *
 * 此结构中的音频采样始终以平面（PLANAR）格式存储（按声道分离）。
 * 这样做是为了简化音频渲染代码。这取代了旧的使用 QByteArray（包含交错音频）的系统。
 * 虽然 SampleBuffer 在渲染/处理方面取代了许多 QByteArray 的使用场景，
 * 但 QByteArray 目前仍用于播放，包括从缓存中读取和写入。
 * 音频数据通常存储为浮点数 (float)。
 */
class SampleBuffer {
 public:
  /**
   * @brief 默认构造函数。
   *
   * 创建一个空的、未分配的 SampleBuffer。音频参数和采样数将处于默认或无效状态。
   */
  SampleBuffer();

  /**
   * @brief 构造函数，根据音频参数和指定的长度（时间）创建 SampleBuffer。
   * @param audio_params 音频参数对象，定义了采样率、格式、声道等。
   * @param length 音频缓冲区的期望长度 (rational 类型，通常表示秒)。
   * 实际的采样数将根据 audio_params 中的采样率计算得出。
   */
  SampleBuffer(AudioParams audio_params, const rational& length);

  /**
   * @brief 构造函数，根据音频参数和每个声道的采样数创建 SampleBuffer。
   * @param audio_params 音频参数对象。
   * @param samples_per_channel 每个声道的采样数量。
   */
  SampleBuffer(AudioParams audio_params, size_t samples_per_channel);

  /**
   * @brief 获取当前 SampleBuffer 的音频参数。
   * @return 返回 AudioParams 对象的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const AudioParams& audio_params() const;

  /**
   * @brief 设置 SampleBuffer 的音频参数。
   *
   * 注意：更改音频参数后，可能需要重新分配或调整内部数据缓冲区。
   * @param params 新的 AudioParams 对象。
   */
  void set_audio_params(const AudioParams& params);

  /**
   * @brief 获取每个声道的采样数量。
   * @return 返回每个声道采样数的常量引用 (size_t 类型)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const size_t& sample_count() const { return sample_count_per_channel_; }

  /**
   * @brief 设置每个声道的采样数量。
   *
   * 此操作可能会导致内部数据缓冲区的重新分配。
   * @param sample_count 新的每个声道的采样数量。
   */
  void set_sample_count(const size_t& sample_count);

  /**
   * @brief 根据指定的长度（时间）设置每个声道的采样数量。
   *
   * 内部会使用 audio_params_ 将时间长度转换为采样数。
   * @param length 音频缓冲区的期望长度 (rational 类型)。
   */
  void set_sample_count(const rational& length) { set_sample_count(audio_params_.time_to_samples(length)); }

  /**
   * @brief 获取指定声道的原始浮点数据指针（可修改）。
   * @param channel 声道索引（从0开始）。
   * @return 返回指向该声道采样数据开头的 float 指针。
   * 如果声道索引无效或缓冲区未分配，行为未定义（取决于 std::vector::operator[]）。
   */
  float* data(int channel) { return data_[channel].data(); }

  /**
   * @brief 获取指定声道的原始浮点数据指针（不可修改）。
   * @param channel 声道索引（从0开始）。
   * @return 返回指向该声道采样数据开头的 const float 指针。
   * 如果声道索引无效或缓冲区未分配，行为未定义（取决于 std::vector::at()，可能会抛出异常）。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const float* data(int channel) const { return data_.at(channel).data(); }

  /**
   * @brief 将内部存储的每个声道的数据转换为一个原始浮点指针的向量。
   *
   * 这对于需要以 C 风格数组指针形式访问所有声道数据的外部库或函数可能很有用。
   * @return 返回一个 std::vector<float*>，其中每个元素指向对应声道数据的开头。
   */
  std::vector<float*> to_raw_ptrs() {
    std::vector<float*> r(data_.size());  // 创建一个大小与声道数相同的指针向量
    for (size_t i = 0; i < r.size(); i++) {
      r[i] = data_[i].data();  // 将每个声道的 data() 指针存入向量
    }
    return r;
  }

  /**
   * @brief 获取缓冲区的声道数量。
   * @return 返回声道的数量 (int)，即内部 data_ 向量的大小。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int channel_count() const { return data_.size(); }

  /**
   * @brief 检查内部数据缓冲区是否已分配内存。
   * @return 如果 data_ 向量不为空（即已为声道分配了子向量），则返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool is_allocated() const { return !data_.empty(); }

  /**
   * @brief 根据当前的音频参数和采样数分配内部数据缓冲区。
   *
   * 如果已分配，可能会先销毁现有数据。
   */
  void allocate();

  /**
   * @brief 销毁（释放）内部数据缓冲区，并重置相关状态。
   */
  void destroy();

  /**
   * @brief 反转缓冲区中所有声道的音频采样顺序。
   */
  void reverse();

  /**
   * @brief 改变音频的播放速度（通过重采样或丢弃/复制采样实现，具体取决于实现）。
   * @param speed 速度因子。大于1加速，小于1减速。
   */
  void speed(double speed);

  /**
   * @brief 对所有声道的音量进行变换（乘以一个因子）。
   * @param f 音量调整因子。
   */
  void transform_volume(float f);

  /**
   * @brief 对指定声道的音量进行变换。
   * @param channel 目标声道索引。
   * @param volume 音量调整因子。
   */
  void transform_volume_for_channel(int channel, float volume);

  /**
   * @brief 对所有声道中指定采样索引处的音量进行变换。
   * @param sample_index 目标采样的索引。
   * @param volume 音量调整因子。
   */
  void transform_volume_for_sample(size_t sample_index, float volume);

  /**
   * @brief 对指定声道中指定采样索引处的音量进行变换。
   * @param sample_index 目标采样的索引。
   * @param channel 目标声道索引。
   * @param volume 音量调整因子。
   */
  void transform_volume_for_sample_on_channel(size_t sample_index, int channel, float volume);

  /**
   * @brief 对所有声道的采样值进行限幅处理（通常限制在 [-1.0, 1.0] 范围内）。
   */
  void clamp();

  /**
   * @brief 将缓冲区中的所有采样静音（设置为0）。
   */
  void silence();

  /**
   * @brief 将缓冲区中指定采样范围内的采样静音。
   * @param start_sample 静音范围的起始采样索引（包含）。
   * @param end_sample 静音范围的结束采样索引（不包含）。
   */
  void silence(size_t start_sample, size_t end_sample);

  /**
   * @brief 将缓冲区中指定字节范围内的采样静音。
   *
   * 这个方法可能需要根据字节偏移量计算出对应的采样点进行操作。
   * @param start_byte 静音范围的起始字节偏移。
   * @param end_byte 静音范围的结束字节偏移。
   */
  void silence_bytes(size_t start_byte, size_t end_byte);

  /**
   * @brief 将外部数据设置（拷贝）到指定声道的指定位置。
   * @param channel 目标声道索引。
   * @param data 指向源浮点数据的指针。
   * @param sample_offset 目标声道中开始写入的采样偏移量。
   * @param sample_length 要从源数据中拷贝的采样数量。
   */
  void set(int channel, const float* data, size_t sample_offset, size_t sample_length);

  /**
   * @brief 将外部数据设置（拷贝）到指定声道的开头。
   * @param channel 目标声道索引。
   * @param data 指向源浮点数据的指针。
   * @param sample_length 要从源数据中拷贝的采样数量。
   */
  void set(int channel, const float* data, size_t sample_length) { set(channel, data, 0, sample_length); }

 private:
  /**
   * @brief 对指定声道的采样值进行限幅处理。
   * @param channel 目标声道索引。
   */
  void clamp_channel(int channel);

  AudioParams audio_params_;  ///< 存储此缓冲区的音频参数（采样率、格式、声道布局等）。

  size_t sample_count_per_channel_;  ///< 每个声道的采样数量。

  /**
   * @brief 存储实际音频数据的二维向量。
   * 外层向量的每个元素代表一个声道。
   * 内层向量存储该声道的浮点采样数据。
   * 数据总是以平面（planar）格式存储。
   */
  std::vector<std::vector<float> > data_;
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_SAMPLEBUFFER_H
