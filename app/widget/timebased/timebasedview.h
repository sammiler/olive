#ifndef TIMELINEVIEWBASE_H // 防止头文件被多次包含的宏定义
#define TIMELINEVIEWBASE_H

#include <QGraphicsView> // 引入 QGraphicsView 类，用于显示 QGraphicsScene 中的内容
#include <vector>        // 引入 std::vector，用于存储动态数组

#include "core.h"                                          // 引入核心定义或类型，具体内容需查看该文件
#include "timescaledobject.h"                              // 引入 TimeScaledObject 类，提供时间缩放相关功能
#include "widget/handmovableview/handmovableview.h"        // 引入 HandMovableView 类，提供视图的手动拖动功能

namespace olive {

class TimeBasedWidget; // 前向声明 TimeBasedWidget 类
class ViewerOutput;    // 前向声明 ViewerOutput 类 (从成员变量 viewer_ 推断)

/**
 * @brief TimeBasedView 类是一个基于时间轴的视图控件。
 *
 * 它继承自 HandMovableView（提供拖动和缩放功能）和 TimeScaledObject（提供时间缩放功能）。
 * 这个类通常用于显示与时间相关的图形元素，例如时间轴、关键帧编辑器等。
 * 它管理一个 QGraphicsScene，并处理播放头、吸附、缩放等交互。
 */
class TimeBasedView : public HandMovableView, public TimeScaledObject {
  Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

 public:
  /**
   * @brief 构造一个 TimeBasedView 对象。
   * @param parent 父 QWidget 对象，默认为 nullptr。
   */
  explicit TimeBasedView(QWidget *parent = nullptr);

  /**
   * @brief 启用吸附功能。
   * @param points 一个包含吸附时间点的 rational 类型向量。
   */
  void EnableSnap(const std::vector<rational> &points);

  /**
   * @brief 禁用吸附功能。
   */
  void DisableSnap();

  /**
   * @brief 检查当前是否启用了吸附功能。
   * @return 如果启用了吸附，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsSnapped() const { return snapped_; }

  /**
   * @brief 获取当前的吸附服务控件。
   * @return 指向 TimeBasedWidget 对象的指针，该对象提供吸附服务。
   */
  [[nodiscard]] TimeBasedWidget *GetSnapService() const { return snap_service_; }

  /**
   * @brief 设置吸附服务控件。
   * @param service 指向提供吸附服务的 TimeBasedWidget 对象的指针。
   */
  void SetSnapService(TimeBasedWidget *service) { snap_service_ = service; }

  /**
   * @brief 获取当前的 Y 轴缩放比例。
   * @return Y 轴的缩放比例 (double)。
   */
  [[nodiscard]] const double &GetYScale() const;

  /**
   * @brief 设置 Y 轴的缩放比例。
   * @param y_scale 新的 Y 轴缩放比例 (double)。
   */
  void SetYScale(const double &y_scale);

  /**
   * @brief 检查当前是否正在拖动播放头。
   * @return 如果正在拖动播放头，则返回 true；否则返回 false。
   */
  [[nodiscard]] virtual bool IsDraggingPlayhead() const { return dragging_playhead_; }

  // 仅供选择管理器调用
  /**
   * @brief 选择管理器选择事件。
   *
   * 当外部选择管理器选择一个对象时，可能会调用此虚函数。
   * 派生类可以重写此函数以处理特定逻辑。
   * @param obj 指向被选择对象的指针 (void*)。
   */
  virtual void SelectionManagerSelectEvent(void *obj) {}

  /**
   * @brief 选择管理器取消选择事件。
   *
   * 当外部选择管理器取消选择一个对象时，可能会调用此虚函数。
   * 派生类可以重写此函数以处理特定逻辑。
   * @param obj 指向被取消选择对象的指针 (void*)。
   */
  virtual void SelectionManagerDeselectEvent(void *obj) {}

  /**
   * @brief 获取关联的 ViewerOutput 节点。
   * @return 指向 ViewerOutput 对象的指针。
   */
  [[nodiscard]] ViewerOutput *GetViewerNode() const { return viewer_; }

  /**
   * @brief 设置关联的 ViewerOutput 节点。
   * @param v 指向 ViewerOutput 对象的指针。
   */
  void SetViewerNode(ViewerOutput *v);

  /**
   * @brief 根据当前视图的缩放比例转换点坐标。
   * @param p 要转换的点 (QPointF)。
   * @return 转换后的点坐标 (QPointF)。
   */
  [[nodiscard]] QPointF ScalePoint(const QPointF &p) const;

  /**
   * @brief 根据当前视图的缩放比例反向转换点坐标。
   * @param p 要反向转换的点 (QPointF)。
   * @return 反向转换后的点坐标 (QPointF)。
   */
  [[nodiscard]] QPointF UnscalePoint(const QPointF &p) const;

 public slots:
  /**
   * @brief 设置视图的结束时间。
   * @param length 新的结束时间 (rational)。
   */
  void SetEndTime(const rational &length);

  /**
   * @brief 当视图大小调整或场景内容更改时调用的槽函数，用于强制执行最小场景尺寸。
   */
  void UpdateSceneRect();

 signals:
  /**
   * @brief 当视图的（水平）缩放比例发生变化时发出此信号。
   * @param scale 新的缩放比例 (double)。
   */
  void ScaleChanged(double scale);

 protected:
  /**
   * @brief 重写 QGraphicsView::drawForeground()，用于在前景中绘制额外内容（例如播放头）。
   * @param painter QPainter 指针，用于绘图。
   * @param rect 需要重绘的区域 (QRectF)。
   */
  void drawForeground(QPainter *painter, const QRectF &rect) override;

  /**
   * @brief 重写 QWidget::resizeEvent()，处理视图大小调整事件。
   * @param event QResizeEvent 指针，包含事件参数。
   */
  void resizeEvent(QResizeEvent *event) override;

  /**
   * @brief 当时间缩放比例（通常是水平缩放）发生变化时调用的事件处理函数。
   *
   * 这是从 TimeScaledObject 继承并重写的虚函数。
   * @param scale 新的（水平）缩放比例 (double)。
   */
  void ScaleChangedEvent(const double &scale) override;

  /**
   * @brief 当场景矩形需要更新时调用的虚函数。
   *
   * 派生类可以重写此函数以自定义场景矩形的更新逻辑。
   * @param rect 对场景矩形 (QRectF) 的引用，可以在函数内修改。
   */
  virtual void SceneRectUpdateEvent(QRectF &) {}

  /**
   * @brief 当垂直缩放比例发生变化时调用的虚函数。
   * @param scale 新的垂直缩放比例 (double)。
   */
  virtual void VerticalScaleChangedEvent(double scale);

  /**
   * @brief 重写 HandMovableView::ZoomIntoCursorPosition()，实现以光标位置为中心的缩放。
   * @param event QWheelEvent 指针，包含滚轮事件参数。
   * @param multiplier 缩放乘数。
   * @param cursor_pos 光标在场景中的位置 (QPointF)。
   */
  void ZoomIntoCursorPosition(QWheelEvent *event, double multiplier, const QPointF &cursor_pos) override;

  /**
   * @brief 处理播放头按下事件。
   * @param event QMouseEvent 指针，包含鼠标事件参数。
   * @return 如果事件被处理，则返回 true；否则返回 false。
   */
  bool PlayheadPress(QMouseEvent *event);

  /**
   * @brief 处理播放头移动事件。
   * @param event QMouseEvent 指针，包含鼠标事件参数。
   * @return 如果事件被处理，则返回 true；否则返回 false。
   */
  bool PlayheadMove(QMouseEvent *event);

  /**
   * @brief 处理播放头释放事件。
   * @param event QMouseEvent 指针，包含鼠标事件参数。
   * @return 如果事件被处理，则返回 true；否则返回 false。
   */
  bool PlayheadRelease(QMouseEvent *event);

  /**
   * @brief 当时间基准（例如帧率）发生变化时调用的事件处理函数。
   *
   * 这是从 TimeScaledObject 继承并重写的虚函数。
   * @param timebase 新的时间基准 (rational)。
   */
  void TimebaseChangedEvent(const rational &) override;

  /**
   * @brief 检查 Y 轴是否启用。
   * @return 如果 Y 轴启用，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsYAxisEnabled() const { return y_axis_enabled_; }

  /**
   * @brief 设置 Y 轴是否启用。
   * @param e 如果为 true，则启用 Y 轴；否则禁用。
   */
  void SetYAxisEnabled(bool e) { y_axis_enabled_ = e; }

 private:
  /**
   * @brief 获取播放头在场景中的 X 坐标。
   * @return 播放头的 X 坐标 (qreal)。
   */
  qreal GetPlayheadX();

  /**
   * @brief 播放头在场景中的最左侧允许位置。
   */
  double playhead_scene_left_;

  /**
   * @brief 播放头在场景中的最右侧允许位置。
   */
  double playhead_scene_right_;

  /**
   * @brief 标记当前是否正在拖动播放头。
   */
  bool dragging_playhead_;

  /**
   * @brief 此视图管理的 QGraphicsScene 对象。
   */
  QGraphicsScene scene_;

  /**
   * @brief 标记吸附功能是否启用。
   */
  bool snapped_;

  /**
   * @brief 存储吸附时间点的向量。
   */
  std::vector<rational> snap_time_;

  /**
   * @brief 视图的结束时间。
   */
  rational end_time_;

  /**
   * @brief 指向提供吸附服务的 TimeBasedWidget 对象的指针。
   */
  TimeBasedWidget *snap_service_;

  /**
   * @brief 标记 Y 轴是否启用。
   *
   * 如果启用，视图可能会显示 Y 轴刻度或进行垂直缩放。
   */
  bool y_axis_enabled_;

  /**
   * @brief Y 轴的缩放比例。
   */
  double y_scale_;

  /**
   * @brief 指向关联的 ViewerOutput 对象的指针。
   *
   * 可能用于获取或显示与此视图相关的渲染输出。
   */
  ViewerOutput *viewer_;
};

}  // namespace olive

#endif  // TIMELINEVIEWBASE_H
