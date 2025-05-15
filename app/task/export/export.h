#ifndef EXPORTTASK_H // 防止头文件被重复包含的预处理器指令
#define EXPORTTASK_H // 定义 EXPORTTASK_H 宏

#include "codec/encoder.h"                 // 包含了编码器相关的定义
#include "node/output/viewer/viewer.h"     // 包含了查看器输出节点相关的定义
#include "render/colorprocessor.h"         // 包含了色彩处理器相关的定义
#include "render/projectcopier.h"          // 包含了项目复制器相关的定义
#include "task/render/render.h"            // 包含了渲染任务基类的定义
#include "task/task.h"                     // 包含了任务基类的定义

namespace olive { // olive 项目的命名空间

/**
 * @brief ExportTask 类定义，继承自 RenderTask 类。
 *
 * 此类负责执行项目的导出操作。它管理从渲染管线获取帧和音频数据，
 * 进行必要的颜色处理，然后使用指定的编码参数将这些数据编码到输出文件中。
 * 它还支持字幕的编码。
 */
class ExportTask : public RenderTask {
  Q_OBJECT // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
 public:
  /**
   * @brief ExportTask 的构造函数。
   * @param viewer_node 指向 ViewerOutput 节点的指针，导出任务将从此节点获取要渲染的帧。
   * @param color_manager 指向 ColorManager 的指针，用于处理导出过程中的色彩空间转换。
   * @param params 编码参数，定义了输出文件的格式、编解码器、码率等设置。
   */
  ExportTask(ViewerOutput *viewer_node, ColorManager *color_manager, const EncodingParams &params);

 protected:
  /**
   * @brief 执行导出任务的核心逻辑。
   *
   * 此方法重写自 RenderTask 的 Run 方法。当任务被调度执行时，此函数会被调用。
   * 它负责初始化编码器，请求渲染帧和音频，并将它们传递给编码器。
   * @return 如果任务成功完成，则返回 true；如果发生任何错误导致任务失败，则返回 false。
   */
  bool Run() override;

  /**
   * @brief 当一帧视频数据下载完成（即渲染完成）时被调用。
   *
   * 此方法重写自 RenderTask。它接收渲染好的视频帧，并将其传递给编码器进行编码。
   * @param frame 指向下载完成的视频帧的智能指针。
   * @param time 该帧对应的时间点（以有理数表示）。
   * @return 如果帧处理和编码成功，则返回 true；否则返回 false。
   */
  bool FrameDownloaded(FramePtr frame, const rational &time) override;

  /**
   * @brief 当一段音频数据下载完成（即渲染完成）时被调用。
   *
   * 此方法重写自 RenderTask。它接收渲染好的音频样本，并将其传递给编码器进行编码。
   * @param range 该段音频数据的时间范围。
   * @param samples 包含下载完成的音频样本的 SampleBuffer。
   * @return 如果音频处理和编码成功，则返回 true；否则返回 false。
   */
  bool AudioDownloaded(const TimeRange &range, const SampleBuffer &samples) override;

  /**
   * @brief 对字幕块进行编码。
   *
   * 此方法重写自 RenderTask。它接收一个字幕块并将其传递给字幕编码器。
   * @param sub 指向要编码的字幕块的指针。
   * @return 如果字幕编码成功，则返回 true；否则返回 false。
   */
  bool EncodeSubtitle(const SubtitleBlock *sub) override;

  /**
   * @brief 指示此渲染任务是否使用两步帧渲染。
   *
   * 对于导出任务，通常不使用两步帧渲染。
   * @return 固定返回 false。
   */
  [[nodiscard]] bool TwoStepFrameRendering() const override { return false; }

 private:
  /**
   * @brief 循环写入音频数据块。
   *
   * 这是一个内部辅助函数，用于处理和编码音频样本，确保音频数据按正确的顺序和时间写入。
   * @param time 当前音频块的时间范围。
   * @param samples 包含要写入的音频样本的 SampleBuffer。
   * @return 如果音频写入和编码成功，则返回 true；否则返回 false。
   */
  bool WriteAudioLoop(const TimeRange &time, const SampleBuffer &samples);

  ProjectCopier *copier_; ///< @brief 指向 ProjectCopier 对象的指针，可能用于在导出前复制项目相关数据。

  QHash<rational, FramePtr> time_map_; ///< @brief 用于存储已渲染视频帧的哈希表，键为帧的时间戳，值为帧数据。

  QHash<TimeRange, SampleBuffer> audio_map_; ///< @brief 用于存储已渲染音频样本的哈希表，键为音频的时间范围，值为音频数据。

  ColorManager *color_manager_; ///< @brief 指向色彩管理器对象的指针，用于色彩空间转换。

  EncodingParams params_; ///< @brief 存储导出任务所使用的编码参数。

  std::shared_ptr<Encoder> encoder_; ///< @brief 指向主编码器（视频和音频）的智能指针。

  std::shared_ptr<Encoder> subtitle_encoder_; ///< @brief 指向字幕编码器的智能指针。

  ColorProcessorPtr color_processor_; ///< @brief 指向色彩处理器对象的智能指针，用于对视频帧应用色彩变换。

  int64_t frame_time_{}; ///< @brief 当前处理的视频帧的时间戳（以内部时间单位表示）。

  rational audio_time_; ///< @brief 当前处理的音频数据的时间点（以有理数表示）。

  TimeRange export_range_; ///< @brief 定义了要导出的时间范围。
};

}  // namespace olive // 结束 olive 命名空间

#endif  // EXPORTTASK_H // 结束预处理器指令 #ifndef EXPORTTASK_H
