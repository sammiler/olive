#ifndef RENDERBACKEND_H // 防止头文件被重复包含的宏
#define RENDERBACKEND_H // 定义 RENDERBACKEND_H 宏

#include <QtConcurrent/QtConcurrent> // Qt 并发编程模块

#include "colorprocessorcache.h"   // 包含 ColorProcessorCache (颜色处理器缓存) 的定义
#include "config/config.h"         // 应用程序配置相关
#include "dialog/rendercancel/rendercancel.h" // 渲染取消对话框 (可能与进度显示或用户取消相关)
#include "node/output/viewer/viewer.h"      // ViewerOutput 接口或基类定义
#include "node/project.h"                   // Project 类定义
#include "node/traverser.h"                 // NodeTraverser (节点遍历器) 定义
#include "render/previewautocacher.h"       // PreviewAutoCacher (预览自动缓存器) 定义
#include "render/renderer.h"                // Renderer (渲染器抽象基类) 定义
#include "render/renderticket.h"            // RenderTicket (渲染票据) 定义
#include "rendercache.h"                    // 包含 DecoderCache 和 ShaderCache 的定义

// 假设 QThread, QMutex, QWaitCondition, QTimer, std::list, std::vector,
// VideoParams, AudioParams, rational, TimeRange, PixelFormat, ColorManager,
// ColorProcessorPtr, FrameHashCache, MultiCamNode 等类型已通过其他方式被间接包含。

namespace olive { // olive 项目的命名空间

class Renderer; // 向前声明 Renderer 类

/**
 * @brief RenderThread 类是一个 QThread 的派生类，用于在单独的线程中执行渲染任务。
 *
 * 它维护一个渲染任务队列 (`queue_`)，并从该队列中取出 RenderTicket 逐个执行。
 * 每个 RenderThread 实例通常会关联一个 Renderer (如 OpenGLRenderer) 实例，
 * 以及共享的解码器缓存和着色器缓存。
 *
 * 使用单独的线程进行渲染可以避免阻塞主 UI 线程，从而保持应用程序的响应性。
 * 可以创建多个 RenderThread 实例来实现并行渲染。
 */
class RenderThread : public QThread { // RenderThread 继承自 QThread
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param renderer 此线程将使用的 Renderer 实例。
   * @param decoder_cache 指向共享的解码器缓存的指针。
   * @param shader_cache 指向共享的着色器缓存的指针。
   * @param parent 父对象指针，默认为 nullptr。
   */
  RenderThread(Renderer *renderer, DecoderCache *decoder_cache, ShaderCache *shader_cache, QObject *parent = nullptr);

  /**
   * @brief 向此线程的任务队列中添加一个新的渲染票据。
   * @param ticket 要添加的 RenderTicketPtr。
   */
  void AddTicket(const RenderTicketPtr &ticket);

  /**
   * @brief 从此线程的任务队列中移除一个渲染票据 (如果它尚未开始执行)。
   * @param ticket 要移除的 RenderTicketPtr。
   * @return 如果成功移除，返回 true。
   */
  bool RemoveTicket(const RenderTicketPtr &ticket);

  /**
   * @brief 请求线程退出。
   * 会设置取消标志并唤醒等待条件的线程。
   */
  void quit(); // Qt 4 风格的退出方法名，通常 QThread::quit() 是一个槽

 protected:
  /**
   * @brief (重写 QThread::run) 线程的主执行函数。
   * 在此函数中，线程会循环等待并处理任务队列中的 RenderTicket。
   */
  void run() override;

 private:
  QMutex mutex_; // 互斥锁，用于保护对任务队列 `queue_` 和 `cancelled_` 标志的并发访问

  QWaitCondition wait_; // 等待条件变量，用于在队列为空时使线程休眠，并在新任务到达时唤醒

  std::list<RenderTicketPtr> queue_; // 存储待处理渲染任务的队列

  bool cancelled_; // 标记线程是否已被请求取消/退出

  Renderer *context_; // 此线程使用的 Renderer 实例 (例如 OpenGLRenderer)

  DecoderCache *decoder_cache_; // 指向共享的解码器缓存
  ShaderCache *shader_cache_;   // 指向共享的着色器缓存
};

/**
 * @brief RenderManager 类是一个单例对象，负责管理整个应用程序的渲染流程。
 *
 * 它是渲染系统的中心协调者，主要职责包括：
 * - 创建和管理渲染后端 (如 OpenGLRenderer)。
 * - 创建和管理渲染线程 (RenderThread 池)。
 * - 接收渲染请求 (RenderFrame, RenderAudio)，将其包装成 RenderTicket，并分发给渲染线程。
 * - 管理共享资源，如解码器缓存 (DecoderCache) 和着色器缓存 (ShaderCache)。
 * - 与 PreviewAutoCacher 协作，处理自动缓存请求。
 * - 提供接口来控制渲染行为，如设置渲染后端、暂停渲染、垃圾回收策略等。
 */
class RenderManager : public QObject { // RenderManager 继承自 QObject
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  // 定义可用的渲染后端枚举
  enum Backend {
    kOpenGL, // 使用 OpenGL 提供图形加速
    kDummy   // 无图形渲染 - 用于测试核心线程逻辑
  };

  // (静态) 创建 RenderManager 的单例实例
  static void CreateInstance() { instance_ = new RenderManager(); }

  // (静态) 销毁 RenderManager 的单例实例
  static void DestroyInstance() {
    delete instance_;
    instance_ = nullptr;
  }

  // (静态) 获取 RenderManager 的单例实例指针
  static RenderManager *instance() { return instance_; }

  // 定义渲染结果的返回类型枚举
  enum ReturnType {
    kTexture, // 返回 TexturePtr (通常是GPU纹理)
    kFrame,   // 返回 FramePtr (通常是CPU内存中的帧数据)
    kNull     // 不返回特定类型的结果 (例如，只写入缓存)
  };

  // 视频渲染参数结构体，封装了渲染单帧视频所需的所有信息
  struct RenderVideoParams {
    /**
     * @brief 构造函数。
     * @param n 要渲染的源节点。
     * @param vparam 目标视频参数。
     * @param aparam 相关的音频参数 (可能影响时间同步或某些音视频效果)。
     * @param t 要渲染的时间点。
     * @param colorman 色彩管理器。
     * @param m 渲染模式。
     */
    RenderVideoParams(Node *n, const VideoParams &vparam, const AudioParams &aparam, const rational &t,
                      ColorManager *colorman, RenderMode::Mode m) {
      node = n;
      video_params = vparam;
      audio_params = aparam;
      time = t;
      color_manager = colorman;
      use_cache = false;         // 默认不强制使用缓存 (自动缓存逻辑会处理)
      return_type = kFrame;      // 默认返回 FramePtr
      force_format = PixelFormat(PixelFormat::INVALID); // 默认不强制像素格式
      force_color_output = nullptr; // 默认不强制输出颜色转换
      force_size = QSize(0, 0);   // 默认不强制输出尺寸
      force_channel_count = 0;    // 默认不强制通道数
      mode = m;                   // 设置渲染模式
      multicam = nullptr;         // 默认无多机位节点
    }

    /**
     * @brief 为此渲染参数添加缓存相关信息。
     * @param cache 用于此渲染的 FrameHashCache。
     */
    void AddCache(FrameHashCache *cache) {
      cache_dir = cache->GetCacheDirectory();    // 获取缓存目录
      cache_timebase = cache->GetTimebase();   // 获取缓存的时间基准
      cache_id = cache->GetUuid().toString();  // 获取缓存的UUID
    }

    Node *node;                   // 要渲染的源节点
    VideoParams video_params;     // 目标视频参数 (分辨率、帧率、像素格式等)
    AudioParams audio_params;     // 相关的音频参数
    rational time;                // 要渲染的时间点
    ColorManager *color_manager;  // 色彩管理器
    bool use_cache;               // 是否强制使用缓存 (通常由自动缓存器控制)
    ReturnType return_type;       // 期望的返回类型 (Texture, Frame, Null)
    RenderMode::Mode mode;        // 渲染模式 (例如，预览、最终输出)
    MultiCamNode *multicam;       // (可选) 相关的多机位节点

    // 缓存相关信息
    QString cache_dir;            // 缓存目录
    rational cache_timebase;      // 缓存的时间基准
    QString cache_id;             // 缓存的UUID

    // 强制覆盖参数 (用于特定情况，例如缩略图生成)
    QSize force_size;             // 强制输出尺寸
    int force_channel_count;      // 强制输出通道数
    QMatrix4x4 force_matrix;      // 强制应用的变换矩阵
    PixelFormat force_format;     // 强制输出像素格式
    ColorProcessorPtr force_color_output; // 强制应用的输出颜色转换处理器
  };

  // 干运行 (dry run) 渲染的时间间隔常量 (可能用于探测性渲染或信息获取)
  static const rational kDryRunInterval;

  /**
   * @brief 异步地在给定时间点生成一帧视频。
   *
   * 从此函数返回的票据 (RenderTicket) 将返回一个 FramePtr - 已渲染的、处于参考色彩空间的帧。
   *
   * 此函数是线程安全的。
   * @param params 包含所有渲染参数的 RenderVideoParams 结构体。
   * @return 返回一个 RenderTicketPtr，用于跟踪此异步渲染任务。
   */
  RenderTicketPtr RenderFrame(const RenderVideoParams &params);

  // 音频渲染参数结构体，封装了渲染一段音频所需的所有信息
  struct RenderAudioParams {
    /**
     * @brief 构造函数。
     * @param n 要渲染的源节点。
     * @param time 要渲染的时间范围。
     * @param aparam 目标音频参数。
     * @param m 渲染模式。
     */
    RenderAudioParams(Node *n, const TimeRange &time, const AudioParams &aparam, RenderMode::Mode m) {
      node = n;
      range = time;
      audio_params = aparam;
      generate_waveforms = false; // 默认不生成波形
      clamp = true;               // 默认对音频样本进行钳位处理
      mode = m;                   // 设置渲染模式
    }

    Node *node;                 // 要渲染的源节点
    TimeRange range;            // 要渲染的时间范围
    AudioParams audio_params;   // 目标音频参数
    bool generate_waveforms;    // 是否同时生成波形数据
    bool clamp;                 // 是否对输出样本进行钳位 (防止超出范围)
    RenderMode::Mode mode;      // 渲染模式
  };

  /**
   * @brief 异步地生成一段音频。
   *
   * 从此函数返回的票据将返回一个 SampleBufferPtr - 已渲染的音频。
   *
   * 此函数是线程安全的。
   * @param params 包含所有渲染参数的 RenderAudioParams 结构体。
   * @return 返回一个 RenderTicketPtr。
   */
  RenderTicketPtr RenderAudio(const RenderAudioParams &params);

  /**
   * @brief 从渲染队列中移除一个渲染票据 (如果它尚未开始执行)。
   * @param ticket 要移除的 RenderTicketPtr。
   * @return 如果成功移除，返回 true。
   */
  bool RemoveTicket(const RenderTicketPtr &ticket);

  // 渲染票据的类型枚举 (用于区分是视频任务还是音频任务)
  enum TicketType { kTypeVideo, kTypeAudio };

  /**
   * @brief 获取当前使用的渲染后端类型。
   * @return 返回 Backend 枚举值。
   */
  [[nodiscard]] Backend backend() const { return backend_; }

  /**
   * @brief 获取预览自动缓存器 (PreviewAutoCacher) 实例。
   * @return 返回 PreviewAutoCacher 指针。
   */
  [[nodiscard]] PreviewAutoCacher *GetCacher() const { return auto_cacher_; }

  /**
   * @brief 设置 RenderManager (及其关联的 PreviewAutoCacher) 当前工作的项目。
   * @param p 指向 Project 的指针。
   */
  void SetProject(Project *p) { auto_cacher_->SetProject(p); }

 public slots: // Qt 公有槽函数
  /**
   * @brief 设置是否启用激进的垃圾回收策略。
   * 激进策略可能会更频繁地清理未使用的资源 (如解码器)，以减少内存占用，
   * 但也可能导致在需要时重新加载资源的开销。
   * @param enabled 如果为 true，则启用激进垃圾回收。
   */
  void SetAggressiveGarbageCollection(bool enabled);

 signals: // Qt 信号声明
  // (此处可以添加 RenderManager 可能发出的信号，例如渲染完成、错误等)

 private:
  // 私有构造函数 (用于单例模式)
  explicit RenderManager(QObject *parent = nullptr);

  // 私有析构函数 (用于单例模式)
  ~RenderManager() override;

  /**
   * @brief 创建一个新的渲染线程。
   * @param renderer (可选) 如果提供，则新线程使用此渲染器；否则可能使用默认渲染器。
   * @return 返回创建的 RenderThread 指针。
   */
  RenderThread *CreateThread(Renderer *renderer = nullptr);

  static RenderManager *instance_; // RenderManager 的静态单例实例指针

  Renderer *context_; // 指向当前渲染后端 (如 OpenGLRenderer) 的实例指针

  Backend backend_; // 当前使用的渲染后端类型

  DecoderCache *decoder_cache_; // 指向共享的解码器缓存
  ShaderCache *shader_cache_;   // 指向共享的着色器缓存

  // 解码器最大不活动时间的阈值 (毫秒)，用于垃圾回收
  static constexpr auto kDecoderMaximumInactivityAggressive = 1000; // 激进模式
  static constexpr auto kDecoderMaximumInactivity = 5000;         // 普通模式

  int aggressive_gc_; // 标记是否启用了激进的垃圾回收 (可能是计数或布尔值)

  QTimer *decoder_clear_timer_; // 用于定期清理旧解码器的定时器

  // 不同类型的渲染任务可能使用不同的渲染线程池
  RenderThread *video_thread_;    // 用于常规视频帧渲染的线程
  RenderThread *dry_run_thread_;  // 用于干运行 (dry run) 任务的线程
  RenderThread *audio_thread_;    // 用于音频渲染的线程

  std::vector<RenderThread *> waveform_threads_; // 用于生成波形数据的线程池
  size_t last_waveform_thread_{}; // 上次分配波形任务的线程索引 (用于轮询)

  std::list<RenderThread *> render_threads_; // 所有活动渲染线程的列表

  PreviewAutoCacher *auto_cacher_; // 预览自动缓存器实例

 private slots: // Qt 私有槽函数
  /**
   * @brief 定时器触发时调用的槽函数，用于清理长时间未使用的解码器。
   */
  void ClearOldDecoders();
};

}  // namespace olive

// 声明 RenderManager::TicketType 类型为元类型
Q_DECLARE_METATYPE(olive::RenderManager::TicketType)

#endif  // RENDERBACKEND_H