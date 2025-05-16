#ifndef SLIPTIMELINETOOL_H // 防止头文件被多次包含的宏定义
#define SLIPTIMELINETOOL_H

#include "pointer.h" // 引入 PointerTool 类的定义，SlipTool 是 PointerTool 的派生类

namespace olive { // olive 命名空间开始

/**
 * @brief SlipTool 类是时间轴上的滑移编辑工具（Slip Edit Tool）。
 *
 * 它继承自 PointerTool，专门用于实现滑移编辑模式下的拖动操作。
 * 滑移编辑通常指的是在保持时间轴上剪辑的入点和出点位置不变（即持续时间不变）的情况下，
 * 改变剪辑内容在其窗口中的起始和结束帧。这相当于在剪辑的“窗口”内滑动其内容。
 */
class SlipTool : public PointerTool {
public:
  /**
   * @brief 构造一个 SlipTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit SlipTool(TimelineWidget *parent);

protected: // 受保护成员
  /**
   * @brief 处理拖动过程中的逻辑的虚函数（重写自 PointerTool 或其基类）。
   *
   * 在滑移编辑模式下，此函数根据鼠标的当前位置调整被滑移剪辑的内容的入点和出点，
   * 同时保持剪辑在时间轴上的位置和总时长不变。
   * @param mouse_pos 当前鼠标在时间轴坐标系中的位置 (TimelineCoordinate)。
   */
  void ProcessDrag(const TimelineCoordinate &mouse_pos) override;

  /**
   * @brief 完成拖动操作的虚函数（重写自 PointerTool 或其基类）。
   *
   * 在滑移编辑模式下，此函数在鼠标释放后被调用，
   * 用于最终确定滑移操作的结果并应用更改，例如更新剪辑的媒体入点/出点。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void FinishDrag(TimelineViewMouseEvent *event) override;
};

}  // namespace olive

#endif  // SLIPTIMELINETOOL_H
