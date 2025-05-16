#ifndef BEAMTIMELINETOOL_H  // 防止头文件被多次包含的宏定义
#define BEAMTIMELINETOOL_H

#include "tool.h"  // 引入 Tool 类的定义，BeamTool 是 TimelineTool (通过类声明推断) 的派生类

namespace olive {  // olive 命名空间开始

/**
 * @brief BeamTool 类是时间轴工具的基类之一，可能代表一种与“光束”或范围指示相关的工具。
 *
 * 它继承自 TimelineTool，提供了在时间轴视图中处理悬停移动事件的基础功能，
 * 以及一个用于验证时间轴坐标的方法。
 */
class BeamTool : public TimelineTool {
 public:
  /**
   * @brief 构造一个 BeamTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit BeamTool(TimelineWidget *parent);

  /**
   * @brief 处理鼠标悬停移动事件。
   *
   * 重写自基类 (TimelineTool)。
   * 当鼠标在时间轴视图中移动（未按下按钮）且此工具处于激活状态时，此方法被调用。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void HoverMove(TimelineViewMouseEvent *event) override;

 protected:  // 受保护成员
  /**
   * @brief 验证并可能调整给定的时间轴坐标。
   *
   * 此方法用于确保时间轴坐标在有效范围内或符合特定规则。
   * @param coord 要验证的 TimelineCoordinate 对象。
   * @return 返回经过验证（可能被调整）的 TimelineCoordinate 对象。
   */
  TimelineCoordinate ValidatedCoordinate(TimelineCoordinate coord);
};

}  // namespace olive

#endif  // BEAMTIMELINETOOL_H
