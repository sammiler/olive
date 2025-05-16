#ifndef SEEKABLEWIDGET_H  // 防止头文件被多次包含的宏定义
#define SEEKABLEWIDGET_H

#include <QHBoxLayout>  // 引入 QHBoxLayout 类，用于水平布局
#include <QScrollBar>   // 引入 QScrollBar 类，用于滚动条功能

#include "widget/menu/menu.h"                                // 引入 Menu 类的定义，可能用于上下文菜单
#include "widget/timebased/timebasedviewselectionmanager.h"  // 引入 TimeBasedViewSelectionManager 模板类的定义，用于管理时间轴上对象的选择

namespace olive {  // olive 命名空间开始

/**
 * @brief SeekableWidget 类是一个可定位的视图控件，通常用于显示时间轴标尺或类似内容。
 *
 * 它继承自 TimeBasedView，提供了在时间轴上显示标记点 (Markers)、工作区 (WorkArea)
 * 和播放头的功能。它还管理这些元素的选择和编辑。
 */
class SeekableWidget : public TimeBasedView {
 Q_OBJECT  // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

     public :
     /**
      * @brief 构造一个 SeekableWidget 对象。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit SeekableWidget(QWidget *parent = nullptr);

  /**
   * @brief 获取当前水平滚动条的值。
   * @return 水平滚动条的当前值 (int)。
   */
  [[nodiscard]] int GetScroll() const { return horizontalScrollBar()->value(); }

  /**
   * @brief 获取关联的标记点列表。
   * @return 指向 TimelineMarkerList 对象的指针。
   */
  [[nodiscard]] TimelineMarkerList *GetMarkers() const { return markers_; }
  /**
   * @brief 获取关联的工作区对象。
   * @return 指向 TimelineWorkArea 对象的指针。
   */
  [[nodiscard]] TimelineWorkArea *GetWorkArea() const { return workarea_; }

  /**
   * @brief 设置关联的标记点列表。
   * @param markers 指向 TimelineMarkerList 对象的指针。
   */
  void SetMarkers(TimelineMarkerList *markers);
  /**
   * @brief 设置关联的工作区对象。
   * @param workarea 指向 TimelineWorkArea 对象的指针。
   */
  void SetWorkArea(TimelineWorkArea *workarea);

  /**
   * @brief 检查当前是否正在拖动播放头 (重写自 TimeBasedView)。
   * @return 如果正在拖动播放头，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsDraggingPlayhead() const override { return dragging_; }

  /**
   * @brief 检查标记点编辑功能是否已启用。
   * @return 如果标记点编辑已启用，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsMarkerEditingEnabled() const { return marker_editing_enabled_; }
  /**
   * @brief 设置标记点编辑功能是否启用。
   * @param e 如果为 true，则启用标记点编辑。
   */
  void SetMarkerEditingEnabled(bool e) { marker_editing_enabled_ = e; }

  /**
   * @brief 删除当前选中的标记点。
   */
  void DeleteSelected();

  /**
   * @brief 复制选中的标记点。
   * @param cut 如果为 true，则执行剪切操作（复制并删除）；否则执行复制操作。
   * @return 如果操作成功（例如有内容被复制），则返回 true；否则返回 false。
   */
  bool CopySelected(bool cut);

  /**
   * @brief 粘贴标记点。
   * @return 如果粘贴操作成功，则返回 true；否则返回 false。
   */
  bool PasteMarkers();

  /**
   * @brief 取消选择所有标记点。
   */
  void DeselectAllMarkers();

  /**
   * @brief 将播放头定位到指定的场景X坐标。
   * @param scene 目标场景X坐标 (qreal)。
   */
  void SeekToScenePoint(qreal scene);

  /**
   * @brief 检查当前是否有任何标记点被选中。
   * @return 如果有标记点被选中，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool HasItemsSelected() const { return !selection_manager_.GetSelectedObjects().empty(); }

  /**
   * @brief 获取当前所有选中的标记点。
   * @return 对选中 TimelineMarker 指针向量的常量引用。
   */
  [[nodiscard]] const std::vector<TimelineMarker *> &GetSelectedMarkers() const {
    return selection_manager_.GetSelectedObjects();
  }

  /**
   * @brief 处理来自选择管理器的对象选择事件 (重写自 TimeBasedView)。
   * @param obj 指向被选择对象的指针 (void*)，在此上下文中应为 TimelineMarker*。
   */
  void SelectionManagerSelectEvent(void *obj) override;
  /**
   * @brief 处理来自选择管理器的对象取消选择事件 (重写自 TimeBasedView)。
   * @param obj 指向被取消选择对象的指针 (void*)，在此上下文中应为 TimelineMarker*。
   */
  void SelectionManagerDeselectEvent(void *obj) override;

  /**
   * @brief 处理“追赶”滚动事件 (重写自 TimeBasedView 或其基类)。
   *
   * 当需要平滑滚动到某个位置时（例如，播放头移出可视区域），可能会调用此函数。
   */
  void CatchUpScrollEvent() override;

 public slots:  // 公共槽函数
  /**
   * @brief 设置水平滚动条的值。
   * @param i 新的滚动条值。
   */
  void SetScroll(int i) { horizontalScrollBar()->setValue(i); }

  /**
   * @brief 当时间基准发生变化时调用的槽函数 (重写自 TimeBasedView 或其基类)。
   * @param timebase 新的时间基准 (rational)。 // 参数名根据上下文推断，原始代码中省略
   */
  void TimebaseChangedEvent(const rational &) override;

 signals:  // 信号
  /**
   * @brief 当拖动操作移动时发出此信号。
   * @param x 鼠标的X坐标。
   * @param y 鼠标的Y坐标。
   */
  void DragMoved(int x, int y);

  /**
   * @brief 当拖动操作释放时发出此信号。
   */
  void DragReleased();

 protected:  // 受保护的重写方法
  /**
   * @brief 重写 QWidget::mousePressEvent()，处理鼠标按下事件。
   * @param event 指向 QMouseEvent 对象的指针。
   */
  void mousePressEvent(QMouseEvent *event) override;
  /**
   * @brief 重写 QWidget::mouseMoveEvent()，处理鼠标移动事件。
   * @param event 指向 QMouseEvent 对象的指针。
   */
  void mouseMoveEvent(QMouseEvent *event) override;
  /**
   * @brief 重写 QWidget::mouseReleaseEvent()，处理鼠标释放事件。
   * @param event 指向 QMouseEvent 对象的指针。
   */
  void mouseReleaseEvent(QMouseEvent *event) override;
  /**
   * @brief 重写 QWidget::mouseDoubleClickEvent()，处理鼠标双击事件。
   * @param event 指向 QMouseEvent 对象的指针。
   */
  void mouseDoubleClickEvent(QMouseEvent *event) override;

  /**
   * @brief 重写 QWidget::focusOutEvent()，处理焦点移出事件。
   * @param event 指向 QFocusEvent 对象的指针。
   */
  void focusOutEvent(QFocusEvent *event) override;

  /**
   * @brief 绘制标记点。
   * @param p QPainter 指针，用于绘图。
   * @param marker_bottom 标记点绘制区域的底部Y坐标，默认为0（可能表示使用内部计算的值）。
   */
  void DrawMarkers(QPainter *p, int marker_bottom = 0);
  /**
   * @brief 绘制工作区（通常是入点和出点之间的区域）。
   * @param p QPainter 指针，用于绘图。
   */
  void DrawWorkArea(QPainter *p);

  /**
   * @brief 绘制播放头。
   * @param p QPainter 指针，用于绘图。
   * @param x 播放头在视图中的X坐标。
   * @param y 播放头在视图中的Y坐标（通常是顶部）。
   */
  void DrawPlayhead(QPainter *p, int x, int y);

  /**
   * @brief 获取用于绘制文本的高度。
   * @return 文本高度 (int)。
   */
  [[nodiscard]] inline const int &text_height() const { return text_height_; }

  /**
   * @brief 获取播放头的宽度。
   * @return 播放头宽度 (int)。
   */
  [[nodiscard]] inline const int &playhead_width() const { return playhead_width_; }

  /**
   * @brief 获取视图内容的左边界限制。
   * @return 左边界的X坐标 (int)。
   */
  [[nodiscard]] int GetLeftLimit() const;
  /**
   * @brief 获取视图内容的右边界限制。
   * @return 右边界的X坐标 (int)。
   */
  [[nodiscard]] int GetRightLimit() const;

 protected slots:  // 受保护槽函数
  /**
   * @brief 显示上下文菜单的虚函数。
   *
   * 派生类可以重写此方法以提供自定义的上下文菜单。
   * @param p 鼠标点击的本地坐标 (QPoint)，用于定位菜单。
   * @return 如果菜单被显示并处理了事件，则返回 true；否则返回 false。
   */
  virtual bool ShowContextMenu(const QPoint &p);

 private:  // 私有枚举和成员变量
  /**
   * @brief ResizeMode 枚举定义了调整大小操作的模式。
   */
  enum ResizeMode {
    kResizeNone,  ///< 无调整大小操作
    kResizeIn,    ///< 正在调整入点
    kResizeOut    ///< 正在调整出点
  };

  /**
   * @brief 查找鼠标事件位置是否在某个可调整大小的句柄上（例如标记的边缘）。
   * @param event QMouseEvent 指针。
   * @return 如果找到了可调整大小的句柄，则返回 true；否则返回 false。
   */
  bool FindResizeHandle(QMouseEvent *event);

  /**
   * @brief 清除当前活动的调整大小句柄状态。
   */
  void ClearResizeHandle();

  /**
   * @brief 在拖动调整大小句柄时处理逻辑。
   * @param scene_pos 当前鼠标在场景中的位置 (QPointF)。
   */
  void DragResizeHandle(const QPointF &scene_pos);

  /**
   * @brief 提交调整大小句柄的操作结果。
   */
  void CommitResizeHandle();

  TimelineMarkerList *markers_;  ///< 指向关联的标记点列表对象的指针。
  TimelineWorkArea *workarea_;   ///< 指向关联的工作区对象的指针。

  int text_height_;  ///< 用于绘制文本的预设高度。

  int playhead_width_;  ///< 播放头的宽度。

  bool dragging_;  ///< 标记当前是否正在拖动播放头或标记点。

  bool ignore_next_focus_out_;  ///< 标记是否应忽略下一次的焦点移出事件。

  TimeBasedViewSelectionManager<TimelineMarker> selection_manager_;  ///< 用于管理 TimelineMarker 选择的实例。

  QObject *resize_item_;         ///< 指向当前正在调整大小的对象的指针 (可能是 TimelineMarker 或 TimelineWorkArea)。
  ResizeMode resize_mode_;       ///< 当前的调整大小模式。
  TimeRange resize_item_range_;  ///< 正在调整大小的对象的原始时间范围。
  QPointF resize_start_;         ///< 调整大小操作开始时的鼠标场景坐标。
  uint32_t resize_snap_mask_{};  ///< 调整大小操作期间使用的吸附掩码。

  int marker_top_;     ///< 标记点绘制区域的顶部Y坐标。
  int marker_bottom_;  ///< 标记点绘制区域的底部Y坐标。

  bool marker_editing_enabled_;  ///< 标记标记点编辑功能是否启用。

  QPolygon last_playhead_shape_;  ///< 上一次绘制的播放头形状，用于高效重绘。

 private slots:  // 私有槽函数
  /**
   * @brief 设置选中标记点的颜色。
   * @param c 颜色索引或 QColor 的某种表示（具体取决于实现）。
   */
  void SetMarkerColor(int c);

  /**
   * @brief 显示选中标记点的属性对话框。
   */
  void ShowMarkerProperties();
};

}  // namespace olive

#endif  // SEEKABLEWIDGET_H
