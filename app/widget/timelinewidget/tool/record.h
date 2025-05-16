#ifndef RECORDTIMELINETOOL_H // 防止头文件被多次包含的宏定义
#define RECORDTIMELINETOOL_H

#include "beam.h" // 引入 BeamTool 类的定义，RecordTool 是 BeamTool 的派生类

// 根据代码上下文，以下类型应由 "beam.h" 或其传递包含的头文件 "tool.h" 提供定义：
// - TimelineWidget: 用于构造函数参数
// - TimelineViewMouseEvent: 用于事件处理函数参数
// - rational: 用于方法参数和成员变量
// - TimelineViewGhostItem: 作为成员变量类型
// 此处严格按照用户提供的代码，不添加额外的 #include 或前向声明。

namespace olive { // olive 命名空间开始

/**
 * @brief RecordTool 类是时间轴上的一个录制工具。
 *
 * 它继承自 BeamTool，可能用于在时间轴上通过类似拖拽“光束”的方式来定义录制范围或进行录制操作。
 * 它处理鼠标的按下、移动和释放事件来控制录制相关的交互。
 */
class RecordTool : public BeamTool {
public:
  /**
   * @brief 构造一个 RecordTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit RecordTool(TimelineWidget* parent);

  /**
   * @brief 处理鼠标按下事件。
   *
   * 重写自基类 (BeamTool)。当用户使用录制工具在时间轴上点击鼠标时调用。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MousePress(TimelineViewMouseEvent* event) override;
  /**
   * @brief 处理鼠标移动事件。
   *
   * 重写自基类 (BeamTool)。当用户在使用录制工具时移动鼠标（通常在按下按钮后），
   * 此方法被调用，可能用于更新录制范围的预览。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseMove(TimelineViewMouseEvent* event) override;
  /**
   * @brief 处理鼠标释放事件。
   *
   * 重写自基类 (BeamTool)。当用户释放鼠标按钮时调用，
   * 此时可能会触发实际的录制操作或完成录制范围的设定。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseRelease(TimelineViewMouseEvent* event) override;

protected: // 受保护成员
  /**
   * @brief 内部处理鼠标移动的逻辑。
   *
   * 在 MouseMove 事件中被调用，用于更新拖动状态和可能的预览（例如幽灵项）。
   * @param cursor_frame 当前光标位置对应的时间帧 (rational)。
   * @param outwards 一个布尔值，可能指示拖动方向是否向外扩展范围。
   */
  void MouseMoveInternal(const rational& cursor_frame, bool outwards);

  TimelineViewGhostItem* ghost_; ///< 指向时间轴视图中的幽灵项 (ghost item) 的指针，用于在拖动时提供视觉反馈，指示将要录制的区域。

  rational drag_start_point_; ///< 记录拖动操作（定义录制范围）开始时的时间点 (rational)。
};

}  // namespace olive

#endif  // RECORDTOOL_H // 注意：此处的宏与 #ifndef/#define 中的 RECORDTIMELINETOOL_H 不同，保持原样
