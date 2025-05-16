#ifndef RENDERTASK_H  // 防止头文件被重复包含的预处理器指令
#define RENDERTASK_H  // 定义 RENDERTASK_H 宏

#include <QtConcurrent/QtConcurrent>  // 包含了 Qt 并发编程相关的头文件，用于异步执行任务

#include "node/block/subtitle/subtitle.h"          // 包含了字幕块 (SubtitleBlock) 的定义
#include "node/color/colormanager/colormanager.h"  // 包含了色彩管理器 (ColorManager) 的定义
#include "node/output/viewer/viewer.h"             // 包含了查看器输出节点 (ViewerOutput) 的定义
#include "render/renderticket.h"                   // 包含了渲染票据 (RenderTicket) 相关的定义
#include "task/task.h"                             // 包含了任务基类 (Task) 的定义

// olive 命名空间前向声明 (如果 RenderTask 中使用了其他 olive 命名空间下的类型作为指针或引用，
// 且这些类型的完整定义不需要在此头文件中，则可以在此进行前向声明。
// 根据当前代码，大部分类型预期从上述 #include 中获得。)
class QThread;  // 前向声明 Qt 线程类

namespace olive {  // olive 项目的命名空间

/**
 * @brief RenderTask 类定义，继承自 Task 类。
 *
 * 此类是所有渲染相关任务的基类。它封装了启动和管理渲染过程的通用逻辑，
 * 包括处理视频帧、音频样本和字幕。它使用渲染票据 (RenderTicket) 和观察者模式
 * 来异步跟踪渲染进度，并提供了回调接口供派生类处理渲染完成的数据。
 */
class RenderTask : public Task {
 Q_OBJECT  // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
     public :
     /**
      * @brief RenderTask 的默认构造函数。
      */
     RenderTask();

  /**
   * @brief RenderTask 的虚析构函数。
   *
   * 确保派生类的析构函数能够被正确调用，并进行必要的资源清理。
   */
  ~RenderTask() override;

 protected:
  /**
   * @brief 启动渲染过程。
   *
   * 此方法负责初始化并启动对指定时间范围内的视频、音频和字幕的渲染。
   * @param manager 指向 ColorManager 的指针，用于处理渲染过程中的色彩转换。
   * @param video_range 一个 TimeRangeList 对象，指定了需要渲染的视频帧的时间范围列表。
   * @param audio_range 一个 TimeRangeList 对象，指定了需要渲染的音频样本的时间范围列表。
   * @param subtitle_range 一个 TimeRange 对象，指定了需要渲染的字幕的时间范围。
   * @param mode 渲染模式，定义了渲染的质量、目的等（例如预览、最终输出）。
   * @param cache 指向 FrameHashCache 的指针，用于缓存已渲染的帧以提高性能。
   * @param force_size 可选参数，用于强制指定输出帧的尺寸。如果为 QSize(0,0)，则使用默认尺寸。
   * @param force_matrix 可选参数，用于强制应用一个变换矩阵到输出帧。
   * @param force_format 可选参数，用于强制指定输出帧的像素格式。
   * @param force_channel_count 可选参数，用于强制指定输出音频的声道数。
   * @param force_color_output 可选参数，指向一个 ColorProcessorPtr 的智能指针，用于强制应用特定的色彩输出处理。
   * @return 如果渲染过程成功启动和管理，则返回 true；否则返回 false。
   */
  bool Render(ColorManager *manager, const TimeRangeList &video_range, const TimeRangeList &audio_range,
              const TimeRange &subtitle_range, RenderMode::Mode mode, FrameHashCache *cache,
              const QSize &force_size = QSize(0, 0), const QMatrix4x4 &force_matrix = QMatrix4x4(),
              PixelFormat force_format = PixelFormat(PixelFormat::INVALID), int force_channel_count = 0,
              const ColorProcessorPtr &force_color_output = nullptr);

  /**
   * @brief （虚方法）下载（处理）单个渲染完成的视频帧。
   *
   * 此方法在渲染票据完成后，在票据观察者的线程中被调用。
   * 派生类可以重写此方法以自定义帧的处理方式，例如直接编码或存入特定缓存。
   * @param thread 指向当前执行此方法的线程的指针。
   * @param frame 指向已渲染完成的视频帧 (FramePtr) 的智能指针。
   * @param time 该帧对应的时间点（以有理数表示）。
   * @return 如果帧处理成功，则返回 true；否则返回 false。
   */
  virtual bool DownloadFrame(QThread *thread, FramePtr frame, const rational &time);

  /**
   * @brief （纯虚方法）当一帧视频数据下载完成（即渲染和初步处理完成）后被调用。
   *
   * 派生类必须实现此方法，以定义如何处理最终的视频帧数据。
   * @param frame 指向下载完成的视频帧 (FramePtr) 的智能指针。
   * @param time 该帧对应的时间点（以有理数表示）。
   * @return 如果帧被成功处理，则返回 true；否则返回 false。
   */
  virtual bool FrameDownloaded(FramePtr frame, const rational &time) = 0;

  /**
   * @brief （纯虚方法）当一段音频数据下载完成（即渲染完成）后被调用。
   *
   * 派生类必须实现此方法，以定义如何处理最终的音频样本数据。
   * @param range 该段音频数据的时间范围。
   * @param samples 包含下载完成的音频样本的 SampleBuffer。
   * @return 如果音频数据被成功处理，则返回 true；否则返回 false。
   */
  virtual bool AudioDownloaded(const TimeRange &range, const SampleBuffer &samples) = 0;

  /**
   * @brief （虚方法）对字幕块进行编码或处理。
   *
   * 派生类可以重写此方法以实现特定的字幕处理逻辑，例如将其编码到输出文件。
   * @param subtitle 指向要处理的 SubtitleBlock 对象的指针。
   * @return 如果字幕块处理成功，则返回 true；否则返回 false。默认实现返回 true。
   */
  virtual bool EncodeSubtitle(const SubtitleBlock *subtitle);

  /**
   * @brief 获取当前渲染任务关联的 ViewerOutput 节点。
   * @return ViewerOutput* 指向 ViewerOutput 节点的指针。
   */
  [[nodiscard]] ViewerOutput *viewer() const { return viewer_; }

  /**
   * @brief 设置当前渲染任务关联的 ViewerOutput 节点。
   * @param v 指向 ViewerOutput 节点的指针。
   */
  void set_viewer(ViewerOutput *v) { viewer_ = v; }

  /**
   * @brief 获取当前渲染任务使用的视频参数。
   * @return const VideoParams& 对视频参数对象的常量引用。
   */
  [[nodiscard]] const VideoParams &video_params() const { return video_params_; }

  /**
   * @brief 设置当前渲染任务使用的视频参数。
   * @param video_params 包含视频参数的 VideoParams 对象。
   */
  void set_video_params(const VideoParams &video_params) { video_params_ = video_params; }

  /**
   * @brief 获取当前渲染任务使用的音频参数。
   * @return const AudioParams& 对音频参数对象的常量引用。
   */
  [[nodiscard]] const AudioParams &audio_params() const { return audio_params_; }

  /**
   * @brief 设置当前渲染任务使用的音频参数。
   * @param audio_params 包含音频参数的 AudioParams 对象。
   */
  void set_audio_params(const AudioParams &audio_params) { audio_params_ = audio_params; }

  /**
   * @brief 处理任务取消事件。
   *
   * 此方法重写自基类 Task 的 CancelEvent 方法。当任务接收到取消请求时，
   * 此函数会被调用。它负责唤醒所有可能因等待条件而阻塞的线程。
   */
  void CancelEvent() override {
    finished_watcher_mutex_.lock();         // 加锁以保护共享的等待条件
    finished_watcher_wait_cond_.wakeAll();  // 唤醒所有等待此条件的线程
    finished_watcher_mutex_.unlock();       // 解锁
  }

  /**
   * @brief （虚方法）指示此渲染任务是否采用两步帧渲染流程。
   *
   * 两步帧渲染可能指先渲染到一个中间格式，然后再进行最终处理。
   * 默认返回 true。派生类（如导出任务）可能会重写此方法以返回 false。
   * @return 如果使用两步帧渲染，则返回 true；否则返回 false。
   */
  [[nodiscard]] virtual bool TwoStepFrameRendering() const { return true; }

  /**
   * @brief 设置是否启用原生进度信号发送。
   *
   * 当启用时，任务可能会使用更底层的机制来报告进度，而不是完全依赖 Qt 的信号槽。
   * @param e 如果为 true，则启用原生进度信号；否则禁用。
   */
  void SetNativeProgressSignallingEnabled(bool e) { native_progress_signalling_ = e; }

  /**
   * @brief 获取渲染任务预计生成的总帧数。
   * @warning 此方法仅在 Render() 方法被调用之后才有效，因为总帧数是在 Render() 方法内部计算的。
   * @return int64_t 总帧数。
   */
  [[nodiscard]] int64_t GetTotalNumberOfFrames() const { return total_number_of_frames_; }

 private:
  /**
   * @brief 准备一个渲染票据观察者 (RenderTicketWatcher)。
   *
   * 此方法负责设置观察者与特定线程的关联，并连接其信号槽。
   * @param watcher 指向要准备的 RenderTicketWatcher 对象的指针。
   * @param thread 指向该观察者将要运行于的 QThread 对象的指针。
   */
  void PrepareWatcher(RenderTicketWatcher *watcher, QThread *thread);

  /**
   * @brief 增加当前正在运行的渲染票据计数。
   */
  void IncrementRunningTickets();

  /**
   * @brief 启动一个新的渲染票据（异步渲染请求）。
   *
   * 此方法创建一个 RenderTicket，配置其渲染参数，并将其提交给指定的线程执行。
   * @param watcher_thread 渲染票据观察者将运行于的线程。
   * @param manager 色彩管理器。
   * @param time 要渲染的帧的时间点。
   * @param mode 渲染模式。
   * @param cache 帧哈希缓存。
   * @param force_size 强制输出尺寸。
   * @param force_matrix 强制变换矩阵。
   * @param force_format 强制像素格式。
   * @param force_channel_count 强制音频声道数。
   * @param force_color_output 强制色彩输出处理器。
   */
  void StartTicket(QThread *watcher_thread, ColorManager *manager, const rational &time, RenderMode::Mode mode,
                   FrameHashCache *cache, const QSize &force_size, const QMatrix4x4 &force_matrix,
                   PixelFormat force_format, int force_channel_count, ColorProcessorPtr force_color_output);

  ViewerOutput *viewer_{};  ///< @brief 指向 ViewerOutput 节点的指针，作为渲染的源。初始化为 nullptr。

  VideoParams video_params_;  ///< @brief 存储当前渲染任务的视频参数。

  AudioParams audio_params_;  ///< @brief 存储当前渲染任务的音频参数。

  QVector<RenderTicketWatcher *> running_watchers_;     ///< @brief 存储当前正在运行的渲染票据观察者列表。
  std::list<RenderTicketWatcher *> finished_watchers_;  ///< @brief 存储已完成的渲染票据观察者列表。
  int running_tickets_;                                 ///< @brief 当前正在运行的渲染票据数量。
  QMutex finished_watcher_mutex_;  ///< @brief 用于同步访问 `finished_watchers_` 和 `finished_watcher_wait_cond_`
                                   ///< 的互斥锁。
  QWaitCondition finished_watcher_wait_cond_;  ///< @brief 用于在所有渲染票据完成时发出信号的等待条件。

  bool native_progress_signalling_;  ///< @brief 标志位，指示是否启用原生进度信号发送机制。

  int64_t total_number_of_frames_{};  ///< @brief 渲染任务预计生成的总帧数。在 Render() 调用后有效。初始化为0。

 private slots:
  /**
   * @brief 当一个渲染票据完成其工作时调用的槽函数。
   *
   * 此槽函数连接到 RenderTicketWatcher 的完成信号。它负责处理已完成的票据，
   * 例如将其从运行列表移至完成列表，并检查是否所有票据都已完成。
   * @param watcher 指向已完成工作的 RenderTicketWatcher 对象的指针。
   */
  void TicketDone(RenderTicketWatcher *watcher);
};

}  // namespace olive

#endif  // RENDERTASK_H // 结束预处理器指令 #ifndef RENDERTASK_H
