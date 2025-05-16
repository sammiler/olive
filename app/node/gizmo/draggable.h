#ifndef DRAGGABLEGIZMO_H  // 防止头文件被多次包含的宏定义开始
#define DRAGGABLEGIZMO_H

#include "gizmo.h"              // 引入基类 NodeGizmo (或类似 Gizmo 基类) 的定义
#include "node/inputdragger.h"  // 引入 NodeInputDragger，用于处理 Gizmo 拖动时节点参数的平滑变化和关键帧记录
#include "undo/undocommand.h"   // 引入撤销命令相关定义，用于支持 Gizmo 操作的撤销/重做

// 前向声明或在此未完全定义的类型
namespace olive {
namespace core {
class rational;  // 假设 rational 类型在 core 命名空间内
}
// class NodeValueRow; // 假设
// class NodeKeyframeTrackReference; // 假设
// class MultiUndoCommand; // 假设
}  // namespace olive

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个可拖动的 Gizmo (交互控件) 的基类。
 * 提供了处理 Gizmo 拖动开始、移动和结束的通用逻辑，
 * 以及如何将这些拖动操作转换为节点参数的变化，并支持撤销/重做。
 */
class DraggableGizmo : public NodeGizmo {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief 定义 HandleMovement 信号发出的 X/Y 坐标的含义。
      */
     enum DragValueBehavior {
       kAbsolute,           ///< X/Y 坐标是鼠标在序列（画布）像素坐标系中的绝对位置。
       kDeltaFromPrevious,  ///< X/Y 坐标是自上次调用 HandleMovement 以来的移动差量。
       kDeltaFromStart      ///< X/Y 坐标是自拖动开始以来的总移动差量。
     };

  /**
   * @brief DraggableGizmo 构造函数。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  explicit DraggableGizmo(QObject *parent = nullptr);

  /**
   * @brief 处理 Gizmo 拖动开始事件。
   *  通常由视图或控制器在用户开始拖动 Gizmo 时调用。
   * @param row 与 Gizmo 关联的节点当前参数值。
   * @param abs_x 拖动开始时鼠标的绝对 X 坐标（序列像素坐标）。
   * @param abs_y 拖动开始时鼠标的绝对 Y 坐标（序列像素坐标）。
   * @param time 当前时间点，用于记录关键帧。
   */
  void DragStart(const NodeValueRow &row, double abs_x, double abs_y, const olive::core::rational &time);

  /**
   * @brief 处理 Gizmo 拖动过程中的移动事件。
   *  根据 drag_value_behavior_ 设置，发射 HandleMovement 信号。
   * @param x 鼠标 X 坐标（含义取决于 drag_value_behavior_）。
   * @param y 鼠标 Y 坐标（含义取决于 drag_value_behavior_）。
   * @param modifiers 当前按下的键盘修饰键 (如 Shift, Ctrl)。
   */
  void DragMove(double x, double y, const Qt::KeyboardModifiers &modifiers);

  /**
   * @brief 处理 Gizmo 拖动结束事件。
   *  完成参数的最终设置，并可能将操作记录到撤销栈。
   * @param command 指向 MultiUndoCommand 的指针，用于聚合此拖动操作产生的多个撤销命令。
   */
  void DragEnd(olive::MultiUndoCommand *command);

  /**
   * @brief 向此 Gizmo 添加一个它将要控制的节点输入参数的引用。
   *  同时会为该输入参数创建一个对应的 NodeInputDragger。
   * @param input 对节点关键帧轨道（输入参数）的引用。
   */
  void AddInput(const NodeKeyframeTrackReference &input) {
    inputs_.append(input);                 // 将输入参数引用添加到列表中
    draggers_.append(NodeInputDragger());  // 为此输入参数添加一个新的拖动处理器
  }

  /**
   * @brief 获取与此 Gizmo 关联的所有 NodeInputDragger 对象的引用。
   * @return QVector<NodeInputDragger>& 对 NodeInputDragger 向量的引用。
   */
  QVector<NodeInputDragger> &GetDraggers() { return draggers_; }

  /**
   * @brief 获取当前拖动值的行为方式。
   * @return DragValueBehavior 枚举值。
   */
  [[nodiscard]] DragValueBehavior GetDragValueBehavior() const { return drag_value_behavior_; }
  /**
   * @brief 设置拖动值的行为方式。
   * @param d新的 DragValueBehavior 枚举值。
   */
  void SetDragValueBehavior(DragValueBehavior d) { drag_value_behavior_ = d; }

 signals:  // Qt 信号声明区域
  /**
   * @brief 当 Gizmo 拖动开始时发射此信号。
   *  具体的 Gizmo 子类会连接到此信号，以执行其特定的拖动开始逻辑。
   * @param row 节点当前的参数值。
   * @param x 拖动开始时的 X 坐标（含义取决于 drag_value_behavior_，但通常对于 Start 是绝对坐标）。
   * @param y 拖动开始时的 Y 坐标。
   * @param time 当前时间点。
   */
  void HandleStart(const olive::NodeValueRow &row, double x, double y, const olive::core::rational &time);

  /**
   * @brief 当 Gizmo 在拖动过程中移动时发射此信号。
   *  具体的 Gizmo 子类会连接到此信号，以根据移动更新其状态或预览。
   * @param x 移动的 X 坐标（含义取决于 drag_value_behavior_）。
   * @param y 移动的 Y 坐标。
   * @param modifiers 当前按下的键盘修饰键。
   */
  void HandleMovement(double x, double y, const Qt::KeyboardModifiers &modifiers);

 private:
  QVector<NodeKeyframeTrackReference> inputs_;  ///< 存储此 Gizmo 控制的节点输入参数的引用列表。

  QVector<NodeInputDragger> draggers_;  ///< 存储与每个受控输入参数对应的 NodeInputDragger 对象列表。
                                        ///< NodeInputDragger 负责实际修改参数值并处理关键帧。

  DragValueBehavior drag_value_behavior_;  ///< 控制 HandleMovement 信号发出的坐标值的行为方式。
};

}  // namespace olive

#endif  // DRAGGABLEGIZMO_H // 头文件宏定义结束