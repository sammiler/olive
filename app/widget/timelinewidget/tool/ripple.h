#ifndef RIPPLETIMELINETOOL_H // 防止头文件被多次包含的宏定义
#define RIPPLETIMELINETOOL_H

#include "pointer.h" // 引入 PointerTool 类的定义，RippleTool 是 PointerTool 的派生类

namespace olive { // olive 命名空间开始

/**
 * @brief RippleTool 类是时间轴上的涟漪编辑工具。
 *
 * 它继承自 PointerTool，专门用于实现涟漪编辑模式下的拖动操作。
 * 涟漪编辑意味着当调整一个剪辑的长度或位置时，后续的剪辑会自动向前或向后移动，
 * 以保持它们之间的时间间隔或填充/删除产生的间隙。
 */
class RippleTool : public PointerTool {
public:
  /**
   * @brief 构造一个 RippleTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit RippleTool(TimelineWidget* parent);

protected: // 受保护成员
  /**
   * @brief 完成拖动操作的虚函数（重写自 PointerTool）。
   *
   * 在涟漪编辑模式下，此函数负责在鼠标释放后应用拖动结果，
   * 包括调整相关剪辑的位置以实现涟漪效果。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void FinishDrag(TimelineViewMouseEvent* event) override;

  /**
   * @brief 初始化拖动操作的虚函数（重写自 PointerTool）。
   *
   * 在涟漪编辑模式下，此函数负责在鼠标按下并开始拖动时，
   * 设置拖动操作的初始状态，为后续的涟漪编辑做准备。
   * @param clicked_item 指向被点击的 Block 对象的指针，该对象将成为拖动操作的主体。
   * @param trim_mode 拖动开始时的修剪模式 (Timeline::MovementMode)。
   * @param modifiers 当前的键盘修饰键状态 (Qt::KeyboardModifiers)。
   */
  void InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers) override;
};

}  // namespace olive

#endif  // RIPPLETIMELINETOOL_H
