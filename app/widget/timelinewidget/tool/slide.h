#ifndef SLIDETIMELINETOOL_H // 防止头文件被多次包含的宏定义
#define SLIDETIMELINETOOL_H

#include "pointer.h" // 引入 PointerTool 类的定义，SlideTool 是 PointerTool 的派生类

namespace olive { // olive 命名空间开始

/**
 * @brief SlideTool 类是时间轴上的滑动编辑工具（Slide Edit Tool）。
 *
 * 它继承自 PointerTool，专门用于实现滑动编辑模式下的拖动操作。
 * 滑动编辑通常指的是移动一个剪辑在时间轴上的位置，但保持其持续时间不变，
 * 同时其左右相邻剪辑的持续时间会相应地缩短或延长，以填充由于被移动剪辑
 * 位置变化而产生的间隙或被覆盖的部分。整个序列的总时长通常保持不变。
 */
class SlideTool : public PointerTool {
public:
  /**
   * @brief 构造一个 SlideTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit SlideTool(TimelineWidget* parent);

protected: // 受保护成员
  /**
   * @brief 初始化拖动操作的虚函数（重写自 PointerTool）。
   *
   * 在滑动编辑模式下，此函数负责在鼠标按下并开始拖动时，
   * 设置拖动操作的初始状态，为后续的滑动编辑做准备。
   * 它会确定被点击的条目以及初始的修剪模式（尽管在滑动编辑中，修剪模式可能不直接适用，
   * 但基类方法可能需要它）和键盘修饰状态，并调用内部的拖动初始化逻辑，
   * 特别设置 `slide_instead_of_moving` 参数为 true。
   * @param clicked_item 指向被点击的 Block 对象的指针，该对象将成为执行滑动操作的主体。
   * @param trim_mode 拖动开始时的修剪模式 (Timeline::MovementMode)。对于滑动工具，这可能被内部忽略或特殊处理。
   * @param modifiers 当前的键盘修饰键状态 (Qt::KeyboardModifiers)，例如 Shift 或 Ctrl 键。
   */
  void InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers) override;
};

}  // namespace olive

#endif  // SLIDETIMELINETOOL_H
