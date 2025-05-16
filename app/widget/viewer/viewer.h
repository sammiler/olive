#ifndef VIEWER_WIDGET_H // 防止头文件被多次包含的宏定义
#define VIEWER_WIDGET_H

#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QTimer>
#include <QWidget>

#include "audio/audioprocessor.h"
#include "audiowaveformview.h"
#include "node/output/viewer/viewer.h"
#include "render/previewaudiodevice.h"
#include "render/previewautocacher.h"
#include "viewerdisplay.h"
#include "viewersizer.h"
#include "viewerwindow.h"
#include "widget/playbackcontrols/playbackcontrols.h"
#include "widget/timebased/timebasedwidget.h"
#include "widget/timelinewidget/timelinewidget.h"

namespace olive {

class MulticamWidget;

/**
 * @brief ViewerWidget 类是一个基于 OpenGL 的查看器控件，带有播放控制功能 (一个 PlaybackControls 控件)。
 *
 * 它继承自 TimeBasedWidget，提供了显示视频帧、音频波形、播放控制、色彩管理、
 * Gizmo（交互辅助图形）显示、全屏功能以及与其他组件（如时间轴、节点编辑器）的交互。
 */
class ViewerWidget : public TimeBasedWidget {
  Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

 public:
  /**
   * @brief WaveformMode 枚举定义了音频波形的显示模式。
   */
  enum WaveformMode {
    kWFAutomatic,         ///< 自动模式：根据上下文决定是否显示波形（例如，如果视频轨道不可见，则显示波形）。
    kWFViewerOnly,        ///< 仅查看器模式：只显示视频，不显示波形。
    kWFWaveformOnly,      ///< 仅波形模式：只显示音频波形，不显示视频。
    kWFViewerAndWaveform  ///< 查看器和波形模式：同时显示视频和音频波形。
  };

  /**
   * @brief 构造一个 ViewerWidget 对象。
   *
   * 此构造函数会创建一个新的 ViewerDisplayWidget 作为其显示核心。
   * @param parent 父 QWidget 对象，默认为 nullptr。
   */
  explicit ViewerWidget(QWidget* parent = nullptr) : ViewerWidget(new ViewerDisplayWidget(), parent) {}

  /**
   * @brief 析构 ViewerWidget 对象。
   *
   * 清理内部资源，例如播放设备、定时器等。
   */
  ~ViewerWidget() override;

  /**
   * @brief 设置播放控制条是否启用。
   * @param enabled 如果为 true，则启用播放控制条；否则禁用。
   */
  void SetPlaybackControlsEnabled(bool enabled);

  /**
   * @brief 设置时间标尺是否启用。
   * @param enabled 如果为 true，则启用时间标尺；否则禁用。
   */
  void SetTimeRulerEnabled(bool enabled);

  /**
   * @brief 切换播放/暂停状态。
   */
  void TogglePlayPause();

  /**
   * @brief 检查当前是否正在播放。
   * @return 如果正在播放，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsPlaying() const;

  /**
   * @brief 启用或禁用色彩管理菜单。
   *
   * 虽然查看器总是进行色彩管理，但在某些上下文中，色彩管理可能由外部UI控制，
   * 使得此菜单变得不必要。
   * @param enabled 如果为 true，则启用色彩管理菜单；否则禁用。
   */
  void SetColorMenuEnabled(bool enabled);

  /**
   * @brief 设置应用于显示的变换矩阵。
   * @param mat 4x4 变换矩阵 (QMatrix4x4)。
   */
  void SetMatrix(const QMatrix4x4& mat);

  /**
   * @brief 创建一个 ViewerWindow 控件并将其全屏放置在另一个屏幕上。
   *
   * 如果 `screen` 为 nullptr，将自动选择包含鼠标光标的屏幕。
   * @param screen 目标 QScreen 对象指针，默认为 nullptr。
   */
  void SetFullScreen(QScreen* screen = nullptr);

  /**
   * @brief 获取当前的色彩管理器。
   * @return 指向 ColorManager 对象的指针。
   */
  [[nodiscard]] ColorManager* color_manager() const { return display_widget_->color_manager(); }

  /**
   * @brief 设置要在查看器中显示的 Gizmo (交互辅助图形)，通常与特定节点关联。
   * @param node 指向关联的 Node 对象的指针。
   */
  void SetGizmos(Node* node);

  /**
   * @brief 开始捕获（录制）指定时间轴源的特定时间范围和轨道。
   * @param source 指向源 TimelineWidget 的指针。
   * @param time 要捕获的时间范围 (TimeRange)。
   * @param track 要捕获的目标轨道引用 (Track::Reference)。
   */
  void StartCapture(TimelineWidget* source, const TimeRange& time, const Track::Reference& track);

  /**
   * @brief 设置是否启用音频拖拽播放（scrubbing）。
   * @param e 如果为 true，则启用音频拖拽播放。
   */
  void SetAudioScrubbingEnabled(bool e) { enable_audio_scrubbing_ = e; }

  /**
   * @brief 添加一个额外的播放设备（显示控件）。
   *
   * 允许将查看器的内容同时输出到多个显示控件。
   * @param vw 指向要添加的 ViewerDisplayWidget 对象的指针。
   */
  void AddPlaybackDevice(ViewerDisplayWidget* vw) { playback_devices_.push_back(vw); }

  /**
   * @brief 设置从时间轴传递过来的选中 Block 列表。
   *
   * 用于在查看器中反映时间轴的选择状态，可能影响多机位节点的检测或显示。
   * @param b 包含选中 Block 指针的 QVector。
   */
  void SetTimelineSelectedBlocks(const QVector<Block*>& b) {
    timeline_selected_blocks_ = b; // 更新内部选中的 Block 列表

    if (!IsPlaying()) { // 如果当前未播放
      // 如果正在播放，这将在下一帧自动发生
      DetectMulticamNodeNow();    // 立即检测多机位节点
      UpdateTextureFromNode();  // 从节点更新纹理
    }
  }

  /**
   * @brief 设置从节点视图传递过来的选中 Node 列表。
   *
   * 用于在查看器中反映节点视图的选择状态，可能影响多机位节点的检测或显示。
   * @param n 包含选中 Node 指针的 QVector。
   */
  void SetNodeViewSelections(const QVector<Node*>& n) {
    node_view_selected_ = n; // 更新内部选中的 Node 列表

    if (!IsPlaying()) { // 如果当前未播放
      // 如果正在播放，这将在下一帧自动发生
      DetectMulticamNodeNow();    // 立即检测多机位节点
      UpdateTextureFromNode();  // 从节点更新纹理
    }
  }

  /**
   * @brief 连接到一个 MulticamWidget 面板。
   * @param p 指向 MulticamWidget 对象的指针。
   */
  void ConnectMulticamWidget(MulticamWidget* p);

 public slots: // 公共槽函数
  /**
   * @brief 开始播放。
   * @param in_to_out_only 如果为 true，则仅播放入点到出点之间的范围。
   */
  void Play(bool in_to_out_only);

  /**
   * @brief 开始播放（默认播放整个范围或根据当前设置）。
   */
  void Play();

  /**
   * @brief 暂停播放。
   */
  void Pause();

  /**
   * @brief 向左快速搜寻（穿梭）。
   */
  void ShuttleLeft();

  /**
   * @brief 停止快速搜寻（穿梭）。
   */
  void ShuttleStop();

  /**
   * @brief 向右快速搜寻（穿梭）。
   */
  void ShuttleRight();

  /**
   * @brief 设置应用于显示的色彩变换。
   * @param transform 色彩变换的名称或标识符 (QString)。
   */
  void SetColorTransform(const QString& transform);

  /**
   * @brief ViewerGLWidget::SetSignalCursorColorEnabled() 的包装器。
   *
   * 用于启用或禁用光标下像素颜色值的信号发送。
   * @param e 如果为 true，则启用。
   */
  void SetSignalCursorColorEnabled(bool e);

  /**
   * @brief 缓存整个序列。
   */
  void CacheEntireSequence();

  /**
   * @brief 缓存序列的入点到出点范围。
   */
  void CacheSequenceInOut();

  /**
   * @brief 设置查看器的渲染分辨率。
   * @param width 宽度（像素）。
   * @param height 高度（像素）。
   */
  void SetViewerResolution(int width, int height);

  /**
   * @brief 设置查看器的像素宽高比。
   * @param ratio 像素宽高比 (rational)。
   */
  void SetViewerPixelAspect(const rational& ratio);

  /**
   * @brief 从当前连接的节点更新显示的纹理。
   *
   * 通常在播放头移动或节点图发生变化时调用。
   */
  void UpdateTextureFromNode();

  /**
   * @brief 请求开始编辑文本（如果当前显示的是文本节点）。
   */
  void RequestStartEditingText() { display_widget_->RequestStartEditingText(); }

 signals: // 信号
  /**
   * @brief ViewerGLWidget::CursorColor() 的包装器。
   *
   * 当光标下的像素颜色被采样时发出此信号。
   * @param reference 参考颜色空间下的颜色值。
   * @param display 显示颜色空间下的颜色值。
   */
  void CursorColor(const Color& reference, const Color& display);

  /**
   * @brief 当新的视频帧被加载并准备好显示时发出此信号。
   * @param t 指向新帧纹理的 TexturePtr (共享指针)。
   */
  void TextureChanged(TexturePtr t);

  /**
   * @brief ViewerGLWidget::ColorProcessorChanged() 的包装器。
   *
   * 当色彩处理器发生变化时发出此信号。
   * @param processor 指向新的色彩处理器的 ColorProcessorPtr (共享指针)。
   */
  void ColorProcessorChanged(ColorProcessorPtr processor);

  /**
   * @brief ViewerGLWidget::ColorManagerChanged() 的包装器。
   *
   * 当色彩管理器发生变化时发出此信号。
   * @param color_manager 指向新的色彩管理器的 ColorManager 指针。
   */
  void ColorManagerChanged(ColorManager* color_manager);

 protected: // 受保护的构造函数和重写方法
  /**
   * @brief 构造一个 ViewerWidget 对象，并使用一个外部提供的 ViewerDisplayWidget。
   * @param display 指向 ViewerDisplayWidget 对象的指针，作为显示核心。
   * @param parent 父 QWidget 对象，默认为 nullptr。
   */
  explicit ViewerWidget(ViewerDisplayWidget* display, QWidget* parent = nullptr);

  /**
   * @brief 当时间基准发生变化时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param timebase 新的时间基准 (rational)。 // 参数名根据上下文推断，原始代码中省略
   */
  void TimebaseChangedEvent(const rational&) override;
  /**
   * @brief 当播放头时间发生变化时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param time 新的播放头时间 (rational)。
   */
  void TimeChangedEvent(const rational& time) override;

  /**
   * @brief 当连接到一个新的 ViewerOutput 节点时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param node 指向被连接的 ViewerOutput 节点的指针。 // 参数名根据上下文推断，原始代码中省略
   */
  void ConnectNodeEvent(ViewerOutput*) override;
  /**
   * @brief 当从 ViewerOutput 节点断开连接时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param node 指向被断开连接的 ViewerOutput 节点的指针。 // 参数名根据上下文推断，原始代码中省略
   */
  void DisconnectNodeEvent(ViewerOutput*) override;
  /**
   * @brief 当连接的 ViewerOutput 节点发生变化时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param node 指向新的 ViewerOutput 节点的指针。 // 参数名根据上下文推断，原始代码中省略
   */
  void ConnectedNodeChangeEvent(ViewerOutput*) override;
  /**
   * @brief 当连接的工作区发生变化时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param workarea 指向新的 TimelineWorkArea 对象的指针。 // 参数名根据上下文推断，原始代码中省略
   */
  void ConnectedWorkAreaChangeEvent(TimelineWorkArea*) override;
  /**
   * @brief 当连接的标记点列表发生变化时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param markers 指向新的 TimelineMarkerList 对象的指针。 // 参数名根据上下文推断，原始代码中省略
   */
  void ConnectedMarkersChangeEvent(TimelineMarkerList*) override;

  /**
   * @brief 当（水平）缩放比例发生变化时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param s 新的缩放比例 (double)。
   */
  void ScaleChangedEvent(const double& s) override;

  /**
   * @brief 重写 QWidget::resizeEvent()，处理控件大小调整事件。
   * @param event QResizeEvent 指针，包含事件参数。
   */
  void resizeEvent(QResizeEvent* event) override;

  PlaybackControls* controls_; ///< 指向播放控制条 (PlaybackControls) 对象的指针。

  /**
   * @brief 获取内部的 ViewerDisplayWidget 显示控件。
   * @return 指向 ViewerDisplayWidget 对象的常量指针。
   */
  [[nodiscard]] ViewerDisplayWidget* display_widget() const { return display_widget_; }

  /**
   * @brief 指示在下一次时间变化时忽略音频拖拽播放事件。
   *
   * 用于避免在某些特定操作后立即触发不必要的音频播放。
   */
  void IgnoreNextScrubEvent() { ignore_scrub_++; }

  /**
   * @brief 获取指定时间点的单帧渲染结果。
   * @param t 目标时间点 (rational)。
   * @param dry 如果为 true，则为“空运行”，可能仅检查是否可渲染而不实际渲染，默认为 false。
   * @return 指向渲染结果的 RenderTicketPtr (共享指针)。
   */
  RenderTicketPtr GetSingleFrame(const rational& t, bool dry = false);

  /**
   * @brief 设置音频波形的显示模式。
   * @param wf 新的波形显示模式 (WaveformMode)。
   */
  void SetWaveformMode(WaveformMode wf);

 private: // 私有方法
  /**
   * @brief 获取当前播放头时间的 Unix 时间戳表示（毫秒）。
   * @return 时间戳 (int64_t)。
   */
  [[nodiscard]] int64_t GetTimestamp() const {
    return Timecode::time_to_timestamp(GetConnectedNode()->GetPlayhead(), timebase(), Timecode::kFloor);
  }

  /**
   * @brief 内部方法，用于更新播放头到指定的时间戳。
   * @param i 新的时间戳 (int64_t)。
   */
  void UpdateTimeInternal(int64_t i);

  /**
   * @brief 内部方法，用于开始播放。
   * @param speed 播放速度（例如 1 表示正常速度，2 表示两倍速）。
   * @param in_to_out_only 如果为 true，则仅播放入点到出点之间的范围。
   */
  void PlayInternal(int speed, bool in_to_out_only);

  /**
   * @brief internal 方法，用于暂停播放。
   */
  void PauseInternal();

  /**
   * @brief 将拖拽播放时产生的音频数据推送到播放设备。
   */
  void PushScrubbedAudio();

  /**
   * @brief 更新视图的最小允许缩放比例。
   */
  void UpdateMinimumScale();

  /**
   * @brief 静态方法，用于设置指定显示控件的色彩变换。
   * @param transform 要应用的色彩变换 (ColorTransform)。
   * @param sender 指向目标 ViewerDisplayWidget 的指针。
   */
  static void SetColorTransform(const ColorTransform& transform, ViewerDisplayWidget* sender);

  /**
   * @brief 根据时间点获取对应的缓存文件名。
   * @param time 时间点 (rational)。
   * @return 缓存文件名 (QString)。
   */
  QString GetCachedFilenameFromTime(const rational& time);

  /**
   * @brief 检查指定时间点是否存在已缓存的帧。
   * @param time 时间点 (rational)。
   * @return 如果存在缓存帧，则返回 true；否则返回 false。
   */
  bool FrameExistsAtTime(const rational& time);

  /**
   * @brief 判断当前查看器显示的内容是否可能是一个静止图像。
   * @return 如果可能是静止图像，则返回 true。
   */
  bool ViewerMightBeAStill();

  /**
   * @brief 设置显示控件显示的图像。
   * @param ticket 指向包含图像数据的 RenderTicketPtr。
   */
  void SetDisplayImage(const RenderTicketPtr& ticket);

  /**
   * @brief 为播放队列请求下一帧。
   * @param increment 是否将内部的帧计数器递增，默认为 true。
   * @return 指向 RenderTicketWatcher 对象的指针，用于监视帧的渲染状态。
   */
  RenderTicketWatcher* RequestNextFrameForQueue(bool increment = true);

  /**
   * @brief 获取指定时间点的帧。
   * @param t 目标时间点 (rational)。
   * @return 指向渲染结果的 RenderTicketPtr。
   */
  RenderTicketPtr GetFrame(const rational& t);

  /**
   * @brief 完成播放预处理操作。
   */
  void FinishPlayPreprocess();

  /**
   * @brief 决定播放队列的大小。
   * @return 播放队列的大小 (int)。
   */
  int DeterminePlaybackQueueSize();

  /**
   * @brief 从缓存文件解码图像。
   * @param cache_path 缓存文件路径。
   * @param cache_id 缓存ID。
   * @param time 图像对应的时间戳。
   * @return 指向解码后图像数据的 FramePtr。
   */
  static FramePtr DecodeCachedImage(const QString& cache_path, const QUuid& cache_id, const int64_t& time);

  /**
   * @brief 从缓存文件解码图像并更新 RenderTicket。
   * @param ticket 指向要更新的 RenderTicketPtr。
   * @param cache_path 缓存文件路径。
   * @param cache_id 缓存ID。
   * @param time 图像对应的时间戳。
   */
  static void DecodeCachedImage(const RenderTicketPtr& ticket, const QString& cache_path, const QUuid& cache_id,
                                const int64_t& time);

  /**
   * @brief 判断是否应强制显示波形。
   * @return 如果应强制显示波形，则返回 true。
   */
  [[nodiscard]] bool ShouldForceWaveform() const;

  /**
   * @brief 设置显示为空白图像。
   */
  void SetEmptyImage();

  /**
   * @brief 更新自动缓存器的状态。
   */
  void UpdateAutoCacher();

  /**
   * @brief 减少预排队的音频缓冲区计数。
   */
  void DecrementPrequeuedAudio();

  /**
   * @brief 准备录制。
   */
  void ArmForRecording();

  /**
   * @brief 取消录制准备状态。
   */
  void DisarmRecording();

  /**
   * @brief 关闭音频处理器。
   */
  void CloseAudioProcessor();

  /**
   * @brief 检测当前时间点是否有多机位节点激活。
   * @param time 当前时间点 (rational)。
   */
  void DetectMulticamNode(const rational& time);

  /**
   * @brief 检查视频内容当前是否可见。
   * @return 如果视频可见，则返回 true。
   */
  [[nodiscard]] bool IsVideoVisible() const;

  ViewerSizer* sizer_; ///< 指向 ViewerSizer 对象的指针，用于管理查看器的大小和缩放。

  int playback_speed_; ///< 当前的播放速度（例如 1 表示正常，2 表示两倍速，-1 表示反向播放）。

  rational last_time_; ///< 上一次记录的播放头时间。

  bool color_menu_enabled_; ///< 标记色彩管理菜单是否启用。

  bool time_changed_from_timer_; ///< 标记时间变化是否由播放定时器触发。

  bool play_in_to_out_only_{}; ///< 标记是否仅播放入点到出点范围，默认为 false。

  AudioWaveformView* waveform_view_; ///< 指向音频波形显示控件的指针。

  QHash<QScreen*, ViewerWindow*> windows_; ///< 存储在不同屏幕上打开的全屏查看器窗口。

  ViewerDisplayWidget* display_widget_; ///< 指向主要的 ViewerDisplayWidget 显示控件的指针。

  ViewerDisplayWidget* context_menu_widget_{}; ///< 指向触发上下文菜单的显示控件的指针，默认为 nullptr。

  QTimer playback_backup_timer_; ///< 播放备用定时器，可能用于在主播放机制失效时提供回退。

  int64_t playback_queue_next_frame_{}; ///< 播放队列中下一帧的时间戳。
  int64_t dry_run_next_frame_{};      ///< “空运行”模式下下一帧的时间戳。
  QVector<ViewerDisplayWidget*> playback_devices_; ///< 存储所有播放设备（显示控件）的列表。

  bool prequeuing_video_; ///< 标记当前是否正在预排队视频帧。
  int prequeuing_audio_;  ///< 当前预排队的音频缓冲区数量。

  QList<RenderTicketWatcher*> nonqueue_watchers_; ///< 存储非播放队列的渲染监视器列表。

  rational last_length_; ///< 上一次记录的序列长度。

  int prequeue_length_{}; ///< 预排队队列的长度。
  int prequeue_count_{};  ///< 当前预排队队列中的帧数。

  QVector<RenderTicketWatcher*> queue_watchers_; ///< 存储播放队列的渲染监视器列表。

  std::list<RenderTicketWatcher*> audio_playback_queue_; ///< 音频播放队列，存储音频帧的渲染监视器。
  rational audio_playback_queue_time_; ///< 音频播放队列的当前时间。
  AudioProcessor audio_processor_;     ///< 音频处理器实例。
  QByteArray prequeued_audio_;         ///< 预排队的原始音频数据。
  static const rational kAudioPlaybackInterval; ///< 音频播放的固定间隔时间。

  static QVector<ViewerWidget*> instances_; ///< 存储所有 ViewerWidget 实例的静态列表（可能用于全局操作）。

  std::list<RenderTicketWatcher*> audio_scrub_watchers_; ///< 音频拖拽播放的渲染监视器列表。

  bool record_armed_;          ///< 标记是否已准备好录制。
  bool recording_;             ///< 标记当前是否正在录制。
  TimelineWidget* recording_callback_{}; ///< 指向录制回调目标的 TimelineWidget 指针，默认为 nullptr。
  TimeRange recording_range_;  ///< 当前录制的时间范围。
  Track::Reference recording_track_; ///< 当前录制的目标轨道。
  QString recording_filename_; ///< 录制输出的文件名。

  qint64 queue_starved_start_{}; ///< 播放队列饥饿状态开始的时间戳，默认为0。
  RenderTicketWatcher* first_requeue_watcher_; ///< 第一个需要重新排队的渲染监视器。

  bool enable_audio_scrubbing_; ///< 标记是否启用音频拖拽播放。

  WaveformMode waveform_mode_; ///< 当前的音频波形显示模式。

  QVector<RenderTicketWatcher*> dry_run_watchers_; ///< “空运行”模式的渲染监视器列表。

  int ignore_scrub_; ///< 计数器，用于忽略后续的几次音频拖拽播放事件。

  QVector<Block*> timeline_selected_blocks_; ///< 从时间轴传递过来的选中 Block 列表。
  QVector<Node*> node_view_selected_;     ///< 从节点视图传递过来的选中 Node 列表。

  MulticamWidget* multicam_panel_; ///< 指向关联的多机位编辑面板的指针。

 private slots: // 私有槽函数
  /**
   * @brief 播放定时器更新时调用的槽函数。
   *
   * 用于驱动播放逻辑，请求下一帧等。
   */
  void PlaybackTimerUpdate();

  /**
   * @brief 当连接的 ViewerOutput 节点的长度发生变化时调用的槽函数。
   * @param length 新的长度 (rational)。
   */
  void LengthChangedSlot(const rational& length);

  /**
   * @brief 当连接的 ViewerOutput 节点的隔行扫描方式发生变化时调用的槽函数。
   * @param interlacing 新的隔行扫描方式 (VideoParams::Interlacing)。
   */
  void InterlacingChangedSlot(VideoParams::Interlacing interlacing);

  /**
   * @brief 更新渲染器的视频参数。
   */
  void UpdateRendererVideoParameters();

  /**
   * @brief 更新渲染器的音频参数。
   */
  void UpdateRendererAudioParameters();

  /**
   * @brief 显示上下文菜单。
   * @param pos 鼠标点击的本地坐标 (QPoint)，用于定位菜单。
   */
  void ShowContextMenu(const QPoint& pos);

  /**
   * @brief 从上下文菜单设置缩放级别。
   * @param action 指向被触发的 QAction 的指针。
   */
  void SetZoomFromMenu(QAction* action);

  /**
   * @brief 根据当前的波形显示模式更新音频波形视图的可见性。
   */
  void UpdateWaveformViewFromMode();

  /**
   * @brief 从上下文菜单设置全屏显示。
   * @param action 指向被触发的 QAction 的指针。
   */
  void ContextMenuSetFullScreen(QAction* action);

  /**
   * @brief 从上下文菜单设置播放分辨率。
   * @param action 指向被触发的 QAction 的指针。
   */
  void ContextMenuSetPlaybackRes(QAction* action);

  /**
   * @brief 从上下文菜单禁用安全框显示。
   */
  void ContextMenuDisableSafeMargins();

  /**
   * @brief 从上下文菜单设置（启用）安全框显示（使用预设值）。
   */
  void ContextMenuSetSafeMargins();

  /**
   * @brief 从上下文菜单设置自定义的安全框参数。
   */
  void ContextMenuSetCustomSafeMargins();

  /**
   * @brief 当全屏窗口即将关闭时调用的槽函数。
   */
  void WindowAboutToClose();

  /**
   * @brief 当渲染器生成一帧图像时调用的槽函数（用于非队列渲染）。
   */
  void RendererGeneratedFrame();

  /**
   * @brief 当渲染器为播放队列生成一帧图像时调用的槽函数。
   */
  void RendererGeneratedFrameForQueue();

  /**
   * @brief 当查看器中某个视频范围失效（需要重新渲染）时调用的槽函数。
   * @param range 失效的时间范围 (olive::TimeRange)。
   */
  void ViewerInvalidatedVideoRange(const olive::TimeRange& range);

  /**
   * @brief 从上下文菜单更新音频波形的显示模式。
   * @param a 指向被触发的 QAction 的指针。
   */
  void UpdateWaveformModeFromMenu(QAction* a);

  /**
   * @brief 处理拖动进入事件的静态槽函数（可能用于全局拖放处理）。
   * @param event QDragEnterEvent 指针。
   */
  static void DragEntered(QDragEnterEvent* event);

  /**
   * @brief 处理拖放事件的槽函数。
   * @param event QDropEvent 指针。
   */
  void Dropped(QDropEvent* event);

  /**
   * @brief 将下一个音频缓冲区加入播放队列。
   */
  void QueueNextAudioBuffer();

  /**
   * @brief 接收到用于播放的音频缓冲区时调用的槽函数。
   */
  void ReceivedAudioBufferForPlayback();

  /**
   * @brief 接收到用于拖拽播放的音频缓冲区时调用的槽函数。
   */
  void ReceivedAudioBufferForScrubbing();

  /**
   * @brief 当播放队列处于饥饿状态（没有足够的帧进行流畅播放）时调用的槽函数。
   */
  void QueueStarved();
  /**
   * @brief 当播放队列不再处于饥饿状态时调用的槽函数。
   */
  void QueueNoLongerStarved();

  /**
   * @brief 强制从当前播放头时间开始重新排队渲染帧。
   */
  void ForceRequeueFromCurrentTime();

  /**
   * @brief 更新音频处理器的状态或参数。
   */
  void UpdateAudioProcessor();

  /**
   * @brief 在指定的矩形区域创建可添加的对象（例如，拖放素材时）。
   * @param f 目标矩形区域 (QRectF)。
   */
  void CreateAddableAt(const QRectF& f);

  /**
   * @brief 处理第一个重新排队的渲染监视器被销毁的事件。
   */
  void HandleFirstRequeueDestroy();

  /**
   * @brief 显示字幕属性对话框。
   */
  void ShowSubtitleProperties();

  /**
   * @brief “空运行”模式完成时调用的槽函数。
   */
  void DryRunFinished();

  /**
   * @brief 请求下一个“空运行”帧。
   */
  void RequestNextDryRun();

  /**
   * @brief 将当前显示的帧保存为图像文件。
   */
  void SaveFrameAsImage();

  /**
   * @brief 立即检测当前时间点是否有多机位节点激活。
   */
  void DetectMulticamNodeNow();
};

}  // namespace olive

#endif  // VIEWER_WIDGET_H
