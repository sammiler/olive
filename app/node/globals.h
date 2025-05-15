#ifndef NODEGLOBALS_H // 防止头文件被多次包含的宏定义开始
#define NODEGLOBALS_H

#include <QVector2D> // Qt 二维向量类，用于表示分辨率
#include <utility>   // 引入 std::move 等工具

#include "render/loopmode.h"    // 引入 LoopMode 枚举定义
#include "render/videoparams.h" // 引入 VideoParams 定义 (AudioParams 可能也在此或类似文件中)

// 可能需要的前向声明
// class TimeRange; // 假设
// class rational; // 假设
// class AudioParams; // 假设 (如果 videoparams.h 未包含)

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 封装了在节点求值 (Node::Value) 过程中传递的全局参数和状态信息。
 * 当节点图被处理以生成输出时（例如，渲染一帧视频或一段音频），
 * 这些全局信息（如当前时间、目标视频/音频参数、循环模式等）会被传递给每个节点。
 * 节点可以根据这些全局信息来调整其行为和输出。
 */
class NodeGlobals {
 public:
  /**
   * @brief NodeGlobals 默认构造函数。
   *  创建一个具有默认（空）参数的 NodeGlobals 对象。
   */
  NodeGlobals() = default;

  /**
   * @brief NodeGlobals 构造函数，使用时间范围。
   * @param vparam 视频参数。
   * @param aparam 音频参数。
   * @param time 当前处理的时间范围（例如，一帧的持续时间或一段音频样本的时间范围）。
   * @param loop_mode 当前的循环播放模式。
   */
  NodeGlobals(VideoParams vparam, AudioParams aparam, const TimeRange &time, LoopMode loop_mode)
      : video_params_(std::move(vparam)),   // 使用移动语义初始化视频参数
        audio_params_(std::move(aparam)),   // 使用移动语义初始化音频参数
        time_(time),                        // 初始化时间范围
        loop_mode_(loop_mode) {}            // 初始化循环模式

  /**
   * @brief NodeGlobals 构造函数，使用单个时间点。
   *  时间范围会根据给定的时间点和视频帧率自动计算（通常为一帧的持续时间）。
   * @param vparam 视频参数。
   * @param aparam 音频参数。
   * @param time 当前处理的时间点。
   * @param loop_mode 当前的循环播放模式。
   */
  NodeGlobals(const VideoParams &vparam, const AudioParams &aparam, const rational &time, LoopMode loop_mode)
      : NodeGlobals(vparam, aparam, TimeRange(time, time + vparam.frame_rate_as_time_base()), loop_mode) {}
      // 委托给上一个构造函数，将单个时间点转换为时间范围

  /**
   * @brief 获取校正像素宽高比后的方形像素分辨率。
   * @return QVector2D 方形像素分辨率 (宽度, 高度)。
   */
  [[nodiscard]] QVector2D square_resolution() const { return video_params_.square_resolution(); }
  /**
   * @brief 获取原始的（可能非方形像素）分辨率。
   * @return QVector2D 原始分辨率 (宽度, 高度)。
   */
  [[nodiscard]] QVector2D nonsquare_resolution() const { return video_params_.resolution(); }
  /**
   * @brief 获取音频参数。
   * @return const AudioParams& 对音频参数对象的常量引用。
   */
  [[nodiscard]] const AudioParams &aparams() const { return audio_params_; }
  /**
   * @brief 获取视频参数。
   * @return const VideoParams& 对视频参数对象的常量引用。
   */
  [[nodiscard]] const VideoParams &vparams() const { return video_params_; }
  /**
   * @brief 获取当前处理的时间范围。
   * @return const TimeRange& 对时间范围对象的常量引用。
   */
  [[nodiscard]] const TimeRange &time() const { return time_; }
  /**
   * @brief 获取当前的循环播放模式。
   * @return LoopMode 循环模式枚举值。
   */
  [[nodiscard]] LoopMode loop_mode() const { return loop_mode_; }

 private:
  VideoParams video_params_; ///< 存储当前处理上下文的视频参数。
  AudioParams audio_params_; ///< 存储当前处理上下文的音频参数。
  TimeRange time_;           ///< 存储当前处理的时间范围或时间点。
  LoopMode loop_mode_;       ///< 存储当前的循环播放模式。
};

}  // namespace olive

#endif  // NODEGLOBALS_H // 头文件宏定义结束