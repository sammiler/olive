#ifndef FOOTAGEJOB_H  // 防止头文件被重复包含的宏
#define FOOTAGEJOB_H  // 定义 FOOTAGEJOB_H 宏

#include <utility>  // 标准库 utility 头文件，提供 std::move

#include "node/project/footage/footage.h"  // 包含 Footage 节点相关的定义，
                                           // 也可能间接包含 AcceleratedJob (如果 Footage 继承自它或相关类)
// 或者直接包含 Track::Type, VideoParams, AudioParams, LoopMode, TimeRange, rational
// 以及 AcceleratedJob (因为 FootageJob 继承自它)
// 为了明确，FootageJob 继承自 AcceleratedJob，所以 AcceleratedJob 的定义是必需的。
// "node/project/footage/footage.h" 可能包含了这些类型定义，或者它们来自更基础的头文件。
// 假设 AcceleratedJob 的定义已通过其他方式包含或在此文件中被间接包含。
// 如果没有，通常会显式 #include "render/job/acceleratedjob.h"。
// **更新：** 考虑到实际项目中 AcceleratedJob 通常在 render/job/ 目录下，
// 并且 FootageJob 明确继承 AcceleratedJob，所以一个更完整的 include 列表可能需要
// #include "render/job/acceleratedjob.h"
// 但我将严格按照您提供的代码进行注释。

namespace olive {  // olive 项目的命名空间

/**
 * @brief FootageJob 类代表一个处理素材 (Footage) 的加速任务。
 *
 * 它继承自 AcceleratedJob，但通常 FootageJob 本身不直接使用 AcceleratedJob 的参数映射 (value_map_)，
 * 而是通过其自身的成员变量来携带处理素材所需的信息。
 * 这些信息包括：
 * - 要处理的时间范围 (`time_`)。
 * - 解码器的名称或标识 (`decoder_`)。
 * - 素材文件的路径 (`filename_`)。
 * - 素材类型 (视频、音频等，`type_`)。
 * - 视频和音频参数 (`video_params_`, `audio_params_`)。
 * - 素材的原始长度 (`length_`)。
 * - 循环模式 (`loop_mode_`)。
 * - 可选的缓存路径 (`cache_path_`)。
 *
 * 当 NodeTraverser遇到一个需要从磁盘读取和解码素材的情况时，它会创建一个 FootageJob。
 * 然后，渲染器 (例如，NodeTraverser 的 ProcessVideoFootage/ProcessAudioFootage 方法)
 * 会使用这个 Job 中的信息来获取特定时间的帧数据或音频样本。
 */
class FootageJob : public AcceleratedJob {  // FootageJob 继承自 AcceleratedJob
 public:
  // 默认构造函数，初始化素材类型为 Track::kNone
  FootageJob() : type_(Track::kNone) {}

  /**
   * @brief 构造函数，初始化所有必要的素材处理信息。
   * @param time 要处理的素材时间范围 (在素材内部的时间)。
   * @param decoder 解码器的名称。
   * @param filename 素材文件的路径。
   * @param type 素材的类型 (视频、音频等)。
   * @param length 素材的原始总长度。
   * @param loop_mode 循环播放模式。
   */
  FootageJob(const TimeRange& time, QString decoder, QString filename, Track::Type type, const rational& length,
             LoopMode loop_mode)
      : time_(time),                     // 初始化时间范围
        decoder_(std::move(decoder)),    // 初始化解码器名称 (使用 std::move)
        filename_(std::move(filename)),  // 初始化文件名 (使用 std::move)
        type_(type),                     // 初始化素材类型
        length_(length),                 // 初始化素材长度
        loop_mode_(loop_mode)            // 初始化循环模式
                                         // video_params_, audio_params_, cache_path_ 会进行默认初始化
  {}

  /**
   * @brief 获取解码器的名称。
   * @return 返回解码器名称字符串的常量引用。
   */
  [[nodiscard]] const QString& decoder() const { return decoder_; }

  /**
   * @brief 获取素材文件的路径。
   * @return 返回文件名字符串的常量引用。
   */
  [[nodiscard]] const QString& filename() const { return filename_; }

  /**
   * @brief 获取素材的类型 (视频、音频等)。
   * @return 返回 Track::Type 枚举值。
   */
  [[nodiscard]] Track::Type type() const { return type_; }

  /**
   * @brief 获取素材的视频参数 (如果适用)。
   * @return 返回 VideoParams 对象的常量引用。
   */
  [[nodiscard]] const VideoParams& video_params() const { return video_params_; }

  /**
   * @brief 设置素材的视频参数。
   * @param p 要设置的 VideoParams 对象。
   */
  void set_video_params(const VideoParams& p) { video_params_ = p; }

  /**
   * @brief 获取素材的音频参数 (如果适用)。
   * @return 返回 AudioParams 对象的常量引用。
   */
  [[nodiscard]] const AudioParams& audio_params() const { return audio_params_; }

  /**
   * @brief 设置素材的音频参数。
   * @param p 要设置的 AudioParams 对象。
   */
  void set_audio_params(const AudioParams& p) { audio_params_ = p; }

  /**
   * @brief 获取素材的缓存路径 (如果已设置)。
   * @return 返回缓存路径字符串的常量引用。
   */
  [[nodiscard]] const QString& cache_path() const { return cache_path_; }

  /**
   * @brief 设置素材的缓存路径。
   * @param p 要设置的缓存路径字符串。
   */
  void set_cache_path(const QString& p) { cache_path_ = p; }

  /**
   * @brief 获取素材的原始总长度。
   * @return 返回 rational 类型的长度。
   */
  [[nodiscard]] const rational& length() const { return length_; }

  /**
   * @brief 设置素材的原始总长度。
   * @param length 新的长度。
   */
  void set_length(const rational& length) { length_ = length; }

  /**
   * @brief 获取当前任务要处理的素材时间范围。
   * 这是指素材内部的时间，而不是序列时间。
   * @return 返回 TimeRange 对象的常量引用。
   */
  [[nodiscard]] const TimeRange& time() const { return time_; }

  /**
   * @brief 获取素材的循环播放模式。
   * @return 返回 LoopMode 枚举值。
   */
  [[nodiscard]] LoopMode loop_mode() const { return loop_mode_; }
  /**
   * @brief 设置素材的循环播放模式。
   * @param m 新的 LoopMode。
   */
  void set_loop_mode(LoopMode m) { loop_mode_ = m; }

 private:
  TimeRange time_;  // 要处理的素材内部时间范围

  QString decoder_;   // 解码器名称
  QString filename_;  // 素材文件路径

  Track::Type type_;  // 素材类型 (视频、音频等)

  VideoParams video_params_;  // 视频参数
  AudioParams audio_params_;  // 音频参数

  QString cache_path_;  // 素材的缓存文件路径 (可选)

  rational length_;  // 素材的原始总长度

  LoopMode loop_mode_;  // 循环播放模式
};

}  // namespace olive

// 声明 FootageJob 类型为元类型，以便在 QVariant 中使用或在信号槽中传递
Q_DECLARE_METATYPE(olive::FootageJob)

#endif  // FOOTAGEJOB_H