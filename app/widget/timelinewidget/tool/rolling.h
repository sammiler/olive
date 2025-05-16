#ifndef ROLLINGTIMELINETOOL_H  // 防止头文件被多次包含的宏定义
#define ROLLINGTIMELINETOOL_H

#include "pointer.h"  // 引入 PointerTool 类的定义，RollingTool 是 PointerTool 的派生类

namespace olive {  // olive 命名空间开始

/**
 * @brief RollingTool 类是时间轴上的滚动编辑工具（Rolling Edit Tool）。
 *
 * 它继承自 PointerTool，专门用于实现滚动编辑模式下的拖动操作。
 * 滚动编辑通常指的是当调整一个剪辑的编辑点（入点或出点）时，
 * 相邻剪辑的对应编辑点会向相反方向移动，从而改变两个剪辑的长度，
 * 但保持它们组合的总时长不变，并且不影响时间轴上其他剪辑的位置。
 */
class RollingTool : public PointerTool {
 public:
  /**
   * @brief 构造一个 RollingTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit RollingTool(TimelineWidget* parent);

 protected:  // 受保护成员
  /**
   * @brief 初始化拖动操作的虚函数（重写自 PointerTool）。
   *
   * 在滚动编辑模式下，此函数负责在鼠标按下并开始拖动时，
   * 设置拖动操作的初始状态，为后续的滚动编辑做准备。
   * 它会确定被点击的条目以及初始的修剪模式和键盘修饰状态，
   * 并调用内部的拖动初始化逻辑。
   * @param clicked_item 指向被点击的 Block 对象的指针，该对象将成为拖动操作的主体或参考点。
   * @param trim_mode 拖动开始时的修剪模式 (Timeline::MovementMode)，指示是拖动入点、出点还是整个剪辑。
   * @param modifiers 当前的键盘修饰键状态 (Qt::KeyboardModifiers)，例如 Shift 或 Ctrl 键。
   */
  void InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers) override;
};

}  // namespace olive

#endif  // ROLLINGTIMELINETOOL_H
