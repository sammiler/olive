

#ifndef AUDIOVISUALWAVEFORM_H // 假设宏名称与类名对应
#define AUDIOVISUALWAVEFORM_H

#include <olive/core/core.h>
#include <QPainter>
#include <vector>
#include <map>

namespace olive {

using namespace core;

// 前向声明 (如果 SampleBuffer 来自 core 命名空间且未完全包含)
// class core::SampleBuffer;

/**
 * @brief 存储音频的可视化表示形式的数据缓冲区。
 *
 * 此类与 SampleBuffer 不同，AudioVisualWaveform 中的数据经过了显著简化
 * 和优化，以便于高效的视觉显示。它通常包含每个时间间隔内音频样本的
 * 最小值和最大值，用于绘制波形图。
 */
class AudioVisualWaveform {
 public:
  /**
   * @brief 构造一个空的 AudioVisualWaveform 对象。
   */
  AudioVisualWaveform();

  /**
   * @brief 每个通道的样本数据结构，包含最小值和最大值。
   */
  struct SamplePerChannel {
    float min; ///< @brief 当前样本区间的最小值。
    float max; ///< @brief 当前样本区间的最大值。
  };

  /**
   * @brief AudioVisualWaveform 内部存储的样本数据类型。
   * 它是一个 SamplePerChannel 结构的 std::vector，每个元素代表一个时间点的多通道样本摘要。
   */
  using Sample = std::vector<SamplePerChannel>;

  /**
   * @brief 获取音频通道的数量。
   * @return int 通道数。
   */
  [[nodiscard]] int channel_count() const { return channels_; }

  /**
   * @brief 设置音频通道的数量。
   * @param channels 要设置的通道数。
   */
  void set_channel_count(int channels) { channels_ = channels; }

  /**
   * @brief 获取波形的总长度（以时间单位表示）。
   * @return const rational& 波形长度。
   */
  [[nodiscard]] const rational &length() const { return length_; }

  /**
   * @brief 将实际的音频样本数据写入到可视化波形缓冲区中。
   *
   * 从指定的 `start` 时间点开始，用新的样本数据覆盖缓冲区中的任何现有数据。
   * 如果需要，会自动扩展缓冲区以容纳新的样本。
   * @param samples 包含原始音频样本的 SampleBuffer 对象。
   * @param sample_rate 原始音频样本的采样率。
   * @param start 波形中开始写入样本的时间点，默认为0。
   */
  void OverwriteSamples(const SampleBuffer &samples, int sample_rate, const rational &start = rational(0));

  /**
   * @brief 替换当前可视化波形中特定范围的摘要数据。
   * @param sums 包含要写入的新摘要数据的 AudioVisualWaveform 对象。
   * @param dest 当前波形中开始写入新摘要数据的目标时间点。
   * @param offset 从 `sums` 参数的哪个时间点开始读取数据，默认为0。
   * @param length 要从 `sums` 中读取并覆盖的最大长度，如果为0或超出范围则处理至 `sums` 末尾或当前波形末尾。
   */
  void OverwriteSums(const AudioVisualWaveform &sums, const rational &dest, const rational &offset = rational(0),
                     const rational &length = rational(0));

  /**
   * @brief 用静音数据覆盖波形中的指定时间范围。
   * @param start 开始覆盖静音的时间点。
   * @param length 要覆盖静音的持续长度。
   */
  void OverwriteSilence(const rational &start, const rational &length);

  /**
   * @brief 从波形的起始位置裁剪掉指定长度的数据。
   * @param length 要裁剪掉的长度。
   */
  void TrimIn(rational length);

  /**
   * @brief 从指定偏移量开始，截取波形的剩余部分。
   * @param offset 截取的起始时间点。
   * @return AudioVisualWaveform 截取后的新波形对象。
   */
  [[nodiscard]] AudioVisualWaveform Mid(const rational &offset) const;

  /**
   * @brief 从指定偏移量开始，截取指定长度的波形部分。
   * @param offset 截取的起始时间点。
   * @param length 要截取的长度。
   * @return AudioVisualWaveform 截取后的新波形对象。
   */
  [[nodiscard]] AudioVisualWaveform Mid(const rational &offset, const rational &length) const;

  /**
   * @brief 调整波形的总长度。
   * 如果新长度小于当前长度，则波形将被截断。
   * 如果新长度大于当前长度，则波形末尾可能会填充静音或未定义数据，具体取决于实现。
   * @param length 新的波形总长度。
   */
  void Resize(const rational &length);

  /**
   * @brief 从波形中移除（裁剪掉）指定时间范围的数据。
   * @param in 开始裁剪的时间点。
   * @param length 要裁剪的长度。
   */
  void TrimRange(const rational &in, const rational &length);

  /**
   * @brief 从指定的时间范围获取音频摘要样本。
   * @param start 获取摘要的起始时间点。
   * @param length 获取摘要的持续长度。
   * @return Sample 包含所请求范围内摘要数据的 Sample 对象。
   */
  [[nodiscard]] Sample GetSummaryFromTime(const rational &start, const rational &length) const;

  /**
   * @brief 根据给定的原始音频样本数据计算摘要信息（最小/最大值）。
   * @param samples 包含原始音频样本的 SampleBuffer。
   * @param start_index 开始处理的样本在其缓冲区中的起始索引。
   * @param length 要处理的样本数量。
   * @return Sample 计算得到的摘要样本数据。
   */
  static Sample SumSamples(const SampleBuffer &samples, size_t start_index, size_t length);

  /**
   * @brief 根据已有的每通道样本摘要数据重新计算（或聚合）摘要信息。
   * @param samples 指向 SamplePerChannel 数据数组的指针。
   * @param nb_samples 样本的数量。
   * @param nb_channels 通道的数量。
   * @return Sample 重新计算或聚合后的摘要样本数据。
   */
  static Sample ReSumSamples(const SamplePerChannel *samples, size_t nb_samples, int nb_channels);

  /**
   * @brief 在给定的 QPainter 上绘制单个音频摘要样本。
   * @param painter 用于绘制的 QPainter 对象指针。
   * @param sample 要绘制的 Sample 数据。
   * @param x 绘制区域的 x 坐标。
   * @param y 绘制区域的 y 坐标。
   * @param height 绘制区域的高度。
   * @param rectified 是否以校正模式绘制（例如，所有值都为正）。
   */
  static void DrawSample(QPainter *painter, const Sample &sample, int x, int y, int height, bool rectified);

  /**
   * @brief 在给定的 QPainter 上绘制整个音频波形。
   * @param painter 用于绘制的 QPainter 对象指针。
   * @param rect 指定绘制波形的矩形区域。
   * @param scale 波形显示的时间轴缩放比例。
   * @param samples 包含要绘制波形数据的 AudioVisualWaveform 对象。
   * @param start_time 波形数据在时间轴上的起始时间。
   */
  static void DrawWaveform(QPainter *painter, const QRect &rect, const double &scale,
                           const AudioVisualWaveform &samples, const rational &start_time);

  /**
   * @brief 定义了用于生成波形摘要的最低有效采样率。
   * 注意：此值通常应为2的幂，以优化mipmap生成。
   */
  static const rational kMinimumSampleRate;
  /**
   * @brief 定义了用于生成波形摘要的最高有效采样率。
   */
  static const rational kMaximumSampleRate;

 private:
  /**
   * @brief 从 SampleBuffer 覆盖样本数据到内部数据结构。
   * @param samples 输入的 SampleBuffer。
   * @param sample_rate 输入样本的采样率。
   * @param start 在此波形中开始写入的时间点。
   * @param target_rate 目标 mipmap 级别的采样率。
   * @param data 目标 mipmap 级别的样本数据容器（输出）。
   * @param start_index 处理的起始索引（输出）。
   * @param samples_length 处理的样本长度（输出）。
   */
  void OverwriteSamplesFromBuffer(const SampleBuffer &samples, int sample_rate, const rational &start,
                                  double target_rate, Sample &data, size_t &start_index, size_t &samples_length);

  /**
   * @brief 从一个已存在的 mipmap 级别覆盖样本数据到另一个 mipmap 级别。
   * @param input 输入的 mipmap 样本数据。
   * @param input_sample_rate 输入 mipmap 的采样率。
   * @param input_start 输入 mipmap 中开始读取的索引（引用，会被修改）。
   * @param input_length 从输入 mipmap 读取的长度（引用，会被修改）。
   * @param start 在输出 mipmap 中开始写入的时间点。
   * @param output_rate 输出 mipmap 的目标采样率。
   * @param output_data 输出 mipmap 的样本数据容器（输出）。
   */
  void OverwriteSamplesFromMipmap(const Sample &input, double input_sample_rate, size_t &input_start,
                                  size_t &input_length, const rational &start, double output_rate, Sample &output_data);

  /**
   * @brief 将时间点转换为相对于给定采样率的样本索引。
   * @param time 要转换的时间点 (rational 类型)。
   * @param sample_rate 参考的采样率。
   * @return size_t 对应的样本索引。
   */
  [[nodiscard]] size_t time_to_samples(const rational &time, double sample_rate) const;

  /**
   * @brief 将时间点转换为相对于给定采样率的样本索引。
   * @param time 要转换的时间点 (double 类型)。
   * @param sample_rate 参考的采样率。
   * @return size_t 对应的样本索引。
   */
  [[nodiscard]] size_t time_to_samples(const double &time, double sample_rate) const;

  /**
   * @brief 根据给定的显示缩放比例获取最合适的 mipmap 数据。
   * @param scale 显示的缩放比例。
   * @return std::map<rational, Sample>::const_iterator 指向最佳 mipmap 数据的迭代器。
   */
  [[nodiscard]] std::map<rational, Sample>::const_iterator GetMipmapForScale(double scale) const;

  /**
   * @brief 验证并可能调整虚拟起始时间。
   * @param new_start 新的虚拟起始时间建议值。
   */
  void ValidateVirtualStart(const rational &new_start);

  /**
   * @brief 波形数据的虚拟起始时间，用于处理裁剪和偏移。
   */
  rational virtual_start_;

  /**
   * @brief 音频的通道数量。
   */
  int channels_;

  /**
   * @brief 存储不同缩放级别（mipmap）的波形摘要数据。
   * 键 (rational) 通常代表该 mipmap 级别的采样率或等效的时间单位。
   * 值 (Sample) 是该级别的摘要样本数据。
   */
  std::map<rational, Sample> mipmapped_data_;

  /**
   * @brief 波形的总长度（以时间单位表示）。
   */
  rational length_;
};

}  // namespace olive

Q_DECLARE_METATYPE(olive::AudioVisualWaveform)

#endif  // AUDIOVISUALWAVEFORM_H