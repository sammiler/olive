#ifndef EDITTIMELINETOOL_H // 防止头文件被多次包含的宏定义
#define EDITTIMELINETOOL_H

#include "beam.h" // 引入 BeamTool 类的定义，EditTool 是 BeamTool 的派生类
#include "tool.h" // 引入 Tool 类的定义，是时间轴工具的更上层基类
#include "widget/timelinewidget/timelinewidgetselections.h" // 引入 TimelineWidgetSelections 类的定义，用于管理时间轴选择

namespace olive { // olive 命名空间开始


/**
 * @brief EditTool 类是时间轴上的一个编辑工具。
 *
 * 它继承自 BeamTool，提供了处理鼠标按下、移动、释放和双击事件的功能，
 * 以支持时间轴上的编辑操作。它会记录编辑开始时的选择状态和坐标。
 */
class EditTool : public BeamTool {
public:
  /**
   * @brief 构造一个 EditTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit EditTool(TimelineWidget *parent);

  /**
   * @brief 处理鼠标按下事件。
   *
   * 重写自基类 (BeamTool 或 Tool)。
   * 当用户在此工具激活时按下鼠标按钮，此方法被调用。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MousePress(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理鼠标移动事件。
   *
   * 重写自基类。
   * 当用户在此工具激活时移动鼠标（通常在按下按钮后），此方法被调用。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseMove(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理鼠标释放事件。
   *
   * 重写自基类。
   * 当用户在此工具激活时释放鼠标按钮，此方法被调用。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseRelease(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理鼠标双击事件。
   *
   * 重写自基类。
   * 当用户在此工具激活时双击鼠标按钮，此方法被调用。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseDoubleClick(TimelineViewMouseEvent *event) override;

private: // 私有成员
  TimelineWidgetSelections start_selections_; ///< 记录鼠标按下时时间轴上的选择状态。

  TimelineCoordinate start_coord_; ///< 记录鼠标按下时的初始时间轴坐标。
};

}  // namespace olive

#endif  // EDITTIMELINETOOL_H
