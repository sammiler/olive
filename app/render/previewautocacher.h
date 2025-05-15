#ifndef AUTOCACHER_H // 防止头文件被重复包含的宏
#define AUTOCACHER_H // 定义 AUTOCACHER_H 宏

#include <QtConcurrent/QtConcurrent> // Qt 并发编程模块 (可能用于后台任务执行)
#include <utility>                   // 标准库 utility 头文件，提供 std::move

#include "config/config.h"                          // 应用程序配置相关 (可能包含缓存范围等设置)
#include "node/color/colormanager/colormanager.h" // 色彩管理器定义
#include "node/group/group.h"                       // 节点组 (NodeGroup) 定义
#include "node/node.h"                              // 节点基类定义
#include "node/output/viewer/viewer.h"              // ViewerOutput 接口或基类定义
#include "node/project.h"                           // Project 类定义
#include "render/projectcopier.h"                   // 项目拷贝器定义 (用于在单独线程中安全地拷贝项目数据进行渲染)
#include "render/renderjobtracker.h"                // 渲染任务跟踪器定义
#include "render/renderticket.h"                    // 渲染票据 (RenderTicket) 定义，用于跟踪渲染任务

// 假设 TimeRange, TimeRangeList, rational, PlaybackCache, ColorProcessorPtr, MultiCamNode
// 等类型已通过上述 include 或其他方式被间接包含。

namespace olive { // olive 项目的命名空间

class MultiCamNode; // 向前声明 MultiCamNode 类

/**
 * @brief PreviewAutoCacher 类是一个管理器，用于在后台动态地缓存序列内容。
 *
 * 它旨在与一个 Viewer (查看器) 配合使用，根据播放头的位置动态地缓存序列的各个部分。
 * 当用户在时间轴上移动播放头时，PreviewAutoCacher 会智能地预测用户可能需要查看的帧，
 * 并在后台线程中提前渲染和缓存这些帧，从而在用户实际播放或 scrubbing 时提供更流畅的体验。
 *
 * 主要功能：
 * - 监控播放头位置，并确定需要缓存的时间范围。
 * - 管理渲染任务的提交和跟踪 (使用 RenderTicket 和 RenderJobTracker)。
 * - 处理缓存失效事件，并在必要时重新触发缓存生成。
 * - 支持强制缓存特定范围 (如整个序列或入/出点范围)。
 * - 提供取消正在进行的缓存任务的机制。
 * - 可能使用 ProjectCopier 在单独的线程中安全地进行渲染。
 *
 * (注意：文件名是 AutoCacher.h，而类名是 PreviewAutoCacher，这可能是一个历史遗留问题或特定命名约定)
 */
class PreviewAutoCacher : public QObject { // PreviewAutoCacher 继承自 QObject
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param parent 父对象指针，默认为 nullptr。
   */
  explicit PreviewAutoCacher(QObject *parent = nullptr);

  // 析构函数，可能需要清理正在运行的任务或释放资源。
  ~PreviewAutoCacher() override;

  /**
   * @brief 获取指定查看器输出在特定时间点的单帧渲染票据。
   * @param viewer 目标 ViewerOutput (例如序列节点)。
   * @param t 要获取的帧的时间点。
   * @param dry (可选) 如果为 true，则为“干运行”，可能只检查是否可渲染或获取信息，而不实际渲染。
   * @return 返回一个 RenderTicketPtr，代表该渲染任务。
   */
  RenderTicketPtr GetSingleFrame(ViewerOutput *viewer, const rational &t, bool dry = false);
  /**
   * @brief 获取指定节点 (通过特定查看器上下文) 在特定时间点的单帧渲染票据。
   * (此重载版本允许更具体地指定渲染源节点，而不仅仅是 ViewerOutput)
   */
  RenderTicketPtr GetSingleFrame(Node *n, ViewerOutput *viewer, const rational &t, bool dry = false);

  /**
   * @brief 获取指定查看器输出在特定时间范围内的音频渲染票据。
   * @param viewer 目标 ViewerOutput。
   * @param range 要获取音频的时间范围。
   * @return 返回一个 RenderTicketPtr，代表该音频渲染任务。
   */
  RenderTicketPtr GetRangeOfAudio(ViewerOutput *viewer, const TimeRange &range);

  // 清除所有当前正在进行的或已完成的单帧渲染任务
  void ClearSingleFrameRenders();
  // 清除所有已完成 (未在运行) 的单帧渲染任务
  void ClearSingleFrameRendersThatArentRunning();

  /**
   * @brief 设置要进行自动缓存的项目。
   * PreviewAutoCacher 需要知道当前活动的项目以访问其节点图和缓存设置。
   * @param project 指向当前 Project 的指针。
   */
  void SetProject(Project *project);

  /**
   * @brief 强制缓存指定的时间范围。
   *
   * 通常，PreviewAutoCacher 会缓存播放头周围用户定义的范围，但是有时用户可能希望
   * 缓存某些与播放头无关的时间范围 (例如整个序列或入/出点范围)，这可以通过此方法设置。
   * @param context 相关的 ViewerOutput 上下文。
   * @param range 要强制缓存的时间范围。
   */
  void ForceCacheRange(ViewerOutput *context, const TimeRange &range);

  /**
   * @brief 更新自动缓存的帧范围 (通常根据播放头位置)。
   * @param playhead 当前的播放头时间点。
   */
  void SetPlayhead(const rational &playhead);

  /**
   * @brief 对所有当前正在运行的视频任务调用取消。
   *
   * 向视频任务发送取消信号表明我们不再对其最终结果感兴趣。
   * 这不会立即结束所有视频任务；RenderManager 会尽其所能加速完成任务。
   * RenderManager 也会返回“无结果”，可以通过 watcher->HasResult 进行检查。
   * @param and_wait_for_them_to_finish (可选) 如果为 true，则在发送取消信号后等待任务实际完成或中止。
   */
  void CancelVideoTasks(bool and_wait_for_them_to_finish = false);
  /**
   * @brief 对所有当前正在运行的音频任务调用取消。
   */
  void CancelAudioTasks(bool and_wait_for_them_to_finish = false);

  /**
   * @brief (静态) 检查当前是否正在渲染自定义范围 (通过 ForceCacheRange 设置的范围)。
   * @return 如果正在渲染自定义范围，则返回 true。
   */
  [[nodiscard]] static bool IsRenderingCustomRange();

  /**
   * @brief 设置是否暂停所有渲染 (视频和可能的音频)。
   * @param e 如果为 true，则暂停渲染。
   */
  void SetRendersPaused(bool e);
  /**
   * @brief 设置是否暂停缩略图的生成。
   * @param e 如果为 true，则暂停缩略图生成。
   */
  void SetThumbnailsPaused(bool e);

  /**
   * @brief 设置当前的多机位节点。
   * 这可能用于在多机位编辑时，优先缓存活动机位或所有相关机位。
   * @param n 指向 MultiCamNode 的指针。
   */
  void SetMulticamNode(MultiCamNode *n) { multicam_ = n; }

  /**
   * @brief 设置是否忽略来自缓存系统的缓存请求。
   * 在某些特定情况下 (例如，手动强制重新渲染所有内容)，可能需要临时忽略缓存。
   * @param e 如果为 true，则忽略缓存请求。
   */
  void SetIgnoreCacheRequests(bool e) { ignore_cache_requests_ = e; }

 public slots: // Qt 公有槽函数
  /**
   * @brief 设置用于预览显示的颜色处理器。
   * 渲染的帧在最终显示前会通过这个处理器进行颜色转换 (例如，到显示器的色彩空间)。
   * @param processor 指向 ColorProcessor 的共享指针。
   */
  void SetDisplayColorProcessor(ColorProcessorPtr processor) { display_color_processor_ = std::move(processor); }

 signals: // Qt 信号声明
  /**
   * @brief (可能用于) 停止所有与缓存代理 (Cache Proxy) 相关的任务。
   * “缓存代理”可能是指一个在后台代表主缓存进行操作的辅助缓存或进程。
   */
  void StopCacheProxyTasks();

  /**
   * @brief (可能用于) 发出缓存代理任务的进度信号。
   * @param d 进度值 (通常在 0.0 到 1.0 之间)。
   */
  void SignalCacheProxyTaskProgress(double d);

 private:
  // 尝试根据当前状态和挂起的任务来启动或继续渲染过程
  void TryRender();

  // 为指定节点、上下文和时间点渲染单帧到指定的缓存
  RenderTicketWatcher *RenderFrame(Node *node, ViewerOutput *context, const rational &time, PlaybackCache *cache,
                                   bool dry);

  // 为指定节点、上下文和时间范围渲染音频到指定的缓存
  RenderTicketPtr RenderAudio(Node *node, ViewerOutput *context, const TimeRange &range, PlaybackCache *cache);

  // 连接到指定节点的缓存，以便监听其失效事件
  void ConnectToNodeCache(Node *node) const;
  // 断开与指定节点缓存的连接
  void DisconnectFromNodeCache(Node *node) const;

  // 取消队列中等待的单帧渲染任务
  void CancelQueuedSingleFrameRender();

  // 开始缓存指定的时间范围，更新 range_list 和 tracker
  void StartCachingRange(const TimeRange &range, TimeRangeList *range_list, RenderJobTracker *tracker);
  // 开始缓存指定上下文和缓存的视频时间范围
  void StartCachingVideoRange(ViewerOutput *context, PlaybackCache *cache, const TimeRange &range);
  // 开始缓存指定上下文和缓存的音频时间范围
  void StartCachingAudioRange(ViewerOutput *context, PlaybackCache *cache, const TimeRange &range);

  // 当节点的视频缓存因节点图变化而失效时的处理函数
  void VideoInvalidatedFromNode(ViewerOutput *context, PlaybackCache *cache, const olive::TimeRange &range);
  // 当节点的音频缓存因节点图变化而失效时的处理函数
  void AudioInvalidatedFromNode(ViewerOutput *context, PlaybackCache *cache, const olive::TimeRange &range);

  Project *project_; // 指向当前活动的项目

  ProjectCopier *copier_; // 项目拷贝器，用于在渲染线程中安全地访问项目数据

  TimeRange cache_range_; // 当前根据播放头计算出的自动缓存范围

  bool use_custom_range_;          // 标记是否正在使用自定义的强制缓存范围
  TimeRange custom_autocache_range_; // 存储自定义的强制缓存范围

  bool pause_renders_;    // 标记是否暂停所有渲染
  bool pause_thumbnails_; // 标记是否暂停缩略图生成

  RenderTicketPtr single_frame_render_; // 当前正在进行的单帧渲染任务的票据
  // 存储视频即时透传 (immediate passthrough) 的渲染票据，键是观察者，值是相关的票据列表
  // (可能用于处理一些不直接写入主缓存，但需要即时结果的情况)
  QMap<RenderTicketWatcher *, QVector<RenderTicketPtr> > video_immediate_passthroughs_;

  QTimer delayed_requeue_timer_; // 延迟重新排队计时器 (可能用于避免过于频繁的重新排队)

  JobTime last_conform_task_; // 上一个音频对齐 (conform) 任务的时间信息 (可能)

  QVector<RenderTicketWatcher *> running_video_tasks_; // 当前正在运行的视频渲染任务的观察者列表
  QVector<RenderTicketWatcher *> running_audio_tasks_; // 当前正在运行的音频渲染任务的观察者列表

  ColorManager *copied_color_manager_{}; // 从主项目拷贝过来的色彩管理器副本，供渲染线程使用

  // 内部结构体，用于存储待处理的视频渲染作业信息
  struct VideoJob {
    Node *node;             // 要渲染的源节点
    ViewerOutput *context;  // 渲染上下文 (例如序列节点)
    PlaybackCache *cache;   // 目标缓存
    TimeRange range;        // 要渲染的时间范围
    TimeRangeListFrameIterator iterator; // 用于迭代时间范围内的帧
  };

  // 内部结构体，用于存储与特定视频缓存相关的附加数据
  struct VideoCacheData {
    RenderJobTracker job_tracker; // 此缓存的渲染任务跟踪器
  };

  // 内部结构体，用于存储待处理的音频渲染作业信息
  struct AudioJob {
    Node *node;            // 源节点
    ViewerOutput *context; // 渲染上下文
    PlaybackCache *cache;  // 目标缓存
    TimeRange range;       // 要渲染的时间范围
  };

  // 内部结构体，用于存储与特定音频缓存相关的附加数据
  struct AudioCacheData {
    RenderJobTracker job_tracker; // 此缓存的渲染任务跟踪器
    TimeRangeList needs_conform;  // 需要进行音频对齐 (conform) 的时间范围列表
  };

  std::list<VideoJob> pending_video_jobs_; // 待处理的视频渲染作业队列
  std::list<AudioJob> pending_audio_jobs_; // 待处理的音频渲染作业队列

  // 将 PlaybackCache 指针映射到其对应的 VideoCacheData
  QHash<PlaybackCache *, VideoCacheData> video_cache_data_;
  // 将 PlaybackCache 指针映射到其对应的 AudioCacheData
  QHash<PlaybackCache *, AudioCacheData> audio_cache_data_;

  ColorProcessorPtr display_color_processor_; // 用于最终显示的颜色处理器

  MultiCamNode *multicam_; // 指向当前活动的多机位节点 (如果存在)

  bool ignore_cache_requests_; // 标记是否忽略来自缓存系统的请求

 private slots: // Qt 私有槽函数
  /**
   * @brief 当节点图报告某个视频缓存范围因缓存系统自身原因 (例如磁盘空间不足、文件删除) 而失效时的处理函数。
   */
  void VideoInvalidatedFromCache(ViewerOutput *context, const olive::TimeRange &range);

  /**
   * @brief 当节点图报告某个音频缓存范围因缓存系统自身原因而失效时的处理函数。
   */
  void AudioInvalidatedFromCache(ViewerOutput *context, const olive::TimeRange &range);

  // 取消当前缓存操作的槽函数
  void CancelForCache();

  /**
   * @brief 当 RenderManager 返回已渲染的音频数据时的处理函数。
   */
  void AudioRendered();

  /**
   * @brief 当 RenderManager 返回已渲染的视频帧数据时的处理函数。
   */
  void VideoRendered();

  /**
   * @brief 通用函数，在需要重新排队要渲染的帧时调用。
   * (注释掉的 RequeueFrames() 可能已被其他逻辑替代或重构)
   */
  // void RequeueFrames();

  // 音频对齐 (conform) 任务完成时的处理函数
  void ConformFinished();

  // 缓存代理任务被取消时的处理函数
  void CacheProxyTaskCancelled();
};

}  // namespace olive

#endif  // AUTOCACHER_H