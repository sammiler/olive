#ifndef AUDIOPLAYBACKCACHE_H  // 防止头文件被重复包含的宏
#define AUDIOPLAYBACKCACHE_H  // 定义 AUDIOPLAYBACKCACHE_H 宏

#include "audio/audiovisualwaveform.h"  // 包含与音频/视觉波形相关的定义 (可能间接包含 SampleBuffer, AudioParams, TimeRange 等)
#include "render/playbackcache.h"  // 包含 PlaybackCache 基类的定义

// 假设 SampleBuffer, AudioParams, TimeRange, TimeRangeList, rational 等类型
// 已通过上述 include 或其他方式被间接包含。

namespace olive {  // olive 项目的命名空间

/**
 * @brief AudioPlaybackCache 类是一个用于存储和播放在 Olive 中已缓存音频的完整集成系统。
 *
 * Olive 中的所有音频都会被预先处理和渲染。这样做可以使播放极其流畅和可靠，
 * 但在存储和操作这些音频，同时最小化必要的重新渲染次数方面带来了一些挑战。
 *
 * Olive 的 PlaybackCache 支持“移位”(shifting)操作；即在时间轴上移动缓存数据到不同的位置，
 * 而无需进行昂贵的重新渲染。虽然视频在磁盘上自然地存储为易于替换的单独帧，
 * 但音频的工作方式略有不同。将每个样本存储为磁盘上的单独文件会非常低效，
 * 但将所有音频存储在一个连续的文件中则不利于移位操作，特别是对于较长的时间轴，
 * 因为数据将不得不在磁盘上实际移动。
 *
 * 因此，AudioPlaybackCache 采取了折衷方案，将音频存储在多个“段”(segments)中。
 * 这使得像移位这样的操作变得容易得多，因为可以在播放列表中简单地移除段，
 * 而不必在每次操作中移动或重新渲染可能长达数小时的音频。
 *
 * 当然，分段存储意味着不能像播放文件一样简单地播放PCM数据，所以
 * AudioPlaybackCache 还提供了一个播放设备 (可通过 CreatePlaybackDevice() 访问，尽管此方法未在此头文件中声明)，
 * 该设备的行为与基于文件的IO设备完全相同，它会透明地将各个段连接在一起，
 * 表现得像一个连续的文件。
 * (注意：描述中提及的 CreatePlaybackDevice() 未在此头文件声明，可能在基类或 .cpp 文件中实现)
 */
class AudioPlaybackCache : public PlaybackCache {  // AudioPlaybackCache 继承自 PlaybackCache
 Q_OBJECT                                          // 声明此类使用 Qt 的元对象系统

     public :
     /**
      * @brief 构造函数。
      * @param parent 父对象指针，默认为 nullptr。
      */
     explicit AudioPlaybackCache(QObject *parent = nullptr);

  // 析构函数
  ~AudioPlaybackCache() override;

  /**
   * @brief 获取当前缓存使用的音频参数 (如采样率、通道数等)。
   * @return 返回 AudioParams 对象。
   */
  AudioParams GetParameters() { return params_; }

  /**
   * @brief 设置缓存使用的音频参数。
   * 这通常在开始缓存新的音频流之前调用，如果参数改变，可能需要清除旧的缓存。
   * @param params 要设置的 AudioParams 对象。
   */
  void SetParameters(const AudioParams &params);

  /**
   * @brief 将 PCM 音频样本数据写入到指定时间范围的缓存中。
   * @param range 要写入数据的时间范围。
   * @param valid_ranges (可能用于指示 `samples` 中哪些部分是有效的，或与整体缓存有效性相关)。
   * @param samples 包含要写入的音频样本数据的 SampleBuffer。
   */
  void WritePCM(const TimeRange &range, const TimeRangeList &valid_ranges, const SampleBuffer &samples);

  /**
   * @brief 在指定的时间范围写入静音数据到缓存中。
   * @param range 要写入静音的时间范围。
   */
  void WriteSilence(const TimeRange &range);

 private:
  /**
   * @brief 从给定的 SampleBuffer 中写入一部分数据到缓存。
   * 这是一个内部辅助函数，用于将大块的 SampleBuffer 分段写入。
   * @param samples 包含源音频样本的 SampleBuffer。
   * @param write_start 缓存中开始写入的时间点。
   * @param buffer_start SampleBuffer 中开始读取数据的时间点 (相对于 SampleBuffer 的开始)。
   * @param length 要写入的音频数据长度 (以时间单位表示)。
   * @return 如果写入成功，返回 true。
   */
  bool WritePartOfSampleBuffer(const SampleBuffer &samples, const rational &write_start, const rational &buffer_start,
                               const rational &length);

  /**
   * @brief 根据段索引和通道号获取对应的缓存段文件名。
   * @param segment_index 段的索引。
   * @param channel 通道号 (对于多通道音频，每个通道可能存储在单独的文件或段中)。
   * @return 返回生成的段文件名字符串。
   */
  QString GetSegmentFilename(qint64 segment_index, int channel);

  // 每个通道的默认音频段大小 (可能是样本数或字节数，具体单位需看实现)
  static const qint64 kDefaultSegmentSizePerChannel;

  AudioParams params_;  // 当前缓存使用的音频参数
};

}  // namespace olive

#endif  // AUDIOPLAYBACKCACHE_H