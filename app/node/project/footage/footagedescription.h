#ifndef FOOTAGEDESCRIPTION_H // 防止头文件被多次包含的宏定义开始
#define FOOTAGEDESCRIPTION_H

#include <utility> // 引入 std::move 等工具

#include "node/output/track/track.h" // 引入 Track::Type 枚举定义
#include "render/subtitleparams.h"   // 字幕流参数定义
#include "render/videoparams.h"      // 视频流参数定义 (AudioParams 可能也在此或类似文件中)

// 可能需要的前向声明
// class QString; // 假设
// class Q_ASSERT; // Qt 断言宏

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 描述一个媒体素材（Footage）的类。
 * 它封装了从媒体文件中探测到的所有重要信息，包括使用的解码器、
 * 以及文件包含的所有视频流、音频流和字幕流的详细参数。
 * 这个类主要用于存储和传递媒体文件的元数据。
 */
class FootageDescription {
 public:
  /**
   * @brief FootageDescription 构造函数。
   * @param decoder (可选) 用于解码此素材的解码器名称/ID。默认为空字符串。
   */
  explicit FootageDescription(QString decoder = QString()) : decoder_(std::move(decoder)), total_stream_count_(0) {}

  /**
   * @brief 检查此素材描述是否有效。
   * 有效的描述必须有一个非空的解码器ID，并且至少包含一个视频、音频或字幕流。
   * @return 如果有效则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsValid() const {
    return !decoder_.isEmpty() && // 解码器ID不能为空
           (!video_streams_.isEmpty() || !audio_streams_.isEmpty() || !subtitle_streams_.isEmpty()); // 至少有一个流
  }

  /**
   * @brief 获取解码器的名称/ID。
   * @return const QString& 对解码器名称字符串的常量引用。
   */
  [[nodiscard]] const QString& decoder() const { return decoder_; }

  /**
   * @brief 添加一个视频流的参数到描述中。
   *  断言确保不会添加具有相同流索引的流。
   * @param video_params 要添加的 VideoParams 对象。
   */
  void AddVideoStream(const VideoParams& video_params) {
    Q_ASSERT(!HasStreamIndex(video_params.stream_index())); // 断言：流索引不应重复
    video_streams_.append(video_params); // 添加到视频流列表
  }

  /**
   * @brief 添加一个音频流的参数到描述中。
   * @param audio_params 要添加的 AudioParams 对象。
   */
  void AddAudioStream(const AudioParams& audio_params) {
    Q_ASSERT(!HasStreamIndex(audio_params.stream_index())); // 断言：流索引不应重复
    audio_streams_.append(audio_params); // 添加到音频流列表
  }

  /**
   * @brief 添加一个字幕流的参数到描述中。
   * @param sub_params 要添加的 SubtitleParams 对象。
   */
  void AddSubtitleStream(const SubtitleParams& sub_params) {
    Q_ASSERT(!HasStreamIndex(sub_params.stream_index())); // 断言：流索引不应重复
    subtitle_streams_.append(sub_params); // 添加到字幕流列表
  }

  /**
   * @brief 根据流的原始索引（在媒体文件中的索引）获取其类型。
   * @param index 流的原始索引。
   * @return Track::Type 流的类型 (视频, 音频, 字幕)，如果未找到则为 Track::kNone。
   */
  [[nodiscard]] Track::Type GetTypeOfStream(int index) const {
    if (StreamIsVideo(index)) {
      return Track::kVideo;
    } else if (StreamIsAudio(index)) {
      return Track::kAudio;
    } else if (StreamIsSubtitle(index)) {
      return Track::kSubtitle;
    } else {
      return Track::kNone;
    }
  }

  /**
   * @brief 检查具有给定原始索引的流是否是视频流。
   * @param index 流的原始索引。
   * @return 如果是视频流则返回 true，否则返回 false。
   */
  [[nodiscard]] bool StreamIsVideo(int index) const {
    foreach (const VideoParams& vp, video_streams_) { // 遍历所有视频流
      if (vp.stream_index() == index) { // 检查原始索引是否匹配
        return true;
      }
    }
    return false;
  }

  /**
   * @brief 检查具有给定原始索引的流是否是音频流。
   * @param index 流的原始索引。
   * @return 如果是音频流则返回 true，否则返回 false。
   */
  [[nodiscard]] bool StreamIsAudio(int index) const {
    foreach (const AudioParams& ap, audio_streams_) { // 遍历所有音频流
      if (ap.stream_index() == index) { // 检查原始索引是否匹配
        return true;
      }
    }
    return false;
  }

  /**
   * @brief 检查具有给定原始索引的流是否是字幕流。
   * @param index 流的原始索引。
   * @return 如果是字幕流则返回 true，否则返回 false。
   */
  [[nodiscard]] bool StreamIsSubtitle(int index) const {
    foreach (const SubtitleParams& sp, subtitle_streams_) { // 遍历所有字幕流
      if (sp.stream_index() == index) { // 检查原始索引是否匹配
        return true;
      }
    }
    return false;
  }

  /**
   * @brief 检查是否存在具有给定原始索引的任何类型的流。
   * @param index 流的原始索引。
   * @return 如果存在该索引的流则返回 true，否则返回 false。
   */
  [[nodiscard]] bool HasStreamIndex(int index) const {
    return StreamIsVideo(index) || StreamIsAudio(index) || StreamIsSubtitle(index);
  }

  /**
   * @brief 获取此素材描述中记录的总流数量（所有类型的总和）。
   * @return int 总流数量。
   */
  [[nodiscard]] int GetStreamCount() const { return total_stream_count_; }
  /**
   * @brief 设置此素材描述中记录的总流数量。
   * @param s 新的总流数量。
   */
  void SetStreamCount(int s) { total_stream_count_ = s; }

  /**
   * @brief 从指定文件加载素材描述信息（通常是之前保存的元数据文件）。
   * @param filename 包含素材描述信息的文件路径。
   * @return 如果加载成功则返回 true，否则返回 false。
   */
  bool Load(const QString& filename);

  /**
   * @brief 将当前的素材描述信息保存到指定文件。
   * @param filename 要保存到的文件路径。
   * @return 如果保存成功则返回 true，否则返回 false。
   */
  [[nodiscard]] bool Save(const QString& filename) const;

  /** @brief 获取所有视频流参数的列表 (常量引用)。 */
  [[nodiscard]] const QVector<VideoParams>& GetVideoStreams() const { return video_streams_; }
  /** @brief 获取所有视频流参数的列表 (可修改的引用)。 */
  QVector<VideoParams>& GetVideoStreams() { return video_streams_; }

  /** @brief 获取所有音频流参数的列表 (常量引用)。 */
  [[nodiscard]] const QVector<AudioParams>& GetAudioStreams() const { return audio_streams_; }
  /** @brief 获取所有音频流参数的列表 (可修改的引用)。 */
  QVector<AudioParams>& GetAudioStreams() { return audio_streams_; }

  /** @brief 获取所有字幕流参数的列表 (常量引用)。 */
  [[nodiscard]] const QVector<SubtitleParams>& GetSubtitleStreams() const { return subtitle_streams_; }
  /** @brief 获取所有字幕流参数的列表 (可修改的引用)。 */
  QVector<SubtitleParams>& GetSubtitleStreams() { return subtitle_streams_; }

 private:
  // 定义素材元数据文件的版本号，用于处理文件格式的向后兼容性。
  static constexpr unsigned kFootageMetaVersion = 6; ///< 素材元数据版本号。

  QString decoder_; ///< 解码器名称/ID。

  QVector<VideoParams> video_streams_; ///< 存储所有视频流参数的列表。
  QVector<AudioParams> audio_streams_; ///< 存储所有音频流参数的列表。
  QVector<SubtitleParams> subtitle_streams_; ///< 存储所有字幕流参数的列表。

  int total_stream_count_; ///< 此素材包含的总流数量。
};

}  // namespace olive

#endif  // FOOTAGEDESCRIPTION_H // 头文件宏定义结束