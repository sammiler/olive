#ifndef HANDMOVABLEVIEW_H
#define HANDMOVABLEVIEW_H

#include <QGraphicsView> // Qt 图形视图基类
#include <QMenu>         // Qt 菜单类 (虽然在此头文件中未直接使用 QMenu 对象，但可能在 .cpp 文件中使用或为派生类提供上下文)
#include <QWidget>       // Qt 控件基类 (作为 HandMovableView 的基类 QGraphicsView 的父类)

#include "tool/tool.h"   // 项目中定义的工具枚举和相关类

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWheelEvent;
// class QMouseEvent;
// class QPointF;

namespace olive {

/**
 * @brief HandMovableView 类是一个 QGraphicsView 的派生类，增加了手型工具平移和鼠标滚轮缩放功能。
 *
 * 它允许用户通过按住特定鼠标按键（通常是中键或激活手型工具时的主键）来拖动视图内容，
 * 以及通过鼠标滚轮来缩放视图。
 * 该类还能够响应应用程序全局工具的切换，以相应地改变其行为。
 */
class HandMovableView : public QGraphicsView {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit HandMovableView(QWidget* parent = nullptr);

  /**
   * @brief 静态方法，判断一个鼠标滚轮事件是否应被解释为缩放事件。
   *
   * 通常检查是否有特定的修饰键被按下（例如 Ctrl 键）。
   * @param event 鼠标滚轮事件指针。
   * @return 如果事件应触发缩放，则返回 true；否则返回 false。
   */
  static bool WheelEventIsAZoomEvent(QWheelEvent* event);

  /**
   * @brief 静态方法，根据鼠标滚轮事件计算缩放乘数。
   *
   * 滚轮向上滚动通常对应大于 1.0 的乘数（放大），向下滚动则小于 1.0（缩小）。
   * @param event 鼠标滚轮事件指针。
   * @return 计算得到的缩放乘数 (qreal 类型)。
   */
  static qreal GetScrollZoomMultiplier(QWheelEvent* event);

  /**
   * @brief 虚函数，用于在滚动事件后执行一些“追赶”操作。
   *
   * 派生类可以重写此函数以在视图滚动或缩放后执行特定更新。
   * 例如，在时间轴中，这可能用于确保标尺或播放头与视图同步。
   */
  virtual void CatchUpScrollEvent() {}

 protected:
  /**
   * @brief 当应用程序的全局工具发生改变时调用的虚函数。
   *
   * 派生类可以重写此函数以响应工具的切换，例如改变视图的拖拽模式或光标样式。
   * @param tool 新激活的工具项 (Tool::Item 枚举值)。
   */
  virtual void ToolChangedEvent(Tool::Item tool) { Q_UNUSED(tool) } // Q_UNUSED 用于避免未使用参数的编译器警告

  /**
   * @brief 处理手型工具的鼠标按下事件。
   *
   * 如果当前工具是手型工具或按下了手型工具的激活键（如鼠标中键），
   * 则开始平移操作。
   * @param event 鼠标事件指针。
   * @return 如果事件被处理（即开始了平移），则返回 true；否则返回 false。
   */
  bool HandPress(QMouseEvent* event);
  /**
   * @brief 处理手型工具的鼠标移动事件。
   *
   * 如果正在进行平移操作，则根据鼠标的移动来滚动视图内容。
   * @param event 鼠标事件指针。
   * @return 如果事件被处理（即执行了平移），则返回 true；否则返回 false。
   */
  bool HandMove(QMouseEvent* event);
  /**
   * @brief 处理手型工具的鼠标释放事件。
   *
   * 如果正在进行平移操作，则结束平移。
   * @param event 鼠标事件指针。
   * @return 如果事件被处理（即结束了平移），则返回 true；否则返回 false。
   */
  bool HandRelease(QMouseEvent* event);

  /**
   * @brief 设置视图的默认拖拽模式。
   * @param mode 要设置的 QGraphicsView::DragMode。
   */
  void SetDefaultDragMode(DragMode mode);
  /**
   * @brief 获取视图的默认拖拽模式。
   * @return 返回当前设置的默认 QGraphicsView::DragMode。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const DragMode& GetDefaultDragMode() const;

  /**
   * @brief 鼠标滚轮事件处理函数。
   *
   * 主要用于实现视图的缩放功能。
   * @param event 鼠标滚轮事件指针。
   */
  void wheelEvent(QWheelEvent* event) override;

  /**
   * @brief 虚函数，根据光标位置和乘数进行缩放。
   *
   * 派生类可以重写此函数以实现自定义的缩放行为，例如围绕光标位置进行缩放。
   * @param event 原始的鼠标滚轮事件指针。
   * @param multiplier 缩放乘数。
   * @param cursor_pos 鼠标光标在场景中的位置。
   */
  virtual void ZoomIntoCursorPosition(QWheelEvent* event, double multiplier, const QPointF& cursor_pos);

  /**
   * @brief 设置当前视图是否代表时间轴的坐标轴区域。
   *
   * 这个标志可能会影响某些交互行为或绘制逻辑。
   * @param e 如果是时间轴坐标轴区域，则为 true；否则为 false。
   */
  void SetIsTimelineAxes(bool e) { is_timeline_axes_ = e; }

 private:
  bool dragging_hand_;            ///< 标记当前是否正在通过手型工具进行拖拽平移。
  DragMode pre_hand_drag_mode_; ///< 在开始手型工具拖拽之前视图的原始拖拽模式。

  DragMode default_drag_mode_;    ///< 视图的默认拖拽模式。

  QPointF transformed_pos_; ///< 在手型工具拖拽开始时，鼠标在场景坐标系中的转换后位置。

  bool is_timeline_axes_; ///< 标记此视图是否作为时间轴的坐标轴部分使用。

 private slots:
  /**
   * @brief 当应用程序的全局工具选择发生改变时调用的槽函数。
   * @param tool 新选中的工具项 (Tool::Item 枚举值)。
   */
  void ApplicationToolChanged(Tool::Item tool);
};

}  // namespace olive

#endif  // HANDMOVABLEVIEW_H
