#ifndef VIEWERGLWIDGET_H
#define VIEWERGLWIDGET_H

#include <QMatrix4x4>
#include <QRubberBand>

#include "node/color/colormanager/colormanager.h"
#include "node/gizmo/text.h"
#include "node/node.h"
#include "node/output/track/tracklist.h"
#include "node/traverser.h"
#include "tool/tool.h"
#include "viewerplaybacktimer.h"
#include "viewerqueue.h"
#include "viewersafemargininfo.h"
#include "viewertexteditor.h"
#include "widget/manageddisplay/manageddisplay.h"
#include "widget/timetarget/timetarget.h"

namespace olive {

/**
 * @brief Viewer 类的内部显示/渲染小部件。
 *
 * 实际的合成在其他地方离屏且多线程进行，
 * 因此其主要目的是接收最终的 OpenGL 纹理并显示它。
 *
 * 主要入口点是 SetTexture()，它将接收一个 OpenGL 纹理 ID，存储它，然后调用 update()
 * 以在屏幕上绘制它。绘制函数在 paintGL() 中（由 Qt 在 update() 过程中调用），
 * 是围绕 OpenGL ES 3.2 Core 标准化的相当简单的 OpenGL 绘制代码。
 *
 * 如果纹理已被修改，并且您 100% 确定此小部件正在使用相同的纹理对象，
 * 则可以直接调用 update() 来触发重绘，但不建议这样做。如果您不 100% 确定它将是
 * 相同的纹理对象，请使用 SetTexture()，因为它几乎总是比检查 *并* 设置它更快。
 */
class ViewerDisplayWidget : public ManagedDisplayWidget, public TimeTargetObject {
  Q_OBJECT
 public:
  /**
   * @brief ViewerGLWidget 构造函数。
   * @param parent 父 QWidget 指针。
   */
  explicit ViewerDisplayWidget(QWidget *parent = nullptr);

  /**
   * @brief 析构函数。
   */
  ~ViewerDisplayWidget() override;

  /**
   * @brief 获取安全边距信息。
   * @return 返回安全边距信息的常量引用。
   */
  const ViewerSafeMarginInfo &GetSafeMargin() const;
  /**
   * @brief 设置安全边距。
   * @param safe_margin 安全边距信息。
   */
  void SetSafeMargins(const ViewerSafeMarginInfo &safe_margin);

  /**
   * @brief 设置 Gizmo。
   * @param node Gizmo 关联的节点。
   */
  void SetGizmos(Node *node);

  /**
   * @brief 获取视频参数。
   * @return 返回视频参数的常量引用。
   */
  const VideoParams &GetVideoParams() const { return gizmo_params_; }
  /**
   * @brief 设置视频参数。
   * @param params 视频参数。
   */
  void SetVideoParams(const VideoParams &params);

  /**
   * @brief 获取音频参数。
   * @return 返回音频参数的常量引用。
   */
  const AudioParams &GetAudioParams() const { return gizmo_audio_params_; }
  /**
   * @brief 设置音频参数。
   * @param p 音频参数。
   */
  void SetAudioParams(const AudioParams &p);

  /**
   * @brief 设置当前时间。
   * @param time 要设置的时间。
   */
  void SetTime(const rational &time);
  /**
   * @brief 设置字幕轨道。
   * @param list 字幕轨道的序列。
   */
  void SetSubtitleTracks(Sequence *list);

  /**
   * @brief 设置是否显示小部件背景。
   * @param e 如果为 true，则显示背景；否则隐藏。
   */
  void SetShowWidgetBackground(bool e) {
    show_widget_background_ = e;
    update();
  }

  /**
   * @brief 将点从查看器空间转换为缓冲区空间。
   * 通过乘以逆变换矩阵来撤销缩放和平移。
   * @param pos 查看器空间中的点。
   * @return 返回缓冲区空间中的点。
   */
  QPointF TransformViewerSpaceToBufferSpace(const QPointF &pos);

  /**
   * @brief 检查是否正在进行反交错。
   * @return 如果正在反交错，则返回 true；否则返回 false。
   */
  bool IsDeinterlacing() const { return deinterlace_; }

  /**
   * @brief 重置 FPS 计时器。
   */
  void ResetFPSTimer();

  /**
   * @brief 获取是否显示 FPS。
   * @return 如果显示 FPS，则返回 true；否则返回 false。
   */
  bool GetShowFPS() const { return show_fps_; }

  /**
   * @brief 获取是否显示字幕。
   * @return 如果显示字幕，则返回 true；否则返回 false。
   */
  bool GetShowSubtitles() const { return show_subtitles_; }
  /**
   * @brief 设置是否显示字幕。
   * @param e 如果为 true，则显示字幕；否则隐藏。
   */
  void SetShowSubtitles(bool e) {
    show_subtitles_ = e;
    update();
  }

  /**
   * @brief 增加跳过的帧数。
   */
  void IncrementSkippedFrames();

  /**
   * @brief 增加帧计数。
   */
  void IncrementFrameCount() { fps_timer_update_count_++; }

  /**
   * @brief 获取当前纹理。
   * @return 返回当前纹理的智能指针。
   */
  TexturePtr GetCurrentTexture() const { return texture_; }

  /**
   * @brief 开始播放。
   * @param start_timestamp 开始播放的时间戳。
   * @param playback_speed 播放速度。
   * @param timebase 时间基准。
   * @param start_updating 是否开始更新。
   */
  void Play(const int64_t &start_timestamp, const int &playback_speed, const rational &timebase, bool start_updating);

  /**
   * @brief 暂停播放。
   */
  void Pause();

  /**
   * @brief 获取查看器队列。
   * @return 返回查看器队列的指针。
   */
  ViewerQueue *queue() { return &queue_; }

  /**
   * @brief 获取查看器播放计时器。
   * @return 返回查看器播放计时器的指针。
   */
  ViewerPlaybackTimer *timer() { return &timer_; }

  /**
   * @brief 将屏幕坐标点转换为场景坐标点。
   * @param p 屏幕坐标点。
   * @return 返回场景坐标点。
   */
  QPointF ScreenToScenePoint(const QPoint &p);

  /**
   * @brief 事件过滤器。
   * @param o 目标对象。
   * @param e 事件。
   * @return 如果事件被过滤，则返回 true；否则返回 false。
   */
  bool eventFilter(QObject *o, QEvent *e) override;

 public slots:
  /**
   * @brief 设置用于绘制的变换矩阵。
   * 如果希望绘制通过某种变换（大多数情况下不需要），则设置此项。
   * @param mat 变换矩阵。
   */
  void SetMatrixTranslate(const QMatrix4x4 &mat);

  /**
   * @brief 设置缩放矩阵。
   * @param mat 缩放矩阵。
   */
  void SetMatrixZoom(const QMatrix4x4 &mat);

  /**
   * @brief 设置裁剪矩阵。
   * @param mat 裁剪矩阵。
   */
  void SetMatrixCrop(const QMatrix4x4 &mat);

  /**
   * @brief 启用或禁用是否应发出光标处的颜色信号。
   *
   * 由于跟踪鼠标的每次移动、读取像素和进行颜色转换都非常耗费处理器资源，
   * 因此我们提供了一个选项。理想情况下，这应该连接到 PixelSamplerPanel::visibilityChanged 信号，
   * 以便在用户进行像素采样时自动启用，在不采样时禁用以进行优化。
   * @param e 如果为 true，则启用信号；否则禁用。
   */
  void SetSignalCursorColorEnabled(bool e);

  /**
   * @brief 设置图像。
   * @param buffer 包含图像数据的 QVariant。
   */
  void SetImage(const QVariant &buffer);

  /**
   * @brief 设置为空白。
   */
  void SetBlank();

  /**
   * @brief 如果工具更改为手形工具，则更改指针类型。否则将指针重置为其正常类型。
   */
  void UpdateCursor();

  /**
   * @brief 工具已更改时调用的槽函数。
   */
  void ToolChanged();

  /**
   * @brief 启用/禁用查看器上的基本反交错。
   * @param e 如果为 true，则启用反交错；否则禁用。
   */
  void SetDeinterlacing(bool e);

  /**
   * @brief 设置是否显示 FPS。
   * @param e 如果为 true，则显示 FPS；否则隐藏。
   */
  void SetShowFPS(bool e);

  /**
   * @brief 请求开始编辑文本。
   */
  void RequestStartEditingText();

 signals:
  /**
   * @brief 当用户从查看器开始拖动时发出的信号。
   * @param p 拖动开始时的点。
   */
  void DragStarted(const QPoint &p);

  /**
   * @brief 当手形拖动开始时发出的信号。
   */
  void HandDragStarted();

  /**
   * @brief 当手形拖动移动时发出的信号。
   * @param x X 坐标。
   * @param y Y 坐标。
   */
  void HandDragMoved(int x, int y);

  /**
   * @brief 当手形拖动结束时发出的信号。
   */
  void HandDragEnded();

  /**
   * @brief 当启用光标颜色并且用户鼠标位置更改时发出的信号。
   * @param reference 参考颜色。
   * @param display 显示颜色。
   */
  void CursorColor(const Color &reference, const Color &display);

  /**
   * @brief 当拖动进入时发出的信号。
   * @param event 拖动进入事件。
   */
  void DragEntered(QDragEnterEvent *event);

  /**
   * @brief 当拖动离开时发出的信号。
   * @param event 拖动离开事件。
   */
  void DragLeft(QDragLeaveEvent *event);

  /**
   * @brief 当拖放操作完成时发出的信号。
   * @param event 拖放事件。
   */
  void Dropped(QDropEvent *event);

  /**
   * @brief 当纹理更改时发出的信号。
   * @param texture 新的纹理。
   */
  void TextureChanged(TexturePtr texture);

  /**
   * @brief 当队列饥饿时发出的信号。
   */
  void QueueStarved();

  /**
   * @brief 当队列不再饥饿时发出的信号。
   */
  void QueueNoLongerStarved();

  /**
   * @brief 在指定矩形区域创建可添加对象时发出的信号。
   * @param rect 指定的矩形区域。
   */
  void CreateAddableAt(const QRectF &rect);

 protected:
  /**
   * @brief 生成世界变换。
   * @return 返回世界变换矩阵。
   */
  QTransform GenerateWorldTransform();

  /**
   * @brief 生成显示变换。
   * @return 返回显示变换矩阵。
   */
  QTransform GenerateDisplayTransform();

  /**
   * @brief 生成 Gizmo 变换。
   * @param gt 节点遍历器。
   * @param range 时间范围。
   * @return 返回 Gizmo 变换矩阵。
   */
  QTransform GenerateGizmoTransform(NodeTraverser &gt, const TimeRange &range);
  /**
   * @brief 生成 Gizmo 变换（使用内部参数）。
   * @return 返回 Gizmo 变换矩阵。
   */
  QTransform GenerateGizmoTransform() {
    NodeTraverser t;
    t.SetCacheVideoParams(gizmo_params_);
    return GenerateGizmoTransform(t, GenerateGizmoTime());
  }

  /**
   * @brief 生成 Gizmo 的时间范围。
   * @return 返回 Gizmo 的时间范围。
   */
  TimeRange GenerateGizmoTime() {
    rational node_time = GetGizmoTime();
    return TimeRange(node_time, node_time + gizmo_params_.frame_rate_as_time_base());
  }

  /**
   * @brief 从帧加载自定义纹理。
   * @param v 包含帧数据的 QVariant。
   * @return 返回加载的纹理的智能指针，如果失败则返回 nullptr。
   */
  virtual TexturePtr LoadCustomTextureFromFrame(const QVariant &v) { return nullptr; }

 protected slots:
  /**
   * @brief 绘制函数，用于在屏幕上显示纹理（在 SetTexture() 中接收）。
   *
   * 用于在屏幕上绘制纹理的简单 OpenGL 绘制函数。围绕 OpenGL ES 3.2 Core 标准化。
   */
  void OnPaint() override;

  /**
   * @brief 销毁时调用的槽函数。
   */
  void OnDestroy() override;

 private:
  /**
   * @brief 获取纹理上对应屏幕位置的点。
   * @param screen_pos 屏幕位置。
   * @return 返回纹理上的对应点。
   */
  QPointF GetTexturePosition(const QPoint &screen_pos);
  /**
   * @brief 获取纹理上对应尺寸的点。
   * @param size 尺寸。
   * @return 返回纹理上的对应点。
   */
  QPointF GetTexturePosition(const QSize &size);
  /**
   * @brief 获取纹理上对应坐标的点。
   * @param x X 坐标。
   * @param y Y 坐标。
   * @return 返回纹理上的对应点。
   */
  QPointF GetTexturePosition(const double &x, const double &y);

  /**
   * @brief 使用粗略阴影绘制文本。
   * @param painter QPainter 指针。
   * @param rect 绘制文本的矩形区域。
   * @param text 要绘制的文本。
   * @param opt 文本选项。
   */
  static void DrawTextWithCrudeShadow(QPainter *painter, const QRect &rect, const QString &text,
                                      const QTextOption &opt = QTextOption());

  /**
   * @brief 获取 Gizmo 的时间。
   * @return 返回 Gizmo 的时间。
   */
  rational GetGizmoTime();

  /**
   * @brief 判断鼠标事件是否为手形拖动。
   * @param event 鼠标事件。
   * @return 如果是手形拖动，则返回 true；否则返回 false。
   */
  static bool IsHandDrag(QMouseEvent *event);

  /**
   * @brief 更新变换矩阵。
   */
  void UpdateMatrix();

  /**
   * @brief 尝试按下 Gizmo。
   * @param row 节点值行。
   * @param p 按下的点。
   * @return 返回按下的 NodeGizmo 指针，如果没有按下则返回 nullptr。
   */
  NodeGizmo *TryGizmoPress(const NodeValueRow &row, const QPointF &p);

  /**
   * @brief 打开文本 Gizmo。
   * @param text 文本 Gizmo 指针。
   * @param event 鼠标事件，默认为 nullptr。
   */
  void OpenTextGizmo(TextGizmo *text, QMouseEvent *event = nullptr);

  /**
   * @brief 处理鼠标按下事件。
   * @param e 鼠标事件。
   * @return 如果事件被处理，则返回 true；否则返回 false。
   */
  bool OnMousePress(QMouseEvent *e);
  /**
   * @brief 处理鼠标移动事件。
   * @param e 鼠标事件。
   * @return 如果事件被处理，则返回 true；否则返回 false。
   */
  bool OnMouseMove(QMouseEvent *e);
  /**
   * @brief 处理鼠标释放事件。
   * @param e 鼠标事件。
   * @return 如果事件被处理，则返回 true；否则返回 false。
   */
  bool OnMouseRelease(QMouseEvent *e);
  /**
   * @brief 处理鼠标双击事件。
   * @param e 鼠标事件。
   * @return 如果事件被处理，则返回 true；否则返回 false。
   */
  bool OnMouseDoubleClick(QMouseEvent *e);

  /**
   * @brief 处理键盘按下事件。
   * @param e 键盘事件。
   * @return 如果事件被处理，则返回 true；否则返回 false。
   */
  bool OnKeyPress(QKeyEvent *e);
  /**
   * @brief 处理键盘释放事件。
   * @param e 键盘事件。
   * @return 如果事件被处理，则返回 true；否则返回 false。
   */
  bool OnKeyRelease(QKeyEvent *e);

  /**
   * @brief 发出光标处颜色的信号。
   * @param e 鼠标事件。
   */
  void EmitColorAtCursor(QMouseEvent *e);

  /**
   * @brief 绘制字幕轨道。
   */
  void DrawSubtitleTracks();

  /**
   * @brief 获取用于文本编辑的虚拟位置。
   * @param p 原始位置。
   * @return 返回调整后的虚拟位置。
   */
  QPointF GetVirtualPosForTextEdit(const QPointF &p) { return text_transform_inverted_.map(p) - text_edit_pos_; }

  /**
   * @brief 将拖动事件转发到文本编辑器。
   * @tparam T 事件类型。
   * @param event 事件指针。
   */
  template <typename T>
  void ForwardDragEventToTextEdit(T *event);

  /**
   * @brief 将鼠标事件转发到文本编辑器。
   * @param event 鼠标事件。
   * @param check_if_outside 是否检查是否在外部，默认为 false。
   * @return 如果事件被转发，则返回 true；否则返回 false。
   */
  bool ForwardMouseEventToTextEdit(QMouseEvent *event, bool check_if_outside = false);
  /**
   * @brief 将事件转发到文本编辑器。
   * @param event 事件。
   * @return 如果事件被转发，则返回 true；否则返回 false。
   */
  bool ForwardEventToTextEdit(QEvent *event);

  /**
   * @brief 根据垂直对齐方式调整位置。
   * @param p 原始位置。
   * @return 返回调整后的位置。
   */
  QPointF AdjustPosByVAlign(QPointF p);

  /**
   * @brief 关闭文本编辑器。
   */
  void CloseTextEditor();

  /**
   * @brief 生成 Gizmo 变换。
   */
  void GenerateGizmoTransforms();

  /**
   * @brief 绘制空白屏幕。
   * @param device_params 设备参数。
   */
  void DrawBlank(const VideoParams &device_params);

  /**
   * @brief 用于绘制的 OpenGL 纹理的内部引用。在 SetTexture() 中设置，并在 paintGL() 中使用。
   */
  TexturePtr texture_;

  /**
   * @brief 用于反交错的内部纹理。
   */
  TexturePtr deinterlace_texture_;

  /**
   * @brief 反交错着色器。
   */
  QVariant deinterlace_shader_;

  /**
   * @brief 空白着色器。
   */
  QVariant blank_shader_;

  /**
   * @brief 仅平移矩阵（默认为单位矩阵）。
   */
  QMatrix4x4 translate_matrix_;

  /**
   * @brief 仅缩放矩阵。
   */
  QMatrix4x4 scale_matrix_;

  /**
   * @brief 仅裁剪矩阵。
   */
  QMatrix4x4 crop_matrix_;

  /**
   * @brief translate_matrix_ 和 scale_matrix_ 相乘的缓存结果。
   */
  QMatrix4x4 combined_matrix_;
  /**
   * @brief combined_matrix_ 翻转后的结果。
   */
  QMatrix4x4 combined_matrix_flipped_;

  /**
   * @brief 是否发出光标颜色信号。
   */
  bool signal_cursor_color_;

  /**
   * @brief 安全边距信息。
   */
  ViewerSafeMarginInfo safe_margin_;

  /**
   * @brief Gizmo 节点。
   */
  Node *gizmos_;
  /**
   * @brief Gizmo 数据库行。
   */
  NodeValueRow gizmo_db_;
  /**
   * @brief Gizmo 视频参数。
   */
  VideoParams gizmo_params_;
  /**
   * @brief Gizmo 音频参数。
   */
  AudioParams gizmo_audio_params_;
  /**
   * @brief Gizmo 开始拖动的点。
   */
  QPoint gizmo_start_drag_;
  /**
   * @brief Gizmo 上次拖动的点。
   */
  QPoint gizmo_last_drag_;
  /**
   * @brief Gizmo 绘制的时间范围。
   */
  TimeRange gizmo_draw_time_;
  /**
   * @brief 当前 Gizmo。
   */
  NodeGizmo *current_gizmo_;
  /**
   * @brief Gizmo 是否开始拖动。
   */
  bool gizmo_drag_started_;
  /**
   * @brief Gizmo 上次绘制的变换矩阵。
   */
  QTransform gizmo_last_draw_transform_;
  /**
   * @brief Gizmo 上次绘制的逆变换矩阵。
   */
  QTransform gizmo_last_draw_transform_inverted_;

  /**
   * @brief 是否显示字幕。
   */
  bool show_subtitles_;
  /**
   * @brief 字幕轨道序列。
   */
  Sequence *subtitle_tracks_;

  /**
   * @brief 当前时间。
   */
  rational time_;

  /**
   * @brief 用于计算增量的鼠标位置。
   */
  QPoint hand_last_drag_pos_;
  /**
   * @brief 是否正在进行手形拖动。
   */
  bool hand_dragging_;

  /**
   * @brief 是否进行反交错。
   */
  bool deinterlace_;

  /**
   * @brief FPS 计时器开始时间。
   */
  qint64 fps_timer_start_{};
  /**
   * @brief FPS 计时器更新计数。
   */
  int fps_timer_update_count_{};

  /**
   * @brief 是否显示 FPS。
   */
  bool show_fps_;
  /**
   * @brief 跳过的帧数。
   */
  int frames_skipped_;

  /**
   * @brief 帧率平均值数组。
   */
  QVector<double> frame_rate_averages_;
  /**
   * @brief 帧率平均值计数。
   */
  int frame_rate_average_count_{};

  /**
   * @brief 是否显示小部件背景。
   */
  bool show_widget_background_;

  /**
   * @brief 加载的帧。
   */
  QVariant load_frame_;

  /**
   * @brief 播放速度。
   */
  int playback_speed_;

  /**
   * @brief 推送模式枚举。
   */
  enum PushMode {
    /// 要推送到内部纹理的新帧
    kPushFrame,

    /// 内部纹理引用是最新的，继续显示它
    kPushUnnecessary,

    /// 绘制空白/黑色屏幕
    kPushBlank,

    /// 不绘制任何内容（甚至不是黑色帧）
    kPushNull,
  };

  /**
   * @brief 当前推送模式。
   */
  PushMode push_mode_;

  // Playback
  /**
   * @brief 查看器队列。
   */
  ViewerQueue queue_;

  /**
   * @brief 查看器播放计时器。
   */
  ViewerPlaybackTimer timer_{};

  /**
   * @brief 播放时间基准。
   */
  rational playback_timebase_;

  /**
   * @brief 是否处于添加区域模式。
   */
  bool add_band_;
  /**
   * @brief 添加区域的起始点。
   */
  QPoint add_band_start_;
  /**
   * @brief 添加区域的结束点。
   */
  QPoint add_band_end_;

  /**
   * @brief 队列是否饥饿。
   */
  bool queue_starved_;

  /**
   * @brief 当前活动的文本 Gizmo。
   */
  TextGizmo *active_text_gizmo_{};
  /**
   * @brief 文本编辑位置。
   */
  QPointF text_edit_pos_;
  /**
   * @brief 文本编辑器。
   */
  ViewerTextEditor *text_edit_;
  /**
   * @brief 文本工具栏。
   */
  ViewerTextEditorToolBar *text_toolbar_{};
  /**
   * @brief 文本变换矩阵。
   */
  QTransform text_transform_;
  /**
   * @brief 文本逆变换矩阵。
   */
  QTransform text_transform_inverted_;

 private slots:
  /**
   * @brief 从队列更新。
   */
  void UpdateFromQueue();

  /**
   * @brief 文本编辑已更改时调用的槽函数。
   */
  void TextEditChanged();
  /**
   * @brief 文本编辑已销毁时调用的槽函数。
   */
  void TextEditDestroyed();

  /**
   * @brief 字幕已更改时调用的槽函数。
   * @param r 更改的时间范围。
   */
  void SubtitlesChanged(const TimeRange &r);

  /**
   * @brief 焦点已更改时调用的槽函数。
   * @param old 旧的焦点小部件。
   * @param now 新的焦点小部件。
   */
  void FocusChanged(QWidget *old, QWidget *now);

  /**
   * @brief 更新活动文本 Gizmo 的大小。
   * @return 返回活动文本 Gizmo 的矩形区域。
   */
  QRectF UpdateActiveTextGizmoSize();
};

}  // namespace olive

#endif  // VIEWERGLWIDGET_H