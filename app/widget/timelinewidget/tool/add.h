#ifndef ADDTIMELINETOOL_H  // 防止头文件被多次包含的宏定义
#define ADDTIMELINETOOL_H

#include "beam.h"  // 引入 BeamTool 类的定义，AddTool 是 BeamTool 的派生类

namespace olive {  // olive 命名空间开始

/**
 * @brief AddTool 类是时间轴上的一个工具，用于通过拖动操作添加新的剪辑。
 *
 * 它继承自 BeamTool，这表明它可能共享一些与光束或范围选择相关的行为。
 * 这个工具允许用户在时间轴上指定一个范围来创建一个新的可添加剪辑 (addable clip)。
 */
class AddTool : public BeamTool {
 public:
  /**
   * @brief 构造一个 AddTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit AddTool(TimelineWidget *parent);

  /**
   * @brief 处理鼠标按下事件。
   *
   * 重写自基类 (可能是 Tool 或 BeamTool)。
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
   * @brief 创建一个可添加到时间轴的剪辑节点。
   *
   * 这是一个静态辅助函数，用于根据给定的参数创建一个新的剪辑 (Node)。
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录撤销/重做操作。
   * @param sequence 指向目标序列 (Sequence) 的指针。
   * @param track 对目标轨道 (Track) 的引用。
   * @param in 新剪辑的入点时间 (rational)。
   * @param length 新剪辑的持续时长 (rational)。
   * @param rect 可选参数，新剪辑在视图中的几何矩形 (QRectF)，默认为空矩形。
   * @return 指向新创建的剪辑节点 (Node) 的指针。
   */
  static Node *CreateAddableClip(MultiUndoCommand *command, Sequence *sequence, const Track::Reference &track,
                                 const rational &in, const rational &length, const QRectF &rect = QRectF());

 protected:  // 受保护成员
  /**
   * @brief 内部处理鼠标移动的逻辑。
   *
   * 在 MouseMove 事件中被调用，用于更新拖动状态和预览。
   * @param cursor_frame 当前光标位置对应的时间帧 (rational)。
   * @param outwards 一个布尔值，指示拖动方向是否向外扩展。
   */
  void MouseMoveInternal(const rational &cursor_frame, bool outwards);

  TimelineViewGhostItem *ghost_;  ///< 指向时间轴视图中的幽灵项 (ghost item) 的指针，用于在拖动时提供视觉反馈。

  rational drag_start_point_;  ///< 记录拖动操作开始时的时间点 (rational)。
};

}  // namespace olive

#endif  // ADDTIMELINETOOL_H
