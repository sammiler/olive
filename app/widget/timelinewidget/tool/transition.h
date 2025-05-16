#ifndef TRANSITIONTIMELINETOOL_H  // 防止头文件被多次包含的宏定义
#define TRANSITIONTIMELINETOOL_H

#include "add.h"  // 引入 AddTool 类的定义，TransitionTool 是 AddTool 的派生类

namespace olive {  // olive 命名空间开始

/**
 * @brief TransitionTool 类是时间轴上用于创建和编辑转场效果的工具。
 *
 * 它继承自 AddTool，这可能表明它利用了 AddTool 的一些与添加或定义范围相关的特性来创建转场。
 * TransitionTool 处理鼠标悬停、按下、移动和释放事件，以支持在剪辑之间交互式地应用转场。
 */
class TransitionTool : public AddTool {
 public:
  /**
   * @brief 构造一个 TransitionTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit TransitionTool(TimelineWidget *parent);

  /**
   * @brief 处理鼠标悬停移动事件。
   *
   * 重写自基类。当鼠标在时间轴视图中移动（未按下按钮）且此工具处于激活状态时调用。
   * 可能用于高亮显示可以应用转场的区域或更改鼠标光标样式。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void HoverMove(TimelineViewMouseEvent *event) override;

  /**
   * @brief 处理鼠标按下事件。
   *
   * 重写自基类。当用户使用转场工具在时间轴上点击鼠标时调用，
   * 通常用于开始定义转场的范围或选择要应用转场的剪辑边缘。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MousePress(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理鼠标移动事件。
   *
   * 重写自基类。当用户在使用转场工具时移动鼠标（通常在按下按钮后），
   * 此方法被调用，可能用于动态调整转场的长度或位置预览。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseMove(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理鼠标释放事件。
   *
   * 重写自基类。当用户释放鼠标按钮时调用，
   * 此时可能会创建或修改一个转场效果。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseRelease(TimelineViewMouseEvent *event) override;

 private:  // 私有方法
  /**
   * @brief 获取指定时间轴坐标处的剪辑块信息，用于确定转场应用的目标。
   *
   * 此方法会尝试找到坐标点附近的一个或两个 ClipBlock，并确定可能的修剪模式和转场起点。
   * @param coord 要检查的时间轴坐标 (TimelineCoordinate)。
   * @param primary 用于存储找到的主要剪辑块（例如左侧剪辑）的指针的指针 (ClipBlock**)。
   * @param secondary 用于存储找到的次要剪辑块（例如右侧剪辑）的指针的指针 (ClipBlock**)。
   * @param trim_mode 用于存储检测到的修剪模式的指针 (Timeline::MovementMode*)。
   * @param start_point 用于存储计算得到的转场起点的指针 (rational*)。
   * @return 如果成功找到可以应用转场的剪辑组合，则返回 true；否则返回 false。
   */
  bool GetBlocksAtCoord(const TimelineCoordinate &coord, ClipBlock **primary, ClipBlock **secondary,
                        Timeline::MovementMode *trim_mode, rational *start_point);

  bool dual_transition_{};  ///< 标记当前操作是否涉及双边转场（例如，同时影响入点和出点的转场或跨越两个剪辑的转场）。
};

}  // namespace olive

#endif  // TRANSITIONTIMELINETOOL_H
