#ifndef TIMEBASEDWIDGET_H // 防止头文件被多次包含的宏定义
#define TIMEBASEDWIDGET_H

#include <QWidget> // 引入 QWidget 类，是所有用户界面对象的基类

#include "node/output/viewer/viewer.h" // 引入 ViewerOutput 类，用于连接视图节点
#include "timeline/timelinecommon.h"   // 引入时间轴相关的通用定义
#include "widget/keyframeview/keyframeviewinputconnection.h" // 引入关键帧视图输入连接类
#include "widget/resizablescrollbar/resizabletimelinescrollbar.h" // 引入可调整大小的时间轴滚动条类
#include "widget/timebased/timescaledobject.h" // 引入 TimeScaledObject 类，提供时间缩放功能
#include "widget/timelinewidget/view/timelineview.h" // 引入 TimelineView 类
#include "widget/timetarget/timetarget.h" // 引入 TimeTargetObject 类，用于时间目标转换

// Qt类的头文件，如QScrollBar, QTimer, QMap等，如果仅在成员变量中使用指针且未在头文件方法中解引用，
// 则通常不需要在头文件中显式#include，除非其定义在cpp文件中需要。
// 根据用户指示，此处不修改任何#include指令。

namespace olive { // olive 命名空间开始

class TimeRuler; // 前向声明 TimeRuler 类

/**
 * @brief TimeBasedWidget 类是一个基于时间轴的复杂控件。
 *
 * 它继承自 TimelineScaledWidget (假定为 TimeScaledObject 的派生类或相关别名)，
 * 提供了时间轴显示、播放头控制、缩放、吸附、标记点、输入/输出点设置等功能。
 * 通常作为更复杂的时间轴编辑器或关键帧编辑器的基础。
 */
class TimeBasedWidget : public TimelineScaledWidget { // TimeBasedWidget 类声明，继承自 TimelineScaledWidget
  Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽
 public:
  /**
   * @brief 构造一个 TimeBasedWidget 对象。
   * @param ruler_text_visible 时间标尺上的文本是否可见，默认为 true。
   * @param ruler_cache_status_visible 时间标尺上的缓存状态是否可见，默认为 false。
   * @param parent 父 QWidget 对象，默认为 nullptr。
   */
  explicit TimeBasedWidget(bool ruler_text_visible = true, bool ruler_cache_status_visible = false,
                           QWidget *parent = nullptr);

  /**
   * @brief 放大视图。
   */
  void ZoomIn();

  /**
   * @brief 缩小视图。
   */
  void ZoomOut();

  /**
   * @brief 获取当前连接的 ViewerOutput 节点。
   * @return 指向 ViewerOutput 对象的指针，如果未连接则可能为 nullptr。
   */
  [[nodiscard]] ViewerOutput *GetConnectedNode() const;

  /**
   * @brief 连接到一个 ViewerOutput 节点。
   * @param node 指向要连接的 ViewerOutput 对象的指针。
   */
  void ConnectViewerNode(ViewerOutput *node);

  /**
   * @brief 获取连接的时间轴工作区。
   * @return 指向 TimelineWorkArea 对象的指针。
   */
  [[nodiscard]] TimelineWorkArea *GetConnectedWorkArea() const { return workarea_; }
  /**
   * @brief 获取连接的时间轴标记列表。
   * @return 指向 TimelineMarkerList 对象的指针。
   */
  [[nodiscard]] TimelineMarkerList *GetConnectedMarkers() const { return markers_; }
  /**
   * @brief 连接到一个时间轴工作区。
   * @param workarea 指向 TimelineWorkArea 对象的指针。
   */
  void ConnectWorkArea(TimelineWorkArea *workarea);
  /**
   * @brief 连接到一个时间轴标记列表。
   * @param markers 指向 TimelineMarkerList 对象的指针。
   */
  void ConnectMarkers(TimelineMarkerList *markers);

  /**
   * @brief 设置缩放比例并将视图中心对齐到播放头位置。
   * @param scale 新的缩放比例 (double)。
   */
  void SetScaleAndCenterOnPlayhead(const double &scale);

  /**
   * @brief 获取时间标尺对象。
   * @return 指向 TimeRuler 对象的指针。
   */
  [[nodiscard]] TimeRuler *ruler() const;

  /**
   * @brief 定义吸附掩码的类型，为一个无符号32位整数。
   */
  using SnapMask = uint32_t;
  /**
   * @brief SnapPoints 枚举定义了不同类型的吸附目标。
   *
   * 这些值可以用作位掩码，以允许多种类型的吸附。
   */
  enum SnapPoints {
    kSnapToClips = 0x1,        ///< 吸附到片段边缘
    kSnapToPlayhead = 0x2,     ///< 吸附到播放头
    kSnapToMarkers = 0x4,      ///< 吸附到标记点
    kSnapToKeyframes = 0x8,    ///< 吸附到关键帧
    kSnapToWorkarea = 0x10,    ///< 吸附到工作区边缘 (入点/出点)
    kSnapAll = UINT32_MAX      ///< 吸附到所有类型的目标 (使用 UINT32_MAX 来确保所有位都被设置)
  };

  /**
   * @brief Snaps point `start_point` that is moving by `movement` to currently existing clips
   * (原始英文注释：将正在按 `movement` 移动的 `start_point` 点吸附到当前存在的剪辑上)
   * @param start_times 一个包含多个起始时间点的向量 (rational)，这些点将一起移动。
   * @param movement 指向表示时间移动量的 rational 对象的指针，此值可能会被吸附逻辑修改。
   * @param snap_points 吸附目标的掩码，默认为 kSnapAll。
   * @return 如果发生了吸附，则返回 true；否则返回 false。
   */
  bool SnapPoint(const std::vector<rational> &start_times, rational *movement, SnapMask snap_points = kSnapAll);
  /**
   * @brief 显示指定时间点的吸附指示线。
   * @param times 一个包含要在其上显示吸附线的 rational 时间点向量。
   */
  void ShowSnaps(const std::vector<rational> &times);
  /**
   * @brief 隐藏所有吸附指示线。
   */
  void HideSnaps();

  /**
   * @brief 复制当前选中的内容。
   * @param cut 如果为 true，则执行剪切操作（复制并删除）；否则执行复制操作。
   * @return 如果操作成功，则返回 true；否则返回 false。
   */
  virtual bool CopySelected(bool cut);

  /**
   * @brief 粘贴内容。
   * @return 如果操作成功，则返回 true；否则返回 false。
   */
  virtual bool Paste();

 public slots: // 公共槽函数
  /**
   * @brief 设置时间基准（例如帧率）。
   * @param timebase 新的时间基准 (rational)。
   */
  void SetTimebase(const rational &timebase);

  /**
   * @brief 设置视图的（水平）缩放比例。
   * @param scale 新的缩放比例 (double)。
   */
  void SetScale(const double &scale);

  /**
   * @brief 跳转到时间轴的开始位置。
   */
  void GoToStart();

  /**
   * @brief 跳转到上一帧。
   */
  void PrevFrame();

  /**
   * @brief 跳转到下一帧。
   */
  void NextFrame();

  /**
   * @brief 跳转到时间轴的结束位置。
   */
  void GoToEnd();

  /**
   * @brief 跳转到上一个剪辑点或标记点。
   */
  void GoToPrevCut();

  /**
   * @brief 跳转到下一个剪辑点或标记点。
   */
  void GoToNextCut();

  /**
   * @brief 将当前播放头位置设置为入点。
   */
  void SetInAtPlayhead();

  /**
   * @brief 将当前播放头位置设置为出点。
   */
  void SetOutAtPlayhead();

  /**
   * @brief 重置入点到时间轴的开始。
   */
  void ResetIn();

  /**
   * @brief 重置出点到时间轴的结束。
   */
  void ResetOut();

  /**
   * @brief 清除入点和出点。
   */
  void ClearInOutPoints() const;

  /**
   * @brief 在当前播放头位置设置一个标记点。
   */
  void SetMarker();

  /**
   * @brief 切换“显示全部”模式。
   *
   * 此模式可能会调整缩放比例以显示整个时间轴内容。
   */
  void ToggleShowAll();

  /**
   * @brief 跳转到入点位置。
   */
  void GoToIn();

  /**
   * @brief 跳转到出点位置。
   */
  void GoToOut();

  /**
   * @brief 删除当前选中的对象。
   */
  void DeleteSelected();

 protected: // 受保护成员
  /**
   * @brief 获取可调整大小的时间轴滚动条。
   * @return 指向 ResizableTimelineScrollBar 对象的指针。
   */
  [[nodiscard]] ResizableTimelineScrollBar *scrollbar() const;

  /**
   * @brief 当时间基准发生变化时调用的事件处理函数。
   *
   * 这是从 TimeScaledObject 继承并重写的虚函数。
   * @param timebase 新的时间基准 (rational)。 // 参数名根据上下文推断，原始代码中省略
   */
  void TimebaseChangedEvent(const rational &) override;

  /**
   * @brief 当播放头时间发生变化时调用的虚函数。
   *
   * 派生类可以重写此函数以响应时间变化。
   * @param time 新的播放头时间 (rational)。 // 参数名根据上下文推断，原始代码中省略
   */
  virtual void TimeChangedEvent(const rational &) {}

  /**
   * @brief 当（水平）缩放比例发生变化时调用的事件处理函数。
   *
   * 这是从 TimeScaledObject 继承并重写的虚函数。
   * @param scale 新的缩放比例 (double)。 // 参数名根据上下文推断，原始代码中省略
   */
  void ScaleChangedEvent(const double &) override;

  /**
   * @brief 当连接的 ViewerOutput 节点发生变化时调用的虚函数。
   * @param node 指向新的 ViewerOutput 节点的指针。 // 参数名根据上下文推断，原始代码中省略
   */
  virtual void ConnectedNodeChangeEvent(ViewerOutput *) {}

  /**
   * @brief 当连接的时间轴工作区发生变化时调用的虚函数。
   * @param workarea 指向新的 TimelineWorkArea 对象的指针。 // 参数名根据上下文推断，原始代码中省略
   */
  virtual void ConnectedWorkAreaChangeEvent(TimelineWorkArea *) {}
  /**
   * @brief 当连接的时间轴标记列表发生变化时调用的虚函数。
   * @param markers 指向新的 TimelineMarkerList 对象的指针。 // 参数名根据上下文推断，原始代码中省略
   */
  virtual void ConnectedMarkersChangeEvent(TimelineMarkerList *) {}

  /**
   * @brief 当连接到一个新的 ViewerOutput 节点时调用的虚函数。
   * @param node 指向被连接的 ViewerOutput 节点的指针。 // 参数名根据上下文推断，原始代码中省略
   */
  virtual void ConnectNodeEvent(ViewerOutput *) {}

  /**
   * @brief 当从 ViewerOutput 节点断开连接时调用的虚函数。
   * @param node 指向被断开连接的 ViewerOutput 节点的指针。 // 参数名根据上下文推断，原始代码中省略
   */
  virtual void DisconnectNodeEvent(ViewerOutput *) {}

  /**
   * @brief 设置是否自动调整滚动条的最大值。
   * @param e 如果为 true，则自动调整；否则不自动调整。
   */
  void SetAutoMaxScrollBar(bool e);

  /**
   * @brief 重写 QWidget::resizeEvent()，处理控件大小调整事件。
   * @param event QResizeEvent 指针，包含事件参数。
   */
  void resizeEvent(QResizeEvent *event) override;

  /**
   * @brief 连接一个 TimeBasedView 实例到此控件。
   *
   * 用于管理多个相关的视图。
   * @param base 指向要连接的 TimeBasedView 对象的指针。
   */
  void ConnectTimelineView(TimeBasedView *base);

  /**
   * @brief 设置滚动条的值，并处理可能的“追赶”滚动。
   * @param b 指向 QScrollBar 对象的指针。
   * @param v 新的滚动条值。
   * @param maximum 滚动条的最大值。
   */
  void SetCatchUpScrollValue(QScrollBar *b, int v, int maximum);
  /**
   * @brief 停止指定滚动条的“追赶”滚动计时器。
   * @param b 指向 QScrollBar 对象的指针。
   */
  void StopCatchUpScrollTimer(QScrollBar *b);

  /**
   * @brief 获取用于吸附的 Block 对象列表。
   *
   * 派生类应重写此函数以提供具体的 Block 对象。
   * @return 指向 Block 指针 QVector 的常量指针，默认为 nullptr。
   */
  [[nodiscard]] virtual const QVector<Block *> *GetSnapBlocks() const { return nullptr; }
  /**
   * @brief 获取用于吸附的关键帧输入连接对象列表。
   *
   * 派生类应重写此函数以提供具体的 KeyframeViewInputConnection 对象。
   * @return 指向 KeyframeViewInputConnection 指针 QVector 的常量指针，默认为 nullptr。
   */
  [[nodiscard]] virtual const QVector<KeyframeViewInputConnection *> *GetSnapKeyframes() const { return nullptr; }
  /**
   * @brief 获取关键帧的时间目标对象。
   *
   * 派生类应重写此函数以提供关键帧的时间目标上下文。
   * @return 指向 TimeTargetObject 对象的常量指针，默认为 nullptr。
   */
  [[nodiscard]] virtual const TimeTargetObject *GetKeyframeTimeTarget() const { return nullptr; }
  /**
   * @brief 获取在吸附时应忽略的 NodeKeyframe 对象列表。
   *
   * 派生类可以重写此函数以排除特定的关键帧不参与吸附。
   * @return 指向 NodeKeyframe 指针 std::vector 的常量指针，默认为 nullptr。
   */
  [[nodiscard]] virtual const std::vector<NodeKeyframe *> *GetSnapIgnoreKeyframes() const { return nullptr; }
  /**
   * @brief 获取在吸附时应忽略的 TimelineMarker 对象列表。
   *
   * 派生类可以重写此函数以排除特定的标记点不参与吸附。
   * @return 指向 TimelineMarker 指针 std::vector 的常量指针，默认为 nullptr。
   */
  [[nodiscard]] virtual const std::vector<TimelineMarker *> *GetSnapIgnoreMarkers() const { return nullptr; }

 protected slots: // 受保护槽函数
  /**
   * @brief Slot to center the horizontal scroll bar on the playhead's current position
   * (原始英文注释：将水平滚动条居中于播放头当前位置的槽函数)
   */
  void CenterScrollOnPlayhead();

  /**
   * @brief By default, TimeBasedWidget will set the timebase to the viewer node's video timebase.
   * Set this to false if you want to set your own timebase.
   * (原始英文注释：默认情况下，TimeBasedWidget 会将时间基准设置为查看器节点的视频时间基准。如果您想设置自己的时间基准，请将此设置为 false。)
   * @param e 如果为 true，则自动设置时间基准；否则不自动设置。
   */
  void SetAutoSetTimebase(bool e);

  /**
   * @brief 内部处理页面滚动的静态函数。
   * @param bar 指向 QScrollBar 对象的指针。
   * @param maximum 滚动条的最大值。
   * @param screen_position 屏幕上的目标位置（例如播放头位置）。
   * @param whole_page_scroll 是否执行整页滚动。
   */
  static void PageScrollInternal(QScrollBar *bar, int maximum, int screen_position, bool whole_page_scroll);

  /**
   * @brief 停止主滚动条的“追赶”滚动计时器。
   */
  void StopCatchUpScrollTimer() { StopCatchUpScrollTimer(scrollbar_); }

  /**
   * @brief 设置主滚动条的“追赶”滚动值。
   * @param v 新的滚动条值。
   */
  void SetCatchUpScrollValue(int v);

 signals: // 信号
  /**
   * @brief 当时间基准发生变化时发出此信号。
   * @param timebase 新的时间基准 (rational)。
   */
  void TimebaseChanged(const rational &timebase);

  /**
   * @brief 当连接的 ViewerOutput 节点发生变化时发出此信号。
   * @param old_node 指向前一个连接的 ViewerOutput 节点的指针。
   * @param new_node 指向当前连接的 ViewerOutput 节点的指针。
   */
  void ConnectedNodeChanged(ViewerOutput *old_node, ViewerOutput *new_node);

 protected slots: // 受保护槽函数 (再次出现，可能是代码风格或遗漏，保持原样)
  /**
   * @brief 发送“追赶”滚动事件的虚槽函数。
   *
   * 当需要平滑滚动到某个位置时，可能会调用此函数。
   */
  virtual void SendCatchUpScrollEvent();

 private: //私有成员
  /**
   * @brief Set either in or out point to the current playhead
   * (原始英文注释：将入点或出点设置到当前播放头位置)
   *
   * @param m Set to kTrimIn or kTrimOut for setting the in point or out point respectively.
   * (原始英文注释：设置为 kTrimIn 或 kTrimOut 分别用于设置入点或出点。)
   * @param time 要设置的时间点。
   */
  void SetPoint(Timeline::MovementMode m, const rational &time);

  /**
   * @brief Reset either the in or out point
   * (原始英文注释：重置入点或出点)
   *
   * Sets either the in point to 0 or the out point to `RATIONAL_MAX`.
   * (原始英文注释：将入点设置为0或将出点设置为 `RATIONAL_MAX`。)
   *
   * @param m Set to kTrimIn or kTrimOut for setting the in point or out point respectively.
   * (原始英文注释：设置为 kTrimIn 或 kTrimOut 分别用于设置入点或出点。)
   */
  void ResetPoint(Timeline::MovementMode m) const;

  /**
   * @brief 内部处理页面滚动的方法。
   * @param screen_position 屏幕上的目标位置。
   * @param whole_page_scroll 是否执行整页滚动。
   */
  void PageScrollInternal(int screen_position, bool whole_page_scroll);

  /**
   * @brief 检查用户当前是否正在拖动播放头。
   * @return 如果用户正在拖动播放头，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool UserIsDraggingPlayhead() const;

  ViewerOutput *viewer_node_; ///< 指向当前连接的 ViewerOutput 节点的指针。

  TimeRuler *ruler_; ///< 指向 TimeRuler 对象的指针，用于显示时间标尺。

  ResizableTimelineScrollBar *scrollbar_; ///< 指向 ResizableTimelineScrollBar 对象的指针，用于时间轴的滚动和缩放。

  bool auto_max_scrollbar_; ///< 标记是否自动设置滚动条的最大值。

  QList<TimeBasedView *> timeline_views_; ///< 存储与此控件关联的 TimeBasedView 实例列表。

  bool toggle_show_all_; ///< 标记“显示全部”模式是否启用。

  double toggle_show_all_old_scale_{}; ///< 在“显示全部”模式切换前保存的旧缩放比例。
  int toggle_show_all_old_scroll_{};   ///< 在“显示全部”模式切换前保存的旧滚动条位置。

  bool auto_set_timebase_; ///< 标记是否根据连接的 ViewerOutput 节点自动设置时间基准。

  int scrollbar_start_width_{};      ///< 滚动条开始调整大小时的宽度。
  double scrollbar_start_value_{};   ///< 滚动条开始调整大小时的值。
  double scrollbar_start_scale_{};   ///< 滚动条开始调整大小时的缩放比例。
  bool scrollbar_top_handle_{};      ///< 标记滚动条调整大小操作是否从顶部句柄开始。

  TimelineWorkArea *workarea_;       ///< 指向连接的时间轴工作区对象的指针。
  TimelineMarkerList *markers_;      ///< 指向连接的时间轴标记列表对象的指针。

  QTimer *catchup_scroll_timer_;     ///< 用于“追赶”滚动的 QTimer 指针。
  /**
   * @brief 存储“追赶”滚动数据的结构体。
   */
  struct CatchUpScrollData {
    qint64 last_forced = 0; ///< 上次强制滚动的时间戳
    int maximum{};          ///< 滚动条最大值
    int value{};            ///< 滚动条当前值
  };
  QMap<QScrollBar *, CatchUpScrollData> catchup_scroll_values_; ///< 存储每个滚动条的“追赶”滚动数据。

 private slots: // 私有槽函数
  /**
   * @brief 更新滚动条的最大值。
   */
  void UpdateMaximumScroll();

  /**
   * @brief 当滚动条开始调整大小时调用的槽函数。
   * @param current_bar_width 当前滚动条滑块的宽度。
   * @param top_handle 是否通过顶部句柄调整。
   */
  void ScrollBarResizeBegan(int current_bar_width, bool top_handle);

  /**
   * @brief 当滚动条调整大小过程中移动时调用的槽函数。
   * @param movement 移动的量。
   */
  void ScrollBarResizeMoved(int movement);

  /**
   * @brief Slot to handle page scrolling of the playhead
   * (原始英文注释：处理播放头页面滚动的槽函数)
   *
   * If the playhead is outside the current scroll bounds, this function will scroll to where it is. Otherwise it will
   * do nothing.
   * (原始英文注释：如果播放头超出了当前滚动范围，此函数会滚动到播放头所在位置。否则不执行任何操作。)
   */
  void PageScrollToPlayhead();

  /**
   * @brief “追赶”滚动到播放头位置的槽函数。
   */
  void CatchUpScrollToPlayhead();

  /**
   * @brief “追赶”滚动到指定点的槽函数。
   * @param point 目标滚动点。
   */
  void CatchUpScrollToPoint(int point);

  /**
   * @brief “追赶”滚动计时器超时槽函数。
   */
  void CatchUpTimerTimeout();

  /**
   * @brief 自动更新时间基准的槽函数。
   *
   * 通常在连接的 ViewerOutput 节点的时间基准发生变化时调用。
   */
  void AutoUpdateTimebase();

  /**
   * @brief 当连接的 ViewerOutput 节点从图中移除时调用的槽函数。
   */
  void ConnectedNodeRemovedFromGraph();

  /**
   * @brief 当播放头时间发生变化时调用的槽函数。
   * @param time 新的播放头时间 (rational)。
   */
  void PlayheadTimeChanged(const rational &time);
};

}  // namespace olive

#endif  // TIMEBASEDWIDGET_H
