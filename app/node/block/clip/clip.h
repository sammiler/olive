#ifndef CLIPBLOCK_H
#define CLIPBLOCK_H

#include "audio/audiovisualwaveform.h"
#include "codec/decoder.h"
#include "node/block/block.h"
#include "node/input/multicam/multicamnode.h"
#include "node/output/track/track.h"

namespace olive {

class ViewerOutput; // 前向声明 ViewerOutput 类，用于显示输出

/**
 * @brief 代表媒体块（通常是视频或音频片段）的节点。
 * 继承自 Block 类，是时间线上的基本构成单元。
 */
class ClipBlock : public Block {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief ClipBlock 构造函数。
   */
  ClipBlock();

  NODE_DEFAULT_FUNCTIONS(ClipBlock) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此 ClipBlock 的名称。
   * @return QString 类型的节点名称。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此 ClipBlock 的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此 ClipBlock 的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 设置片段长度并调整媒体出点。
   * @param length 新的片段长度（以 rational 类型表示）。
   */
  void set_length_and_media_out(const rational &length) override;
  /**
   * @brief 设置片段长度并调整媒体入点。
   * @param length 新的片段长度（以 rational 类型表示）。
   */
  void set_length_and_media_in(const rational &length) override;

  /**
   * @brief 获取此片段所在轨道的类型。
   * @return Track::Type 枚举，表示轨道类型（如视频、音频等），如果片段不在轨道上则返回 Track::kNone。
   */
  [[nodiscard]] Track::Type GetTrackType() const {
    if (track()) { // 检查是否存在关联的轨道
      return track()->type(); // 返回轨道的类型
    } else {
      return Track::kNone; // 没有轨道则返回 kNone
    }
  }

  /**
   * @brief 获取媒体的入点时间（在源媒体文件中的时间）。
   * @return rational 类型的媒体入点时间。
   */
  [[nodiscard]] rational media_in() const;
  /**
   * @brief 设置媒体的入点时间。
   * @param media_in rational 类型的媒体入点时间。
   */
  void set_media_in(const rational &media_in);

  /**
   * @brief 检查是否启用了自动缓存。
   * @return 如果启用了自动缓存则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsAutocaching() const { return GetStandardValue(kAutoCacheInput).toBool(); }
  /**
   * @brief 设置是否启用自动缓存。
   * @param e true 表示启用自动缓存，false 表示禁用。
   */
  void SetAutocache(bool e);

  /**
   * @brief 丢弃此片段的所有缓存数据。
   */
  void DiscardCache();

  /**
   * @brief 使指定时间范围内的缓存无效。
   * @param range 需要作废的时间范围。
   * @param from 触发作废的源头标识。
   * @param element 相关的元素索引。
   * @param options 作废缓存的选项。
   */
  void InvalidateCache(const TimeRange &range, const QString &from, int element,
                       InvalidateCacheOptions options) override;

  /**
   * @brief 根据输入调整时间范围（例如，考虑速度、反向播放等）。
   * 这是从节点输入到节点内部逻辑的时间映射。
   * @param input 输入端口的名称。
   * @param element 输入端口的元素索引。
   * @param input_time 输入的时间范围。
   * @param clamp 是否将时间限制在有效范围内。
   * @return 调整后的时间范围。
   */
  [[nodiscard]] TimeRange InputTimeAdjustment(const QString &input, int element, const TimeRange &input_time,
                                              bool clamp) const override;

  /**
   * @brief 根据输出调整时间范围。
   * 这是从节点内部逻辑到节点输出的时间映射。
   * @param input 输入端口的名称（通常指引向此输出的逻辑输入）。
   * @param element 输入端口的元素索引。
   * @param input_time 内部处理的时间范围。
   * @return 调整后用于输出的时间范围。
   */
  [[nodiscard]] TimeRange OutputTimeAdjustment(const QString &input, int element,
                                               const TimeRange &input_time) const override;

  /**
   * @brief 计算并输出节点在特定时间点的值（例如视频帧或音频样本）。
   * @param value 当前输入行数据。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本。
   */
  void Retranslate() override;

  /**
   * @brief 请求从连接的节点使其缓存失效。
   * @param force_all 是否强制所有连接的节点都失效，默认为 false。
   * @param intersect 可选参数，指定失效的相交时间范围。
   */
  void RequestInvalidatedFromConnected(bool force_all = false, const TimeRange &intersect = TimeRange());

  /**
   * @brief 获取当前片段的播放速度。
   * @return double 类型的播放速度（1.0 为正常速度）。
   */
  [[nodiscard]] double speed() const { return GetStandardValue(kSpeedInput).toDouble(); }

  /**
   * @brief 检查片段是否设置为反向播放。
   * @return 如果反向播放则返回 true，否则返回 false。
   */
  [[nodiscard]] bool reverse() const { return GetStandardValue(kReverseInput).toBool(); }

  /**
   * @brief 设置片段是否反向播放。
   * @param e true 表示反向播放，false 表示正常播放。
   */
  void set_reverse(bool e) { SetStandardValue(kReverseInput, e); }

  /**
   * @brief 检查在变速播放时是否保持音频音调。
   * @return 如果保持音频音调则返回 true，否则返回 false。
   */
  [[nodiscard]] bool maintain_audio_pitch() const { return GetStandardValue(kMaintainAudioPitchInput).toBool(); }

  /**
   * @brief 设置在变速播放时是否保持音频音调。
   * @param e true 表示保持音频音调，false 表示不保持。
   */
  void set_maintain_audio_pitch(bool e) { SetStandardValue(kMaintainAudioPitchInput, e); }

  /**
   * @brief 获取入点转场效果。
   * @return 指向 TransitionBlock 对象的指针，如果没有则为 nullptr。
   */
  TransitionBlock *in_transition() { return in_transition_; }

  /**
   * @brief 设置入点转场效果。
   * @param t 指向 TransitionBlock 对象的指针。
   */
  void set_in_transition(TransitionBlock *t) { in_transition_ = t; }

  /**
   * @brief 获取出点转场效果。
   * @return 指向 TransitionBlock 对象的指针，如果没有则为 nullptr。
   */
  TransitionBlock *out_transition() { return out_transition_; }

  /**
   * @brief 设置出点转场效果。
   * @param t 指向 TransitionBlock 对象的指针。
   */
  void set_out_transition(TransitionBlock *t) { out_transition_ = t; }

  /**
   * @brief 获取与此片段链接的其他片段块。
   * @return Block 指针的 QVector 引用，表示链接的片段。
   */
  [[nodiscard]] const QVector<Block *> &block_links() const { return block_links_; }

  /**
   * @brief 获取连接的输入节点的视频帧哈希缓存。
   * @return 指向 FrameHashCache 对象的指针，如果无连接或无缓存则为 nullptr。
   */
  [[nodiscard]] FrameHashCache *connected_video_cache() const {
    if (Node *n = GetConnectedOutput(kBufferIn)) { // 检查是否存在通过 kBufferIn 连接的输出节点
      return n->video_frame_cache(); // 返回连接节点的视频帧缓存
    } else {
      return nullptr; // 无连接则返回 nullptr
    }
  }

  /**
   * @brief 获取连接的输入节点的音频播放缓存。
   * @return 指向 AudioPlaybackCache 对象的指针，如果无连接或无缓存则为 nullptr。
   */
  [[nodiscard]] AudioPlaybackCache *connected_audio_cache() const {
    if (Node *n = GetConnectedOutput(kBufferIn)) { // 检查是否存在通过 kBufferIn 连接的输出节点
      return n->audio_playback_cache(); // 返回连接节点的音频播放缓存
    } else {
      return nullptr; // 无连接则返回 nullptr
    }
  }

  /**
   * @brief 获取连接的输入节点的缩略图缓存。
   * @return 指向 FrameHashCache 对象的指针（用于缩略图），如果无连接或无缓存则为 nullptr。
   */
  FrameHashCache *thumbnails() {
    if (Node *n = GetConnectedOutput(kBufferIn)) { // 检查是否存在通过 kBufferIn 连接的输出节点
      return n->thumbnail_cache(); // 返回连接节点的缩略图缓存
    } else {
      return nullptr; // 无连接则返回 nullptr
    }
  }

  /**
   * @brief 获取连接的输入节点的音频波形缓存。
   * @return 指向 AudioWaveformCache 对象的指针，如果无连接或无缓存则为 nullptr。
   */
  AudioWaveformCache *waveform() {
    if (Node *n = GetConnectedOutput(kBufferIn)) { // 检查是否存在通过 kBufferIn 连接的输出节点
      return n->waveform_cache(); // 返回连接节点的波形缓存
    } else {
      return nullptr; // 无连接则返回 nullptr
    }
  }

  /**
   * @brief 添加来自另一个 ClipBlock 的缓存直通。
   * 这允许当前片段在某些情况下直接使用另一个片段的缓存数据。
   * @param other 另一个 ClipBlock 对象。
   */
  void AddCachePassthroughFrom(ClipBlock *other);

  /**
   * @brief 获取连接到的查看器输出对象。
   * @return 指向 ViewerOutput 对象的指针，如果未连接到查看器则为 nullptr。
   */
  [[nodiscard]] ViewerOutput *connected_viewer() const { return connected_viewer_; }

  /**
   * @brief 获取此片段的视频缓存范围。
   * 通常是从 0 到片段长度。
   * @return TimeRange 表示的视频缓存时间范围。
   */
  [[nodiscard]] TimeRange GetVideoCacheRange() const override { return TimeRange(rational(0), length()); }

  /**
   * @brief 获取此片段的音频缓存范围。
   * 通常是从 0 到片段长度。
   * @return TimeRange 表示的音频缓存时间范围。
   */
  [[nodiscard]] TimeRange GetAudioCacheRange() const override { return TimeRange(rational(0), length()); }

  /**
   * @brief 当此片段连接到预览/查看器时触发的事件。
   */
  void ConnectedToPreviewEvent() override;

  /**
   * @brief 获取此片段在源媒体文件中的实际使用范围（考虑媒体入点和片段长度）。
   * @return TimeRange 表示的媒体时间范围。
   */
  [[nodiscard]] TimeRange media_range() const;

  /**
   * @brief 获取当前设置的循环模式。
   * @return LoopMode 枚举值，表示循环模式。
   */
  [[nodiscard]] LoopMode loop_mode() const { return static_cast<LoopMode>(GetStandardValue(kLoopModeInput).toInt()); }

  /**
   * @brief 设置片段的循环模式。
   * @param l LoopMode 枚举值。
   */
  void set_loop_mode(LoopMode l) { SetStandardValue(kLoopModeInput, int(l)); }

  /**
   * @brief 查找与此片段关联的多机位节点（MultiCamNode）。
   * @return 指向 MultiCamNode 的指针，如果找不到则为 nullptr。
   */
  MultiCamNode *FindMulticam();

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kBufferIn;                ///< "BufferIn" - 缓冲输入端口的键名，通常用于连接媒体源。
  static const QString kMediaInInput;            ///< "MediaIn" - 媒体入点参数的键名。
  static const QString kSpeedInput;              ///< "Speed" - 播放速度参数的键名。
  static const QString kReverseInput;            ///< "Reverse" - 反向播放参数的键名。
  static const QString kMaintainAudioPitchInput; ///< "MaintainAudioPitch" - 保持音频音调参数的键名。
  static const QString kLoopModeInput;           ///< "LoopMode" - 循环模式参数的键名。

  static const QString kAutoCacheInput;          ///< "AutoCache" - 自动缓存参数的键名。

 protected:
  /**
   * @brief 当片段的链接关系发生变化时调用的事件处理函数。
   */
  void LinkChangeEvent() override;

  /**
   * @brief 当节点的某个输入端口连接上一个输出节点时调用的事件处理函数。
   * @param input 连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param output 连接到的输出节点。
   */
  void InputConnectedEvent(const QString &input, int element, Node *output) override;

  /**
   * @brief 当节点的某个输入端口断开连接时调用的事件处理函数。
   * @param input 断开连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param output 之前连接的输出节点。
   */
  void InputDisconnectedEvent(const QString &input, int element, Node *output) override;

  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString &input, int element) override;

 private:
  /**
   * @brief 用于 SequenceToMediaTime 函数的标志位枚举。
   * 控制时间转换时是否忽略某些因素（如反向、速度、循环）。
   */
  enum SequenceToMediaTimeFlag {
    kSTMNone = 0x0,           ///< 无特殊标志。
    kSTMIgnoreReverse = 0x1,  ///< 转换时忽略反向播放设置。
    kSTMIgnoreSpeed = 0x2,    ///< 转换时忽略播放速度设置。
    kSTMIgnoreLoop = 0x4      ///< 转换时忽略循环模式设置。
  };

  /**
   * @brief 将序列时间（时间线上的时间）转换为媒体时间（源文件中的时间）。
   * 会考虑片段的速度、反向播放、循环模式和媒体入点等因素。
   * @param sequence_time rational 类型的序列时间。
   * @param flags uint64_t 类型的标志位，用于控制转换行为 (SequenceToMediaTimeFlag)。
   * @return rational 类型的媒体时间。
   */
  [[nodiscard]] rational SequenceToMediaTime(const rational &sequence_time, uint64_t flags = kSTMNone) const;

  /**
   * @brief 将媒体时间（源文件中的时间）转换为序列时间（时间线上的时间）。
   * 会考虑片段的速度、反向播放、循环模式和媒体入点等因素。
   * @param media_time rational 类型的媒体时间。
   * @return rational 类型的序列时间。
   */
  [[nodiscard]] rational MediaToSequenceTime(const rational &media_time) const;

  /**
   * @brief 请求从连接的输入节点获取指定时间范围的数据。
   * @param range 需要请求的时间范围。
   */
  void RequestRangeFromConnected(const TimeRange &range);

  /**
   * @brief 为指定的缓存请求数据或使其失效。
   * @param cache 目标 PlaybackCache 对象。
   * @param max_range 允许的最大时间范围。
   * @param range 需要操作的时间范围。
   * @param invalidate 如果为 true，则作废该范围的缓存；否则，请求该范围的数据。
   * @param request 如果为 true，则实际发起数据请求。
   */
  void RequestRangeForCache(PlaybackCache *cache, const TimeRange &max_range, const TimeRange &range, bool invalidate,
                            bool request);
  /**
   * @brief 请求使指定缓存中与最大范围相交的部分失效。
   * @param cache 目标 PlaybackCache 对象。
   * @param max_range 最大时间范围，用于确定失效区域。
   */
  void RequestInvalidatedForCache(PlaybackCache *cache, const TimeRange &max_range);

  /**
   * @brief 获取调整后的缩略图时间范围。
   * @param r 指向 TimeRange 对象的指针，用于存储结果。
   * @return 如果成功获取范围则返回 true，否则返回 false。
   */
  bool GetAdjustedThumbnailRange(TimeRange *r) const;

  // --- 私有成员变量 ---
  QVector<Block *> block_links_; ///<存储与其他 Block 链接的列表。

  TransitionBlock *in_transition_;  ///< 指向入点转场效果的指针。
  TransitionBlock *out_transition_; ///< 指向出点转场效果的指针。

  ViewerOutput *connected_viewer_;  ///< 指向当前连接的查看器输出对象。

 private:
  rational last_media_in_; ///< 上一次记录的媒体入点时间，可能用于检测变化。
};

}  // namespace olive

#endif  // CLIPBLOCK_H // 原为 TIMELINEBLOCK_H，根据上下文推断应为 CLIPBLOCK_H