#ifndef NODEINPUTDRAGGER_H // 防止头文件被多次包含的宏定义开始
#define NODEINPUTDRAGGER_H

#include "node/keyframe.h" // 引入 NodeKeyframe 定义，拖动操作可能涉及创建或修改关键帧
#include "node/param.h"    // 引入 NodeInput 和 NodeKeyframeTrackReference 等参数相关定义
#include "undo/undocommand.h" // 引入撤销命令相关定义，拖动操作应可撤销

// 可能需要的前向声明
// class QVariant; // 假设
// class rational; // 假设
// class MultiUndoCommand; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 用于处理节点输入参数值通过 Gizmo 或其他 UI 控件进行拖动修改的辅助类。
 * 当用户开始拖动一个与节点参数关联的 Gizmo 时，这个类会：
 * 1. 记录拖动开始时的参数值和时间。
 * 2. 在拖动过程中，实时更新参数值（预览效果）。
 * 3. 在拖动结束时，最终确定参数值，并可能创建关键帧，同时将整个操作封装成一个可撤销的命令。
 * 它还管理一个全局状态 `input_being_dragged` 来指示是否有参数正在被拖动。
 */
class NodeInputDragger {
 public:
  /**
   * @brief NodeInputDragger 构造函数。
   */
  NodeInputDragger();

  /**
   * @brief 检查拖动操作是否已经开始。
   * @return 如果已调用 Start() 且未调用 End()，则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsStarted() const;

  /**
   * @brief 开始一个参数拖动操作。
   * @param input 要拖动修改的节点输入参数的引用 (NodeKeyframeTrackReference)。
   * @param time 当前时间点，如果需要在拖动开始时创建关键帧，则使用此时间。
   * @param create_key_on_all_tracks (可选) 如果为 true (默认值)，并且输入参数是向量或颜色等复合类型，
   *                                   则可能在所有相关的子轨道（例如 R, G, B, A 或 X, Y, Z）上都创建关键帧。
   */
  void Start(const NodeKeyframeTrackReference &input, const rational &time, bool create_key_on_all_tracks = true);

  /**
   * @brief 在拖动过程中更新参数的值。
   *  此方法会临时改变参数值以供预览，但最终的值在 End() 中确定。
   * @param value 新的参数值 (通常是 QVariant 类型，因为参数可以是多种数据类型)。
   */
  void Drag(QVariant value);

  /**
   * @brief 结束一个参数拖动操作。
   *  此方法会最终设置参数值，如果需要则创建或修改关键帧，
   *  并将整个拖动操作（从 Start() 到 End()）封装成一个 MultiUndoCommand，以便可以撤销。
   * @param command 指向 MultiUndoCommand 的指针，用于聚合此拖动操作产生的多个子命令（如关键帧添加/修改命令）。
   */
  void End(MultiUndoCommand *command);

  /**
   * @brief (静态方法) 检查当前是否有任何输入参数正在被拖动。
   * @return 如果有参数正在被拖动，则返回 true，否则返回 false。
   */
  static bool IsInputBeingDragged() { return input_being_dragged > 0; } // 修改：如果 input_being_dragged 是计数器，则应 > 0

  /**
   * @brief 获取拖动开始时参数的初始值。
   * @return const QVariant& 对初始值的常量引用。
   */
  [[nodiscard]] const QVariant &GetStartValue() const { return start_value_; }

  /**
   * @brief 获取当前正在拖动的节点输入参数的引用。
   * @return const NodeKeyframeTrackReference& 对输入参数引用的常量引用。
   */
  [[nodiscard]] const NodeKeyframeTrackReference &GetInput() const { return input_; }

  /**
   * @brief 获取拖动操作开始时的时间点。
   * @return const rational& 对时间的常量引用。
   */
  [[nodiscard]] const rational &GetTime() const { return time_; }

 private:
  NodeKeyframeTrackReference input_; ///< 当前正在拖动修改的节点输入参数的引用。

  rational time_; ///< 拖动操作开始时的时间点。

  QVariant start_value_; ///< 拖动开始时参数的初始值。
  QVariant end_value_;   ///< 拖动结束时参数的最终值（在 Drag() 中临时更新，在 End() 中确认）。

  NodeKeyframe *dragging_key_{};      ///< 如果拖动操作直接修改一个已存在的关键帧，则此指针指向该关键帧。
  QVector<NodeKeyframe *> created_keys_; ///< 在拖动过程中（通常在 Start() 或 End() 时）新创建的关键帧列表。

  // 静态成员变量，用于全局追踪是否有任何 NodeInputDragger 实例当前处于活动拖动状态。
  // 这可能是一个简单的布尔标志，或者是一个计数器（如果允许多个拖动器同时活动，虽然不常见）。
  // 从 IsInputBeingDragged 的实现来看，它更可能是一个计数器。
  static int input_being_dragged; ///< 全局静态变量，指示是否有输入参数正在被拖动 (可能是一个计数器)。
};

}  // namespace olive

#endif  // NODEINPUTDRAGGER_H // 头文件宏定义结束