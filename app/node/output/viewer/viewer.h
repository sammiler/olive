#ifndef VIEWER_H // 防止头文件被多次包含的宏定义开始
#define VIEWER_H

#include "codec/encoder.h"                // 编码器相关定义，可能用于导出或录制
#include "node/node.h"                    // 引入基类 Node 的定义
#include "node/output/track/track.h"      // 引入 Track 定义，用于流类型等
#include "render/audioplaybackcache.h"    // 音频播放缓存
#include "render/framehashcache.h"        // 帧哈希缓存，可能用于视频帧缓存
#include "render/subtitleparams.h"        // 字幕流参数定义
#include "render/videoparams.h"           // 视频流参数定义
#include "timeline/timelinemarker.h"      // 时间线标记定义
#include "timeline/timelineworkarea.h"    // 时间线工作区定义

// 可能需要的前向声明
// class QVariant; // 假设
// class QXmlStreamReader; // 假设
// class QXmlStreamWriter; // 假设
// class SerializedData; // 假设
// class TimeRange; // 假设
// class rational; // 假设
// struct InvalidateCacheOptions; // 假设
// class AudioParams; // 假设，尽管 videoparams.h 可能包含
// class AudioWaveformCache; // 假设
// struct ValueHint; // 假设
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设
// class EncodingParams; // 假设
// enum class SampleFormat; // 假设

namespace olive { // Olive 编辑器的命名空间

class Footage; // 前向声明 Footage 类，ViewerOutput 可能与素材交互

/**
 * @brief 作为节点系统与 ViewerPanel (查看器界面面板) 之间的桥梁。
 * 它接收来自 ViewerPanel 的更新请求和时间变化信号，
 * 并通过向上游节点请求数据，最终将渲染好的帧（纹理）发送给 ViewerPanel 进行显示。
 * ViewerOutput 本质上是一个特殊的输出节点，代表了用户最终看到的画面和听到的声音。
 */
class ViewerOutput : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief ViewerOutput 构造函数。
   * @param create_buffer_inputs 是否创建用于连接纹理和音频样本的默认输入端口，默认为 true。
   * @param create_default_streams 是否创建默认的视频和音频流参数，默认为 true。
   */
  explicit ViewerOutput(bool create_buffer_inputs = true, bool create_default_streams = true);

  NODE_DEFAULT_FUNCTIONS(ViewerOutput) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /** @brief 获取此查看器输出节点的名称。 */
  [[nodiscard]] QString Name() const override;
  /** @brief 获取此查看器输出节点的唯一标识符。 */
  [[nodiscard]] QString id() const override;
  /** @brief 获取此节点所属的分类 ID 列表 (通常是 "输出" Output)。 */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /** @brief 获取此查看器输出节点的描述信息。 */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 获取特定类型的数据（例如，节点特定的元数据）。
   * @param d 要获取的数据类型标识。
   * @return QVariant 包含所请求数据的 QVariant 对象。
   */
  [[nodiscard]] QVariant data(const DataType &d) const override;

  /** @brief 设置节点的默认参数（例如，默认分辨率、帧率等）。 */
  void set_default_parameters();

  /**
   * @brief 根据一组 Footage (素材) 对象来设置节点的参数。
   * @param footage Footage 对象指针的 QVector。
   */
  void set_parameters_from_footage(const QVector<ViewerOutput *> &footage); // 参数类型可能是笔误，应为 Footage*

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
   * @brief 获取指定索引的视频流参数。
   * @param index 视频流的索引 (默认为0，即第一个视频流)。
   * @return VideoParams 视频参数对象。如果索引无效则返回空参数对象。
   */
  [[nodiscard]] VideoParams GetVideoParams(int index = 0) const {
    // This check isn't strictly necessary (GetStandardValue will return a null VideoParams anyway),
    // but it does suppress a warning message that we don't need
    if (index < InputArraySize(kVideoParamsInput)) { // 检查索引是否在有效范围内
      return GetStandardValue(kVideoParamsInput, index).value<VideoParams>(); // 获取并转换为 VideoParams
    } else {
      return {}; // 返回默认构造的 VideoParams (空参数)
    }
  }

  /**
   * @brief 获取指定索引的音频流参数。
   * @param index 音频流的索引 (默认为0)。
   * @return AudioParams 音频参数对象。如果索引无效则返回空参数对象。
   */
  [[nodiscard]] AudioParams GetAudioParams(int index = 0) const {
    // This check isn't strictly necessary (GetStandardValue will return a null VideoParams anyway),
    // but it does suppress a warning message that we don't need
    if (index < InputArraySize(kAudioParamsInput)) { // 检查索引是否在有效范围内
      return GetStandardValue(kAudioParamsInput, index).value<AudioParams>(); // 获取并转换为 AudioParams
    } else {
      return {}; // 返回默认构造的 AudioParams (空参数)
    }
  }

  /**
   * @brief 获取指定索引的字幕流参数。
   * @param index 字幕流的索引 (默认为0)。
   * @return SubtitleParams 字幕参数对象。如果索引无效则返回空参数对象。
   */
  [[nodiscard]] SubtitleParams GetSubtitleParams(int index = 0) const {
    // This check isn't strictly necessary (GetStandardValue will return a null VideoParams anyway),
    // but it does suppress a warning message that we don't need
    if (index < InputArraySize(kSubtitleParamsInput)) { // 检查索引是否在有效范围内
      return GetStandardValue(kSubtitleParamsInput, index).value<SubtitleParams>(); // 获取并转换为 SubtitleParams
    } else {
      return {}; // 返回默认构造的 SubtitleParams (空参数)
    }
  }

  /** @brief 获取当前播放头的位置。 */
  const rational &GetPlayhead() { return playhead_; }

  /**
   * @brief 设置指定索引的视频流参数。
   * @param video 新的视频参数对象。
   * @param index 视频流的索引 (默认为0)。
   */
  void SetVideoParams(const VideoParams &video, int index = 0) {
    SetStandardValue(kVideoParamsInput, QVariant::fromValue(video), index);
  }

  /**
   * @brief 设置指定索引的音频流参数。
   * @param audio 新的音频参数对象。
   * @param index 音频流的索引 (默认为0)。
   */
  void SetAudioParams(const AudioParams &audio, int index = 0) {
    SetStandardValue(kAudioParamsInput, QVariant::fromValue(audio), index);
  }

  /**
   * @brief 设置指定索引的字幕流参数。
   * @param subs 新的字幕参数对象。
   * @param index 字幕流的索引 (默认为0)。
   */
  void SetSubtitleParams(const SubtitleParams &subs, int index = 0) {
    SetStandardValue(kSubtitleParamsInput, QVariant::fromValue(subs), index);
  }

  /** @brief 获取视频流的总数。 */
  [[nodiscard]] int GetVideoStreamCount() const { return InputArraySize(kVideoParamsInput); }
  /** @brief 获取音频流的总数。 */
  [[nodiscard]] int GetAudioStreamCount() const { return InputArraySize(kAudioParamsInput); }
  /** @brief 获取字幕流的总数。 */
  [[nodiscard]] int GetSubtitleStreamCount() const { return InputArraySize(kSubtitleParamsInput); }

  /** @brief 获取所有类型流的总数 (视频 + 音频 + 字幕)。 */
  [[nodiscard]] virtual int GetTotalStreamCount() const {
    return GetVideoStreamCount() + GetAudioStreamCount() + GetSubtitleStreamCount();
  }

  /**
   * @brief 获取连接的音频输出节点的波形缓存。
   * @return const AudioWaveformCache* 指向波形缓存的指针，如果没有连接或无缓存则为 nullptr。
   */
  const AudioWaveformCache *GetConnectedWaveform() {
    if (Node *n = GetConnectedSampleOutput()) { // 获取连接的音频样本输出节点
      return n->waveform_cache(); // 返回其波形缓存
    } else {
      return nullptr;
    }
  }

  /** @brief 检查是否存在已启用的视频流。 */
  [[nodiscard]] bool HasEnabledVideoStreams() const;
  /** @brief 检查是否存在已启用的音频流。 */
  [[nodiscard]] bool HasEnabledAudioStreams() const;
  /** @brief 检查是否存在已启用的字幕流。 */
  [[nodiscard]] bool HasEnabledSubtitleStreams() const;

  /** @brief 获取第一个已启用的视频流的参数。 */
  [[nodiscard]] VideoParams GetFirstEnabledVideoStream() const;
  /** @brief 获取第一个已启用的音频流的参数。 */
  [[nodiscard]] AudioParams GetFirstEnabledAudioStream() const;
  /** @brief 获取第一个已启用的字幕流的参数。 */
  [[nodiscard]] SubtitleParams GetFirstEnabledSubtitleStream() const;

  /** @brief 获取总长度（通常是视频和音频长度中较大的一个）。 */
  [[nodiscard]] const rational &GetLength() const { return last_length_; }
  /** @brief 获取视频内容的总长度。 */
  [[nodiscard]] const rational &GetVideoLength() const { return video_length_; }
  /** @brief 获取音频内容的总长度。 */
  [[nodiscard]] const rational &GetAudioLength() const { return audio_length_; }

  /** @brief 获取与此查看器关联的时间线工作区对象。 */
  [[nodiscard]] TimelineWorkArea *GetWorkArea() const { return workarea_; }
  /** @brief 获取与此查看器关联的时间线标记列表对象。 */
  [[nodiscard]] TimelineMarkerList *GetMarkers() const { return markers_; }

  /** @brief 获取视频缓存的时间范围 (从0到视频长度)。 */
  [[nodiscard]] TimeRange GetVideoCacheRange() const override { return TimeRange(rational(0), GetVideoLength()); }
  /** @brief 获取音频缓存的时间范围 (从0到音频长度)。 */
  [[nodiscard]] TimeRange GetAudioCacheRange() const override { return TimeRange(rational(0), GetAudioLength()); }

  /** @brief 获取所有已启用的流作为轨道引用的列表。 */
  [[nodiscard]] QVector<Track::Reference> GetEnabledStreamsAsReferences() const;

  /** @brief 获取所有已启用的视频流参数的列表。 */
  [[nodiscard]] QVector<VideoParams> GetEnabledVideoStreams() const;
  /** @brief 获取所有已启用的音频流参数的列表。 */
  [[nodiscard]] QVector<AudioParams> GetEnabledAudioStreams() const;

  /** @brief 当界面语言等需要重新翻译时调用。 */
  void Retranslate() override;

  /** @brief 获取连接到纹理输入端口（用于视频帧）的上游输出节点。 */
  virtual Node *GetConnectedTextureOutput();
  /** @brief 获取连接的纹理输入的数值类型提示。 */
  virtual ValueHint GetConnectedTextureValueHint();
  /** @brief 获取连接到音频样本输入端口的上游输出节点。 */
  virtual Node *GetConnectedSampleOutput();
  /** @brief 获取连接的音频样本输入的数值类型提示。 */
  virtual ValueHint GetConnectedSampleValueHint();

  /** @brief 设置是否启用波形数据的请求和生成。 */
  void SetWaveformEnabled(bool e);

  /** @brief (静态方法) 检查是否启用了视频自动缓存 (此实现为存根)。 */
  [[nodiscard]] static bool IsVideoAutoCacheEnabled() {
    qDebug() << "sequence ac is a stub"; // 调试输出，表明是存根实现
    return false;
  }
  /** @brief (静态方法) 设置是否启用视频自动缓存 (此实现为存根)。 */
  static void SetVideoAutoCacheEnabled(bool e) { qDebug() << "sequence ac is a stub"; }

  /**
   * @brief 计算并输出节点在特定时间点的值（通常是将输入纹理和样本直接传递或进行简单处理）。
   *  ViewerOutput 的主要职责是请求和接收数据，而不是大量计算。
   * @param value 当前输入行数据。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /** @brief 获取上一次使用的编码参数。 */
  [[nodiscard]] const EncodingParams &GetLastUsedEncodingParams() const { return last_used_encoding_params_; }
  /** @brief 设置上一次使用的编码参数。 */
  void SetLastUsedEncodingParams(const EncodingParams &p) { last_used_encoding_params_ = p; }

  /**
   * @brief 从 XML 流加载节点的自定义数据（例如，流参数、工作区、标记等）。
   * @param reader 指向 QXmlStreamReader 的指针。
   * @param data 指向 SerializedData 的指针。
   * @return 如果加载成功则返回 true，否则返回 false。
   */
  bool LoadCustom(QXmlStreamReader *reader, SerializedData *data) override;
  /**
   * @brief 将节点的自定义数据保存到 XML 流。
   * @param writer 指向 QXmlStreamWriter 的指针。
   */
  void SaveCustom(QXmlStreamWriter *writer) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kVideoParamsInput;    ///< "VideoParams" - 存储视频流参数的数组输入参数键名。
  static const QString kAudioParamsInput;    ///< "AudioParams" - 存储音频流参数的数组输入参数键名。
  static const QString kSubtitleParamsInput; ///< "SubtitleParams" - 存储字幕流参数的数组输入参数键名。

  static const QString kTextureInput; ///< "TextureIn" - 连接视频帧纹理的输入端口键名。
  static const QString kSamplesInput; ///< "SamplesIn" - 连接音频样本的输入端口键名。

  static const SampleFormat kDefaultSampleFormat; ///< 默认的音频采样格式。

 signals: // Qt 信号声明区域
  /** @brief 当视频帧率改变时发射。 @param 新的帧率。 */
  void FrameRateChanged(const rational &);
  /** @brief 当总长度改变时发射。 @param 新的长度。 */
  void LengthChanged(const rational &length);
  /** @brief 当视频尺寸（宽度、高度）改变时发射。 @param 新的宽度。 @param 新的高度。 */
  void SizeChanged(int width, int height);
  /** @brief 当像素宽高比改变时发射。 @param 新的像素宽高比。 */
  void PixelAspectChanged(const rational &pixel_aspect);
  /** @brief 当隔行扫描模式改变时发射。 @param 新的隔行扫描模式。 */
  void InterlacingChanged(VideoParams::Interlacing mode);

  /** @brief 当视频参数发生任何改变时发射。 */
  void VideoParamsChanged();
  /** @brief 当音频参数发生任何改变时发射。 */
  void AudioParamsChanged();

  /** @brief 当纹理输入连接发生改变时发射。 */
  void TextureInputChanged();
  /** @brief 当音频采样率改变时发射。 @param 新的采样率。 */
  void SampleRateChanged(int sr);
  /** @brief 当连接的波形缓存对象发生改变时发射。 */
  void ConnectedWaveformChanged();
  /** @brief 当播放头位置改变时发射。 @param 新的播放头时间。 */
  void PlayheadChanged(const rational &t);

 public slots:
  /** @brief 验证并更新当前的总长度、视频长度和音频长度。 */
  void VerifyLength();
  /**
   * @brief 设置播放头的位置。会发射 PlayheadChanged 信号。
   * @param t 新的播放头时间。
   */
  void SetPlayhead(const rational &t);

 protected:
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
   * @brief (虚保护函数) 验证并返回特定类型流的内部计算长度。
   * @param type 流类型 (视频、音频等)。
   * @return rational 计算得到的长度。
   */
  [[nodiscard]] virtual rational VerifyLengthInternal(Track::Type type) const;

  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString &input, int element) override;

  /**
   * @brief 添加一个指定类型的流参数。
   * @param type 要添加的流类型。
   * @param value 流参数的值 (QVariant 封装)。
   * @return int 新添加流的索引。
   */
  int AddStream(Track::Type type, const QVariant &value);
  /**
   * @brief 设置指定索引处指定类型的流参数。
   * @param type 要设置的流类型。
   * @param value 新的流参数值。
   * @param index 要设置的流的索引。
   * @return int 被设置流的索引 (如果成功)。
   */
  int SetStream(Track::Type type, const QVariant &value, int index);

 private:
  rational last_length_;  ///< 上一次计算得到的总长度。
  rational video_length_; ///< 视频内容的总长度。
  rational audio_length_; ///< 音频内容的总长度。

  VideoParams cached_video_params_; ///< 缓存的视频参数，用于快速访问或比较变化。
  AudioParams cached_audio_params_; ///< 缓存的音频参数。

  TimelineWorkArea *workarea_;   ///< 指向时间线工作区对象的指针。
  TimelineMarkerList *markers_; ///< 指向时间线标记列表对象的指针。

  bool autocache_input_video_; ///< 是否自动缓存视频输入。
  bool autocache_input_audio_; ///< 是否自动缓存音频输入。

  EncodingParams last_used_encoding_params_; ///< 上一次导出或编码时使用的参数。

  bool waveform_requests_enabled_; ///< 是否启用了音频波形数据的请求。

  rational playhead_; ///< 当前播放头的时间位置。
};

}  // namespace olive

#endif  // VIEWER_H // 头文件宏定义结束