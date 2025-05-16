#ifndef AUDIOWAVEFORMCACHE_H  // 防止头文件被重复包含的宏
#define AUDIOWAVEFORMCACHE_H  // 定义 AUDIOWAVEFORMCACHE_H 宏

#include "audio/audiovisualwaveform.h"  // 包含 AudioVisualWaveform 类的定义
#include "playbackcache.h"              // 包含 PlaybackCache 基类的定义

// 假设 AudioParams, TimeRange, TimeRangeList, rational, QPainter, QRect
// 已通过上述 include 或其他方式被间接包含。

namespace olive {  // olive 项目的命名空间

/**
 * @brief AudioWaveformCache 类用于缓存和管理音频波形数据。
 *
 * 它继承自 PlaybackCache，表明它是一个用于回放或显示目的的缓存系统。
 * 这个缓存专门存储预先计算好的音频波形数据 (AudioVisualWaveform)，
 * 以便在UI中 (例如时间轴或音频编辑器) 快速绘制音频的视觉表示。
 *
 * 与 AudioPlaybackCache 存储实际PCM数据不同，这个缓存存储的是用于可视化的波形摘要信息。
 * 它也可能支持从其他缓存 (通过 SetPassthrough) 获取数据或与其同步。
 */
class AudioWaveformCache : public PlaybackCache {  // AudioWaveformCache 继承自 PlaybackCache
 Q_OBJECT                                          // 声明此类使用 Qt 的元对象系统

     public :
     /**
      * @brief 构造函数。
      * @param parent 父对象指针，默认为 nullptr。
      */
     explicit AudioWaveformCache(QObject *parent = nullptr);

  /**
   * @brief 将计算好的音频波形数据写入到指定时间范围的缓存中。
   * @param range 要写入波形数据的时间范围。
   * @param valid_ranges (可能用于指示 `waveform` 中哪些部分是有效的，或与整体缓存有效性相关)。
   * @param waveform 指向包含要写入的音频波形数据的 AudioVisualWaveform 对象的指针。
   */
  void WriteWaveform(const TimeRange &range, const TimeRangeList &valid_ranges, const AudioVisualWaveform *waveform);

  /**
   * @brief 获取当前缓存使用的音频参数。
   * @return 返回 AudioParams 对象的常量引用。
   */
  [[nodiscard]] const AudioParams &GetParameters() const { return params_; }
  /**
   * @brief 设置缓存使用的音频参数。
   * 同时会更新内部 `waveforms_` 对象的通道数。
   * @param p 要设置的 AudioParams 对象。 (如果原始代码是 `&params`，这里注释为 `p` 以匹配常见命名)
   */
  void SetParameters(const AudioParams &p) {  // 如果原始代码是 (const AudioParams &params)，则参数名应为 params 或类似
    params_ = p;
    if (waveforms_) {  // 确保 waveforms_ 已被初始化
      waveforms_->set_channel_count(p.channel_count());
    }
  }

  /**
   * @brief 在给定的 QPainter 上绘制缓存的音频波形。
   * @param painter 用于绘制的 QPainter 对象指针。
   * @param rect 要在其中绘制波形的矩形区域。
   * @param scale 波形绘制的水平缩放比例 (时间轴缩放)。
   * @param start_time 绘制区域左边缘对应的时间点。
   */
  void Draw(QPainter *painter, const QRect &rect, const double &scale, const rational &start_time) const;

  /**
   * @brief 从缓存中获取指定时间范围内的波形摘要信息 (例如，该范围内的最小/最大振幅)。
   * @param start 摘要范围的开始时间。
   * @param length 摘要范围的长度。
   * @return 返回 AudioVisualWaveform::Sample 结构体，包含摘要信息。
   */
  [[nodiscard]] AudioVisualWaveform::Sample GetSummaryFromTime(const rational &start, const rational &length) const;

  /**
   * @brief 获取当前缓存中波形数据的总长度 (以时间单位表示)。
   * @return 返回 rational 类型的长度。
   */
  [[nodiscard]] rational length() const;

  /**
   * @brief (重写 PlaybackCache::SetPassthrough) 设置一个“透传”缓存。
   * 这可能意味着此波形缓存可以从另一个主缓存 (例如 AudioPlaybackCache) 获取原始数据来生成波形，
   * 或者在某些情况下直接显示来自透传缓存的波形 (如果它也存储波形)。
   * @param cache 指向要设置为透传的 PlaybackCache 对象的指针。
   */
  void SetPassthrough(PlaybackCache *cache) override;

 protected:
  /**
   * @brief (重写 PlaybackCache::InvalidateEvent) 当缓存的某个时间范围失效时调用的事件处理函数。
   * 波形缓存需要清除或标记对应范围的波形数据为无效，以便后续重新生成。
   * @param range 失效的时间范围。
   */
  void InvalidateEvent(const TimeRange &range) override;

 private:
  // 类型别名：WaveformPtr 是一个指向 AudioVisualWaveform 对象的共享指针
  using WaveformPtr = std::shared_ptr<AudioVisualWaveform>;

  WaveformPtr waveforms_;  // 存储实际音频波形数据的对象 (可能是一个聚合了所有通道和段的波形对象)

  AudioParams params_;  // 当前缓存使用的音频参数

  /**
   * @brief WaveformPassthrough 内部类，用于在处理透传缓存时，
   *        将一个时间范围与一个特定的波形数据片段关联起来。
   */
  class WaveformPassthrough : public TimeRange {  // 继承自 TimeRange
   public:
    // 构造函数，初始化时间范围
    explicit WaveformPassthrough(const TimeRange &r) : TimeRange(r) {}

    WaveformPtr waveform;  // 指向与此时间范围关联的波形数据的共享指针
  };

  // 存储从透传缓存中获取或生成的波形片段及其对应时间范围的列表
  std::vector<WaveformPassthrough> passthroughs_;
};

}  // namespace olive

#endif  // AUDIOWAVEFORMCACHE_H