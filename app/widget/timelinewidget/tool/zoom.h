#ifndef ZOOMTIMELINETOOL_H // 防止头文件被多次包含的宏定义
#define ZOOMTIMELINETOOL_H

#include "tool.h" // 引入 Tool 类的定义，ZoomTool 是 TimelineTool 的派生类

namespace olive { // olive 命名空间开始

/**
 * @brief ZoomTool 类是时间轴上的缩放工具。
 *
 * 它继承自 TimelineTool，专门用于通过鼠标拖拽操作来实现时间轴视图的缩放功能。
 * 用户可以通过按下鼠标、拖动来定义一个缩放区域或方向，释放鼠标后应用缩放。
 */
class ZoomTool : public TimelineTool {
public:
  /**
   * @brief 构造一个 ZoomTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit ZoomTool(TimelineWidget *parent);

  /**
   * @brief 处理鼠标按下事件。
   *
   * 重写自基类 (TimelineTool)。当用户使用缩放工具在时间轴上点击鼠标时调用，
   * 通常用于记录拖拽缩放的起始点。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MousePress(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理鼠标移动事件。
   *
   * 重写自基类 (TimelineTool)。当用户在使用缩放工具时移动鼠标（通常在按下按钮后），
   * 此方法被调用，可能用于绘制一个表示缩放区域的橡皮筋框或提供其他视觉反馈。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseMove(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理鼠标释放事件。
   *
   * 重写自基类 (TimelineTool)。当用户释放鼠标按钮时调用，
   * 此时会根据拖拽的范围或方向来执行实际的缩放操作。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseRelease(TimelineViewMouseEvent *event) override;

private: // 私有成员
  QPoint drag_global_start_; ///< 记录鼠标拖拽开始时的全局屏幕坐标。
};

}  // namespace olive

#endif  // ZOOMTIMELINETOOL_H
