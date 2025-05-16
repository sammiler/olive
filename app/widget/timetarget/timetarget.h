#ifndef TIMETARGETOBJECT_H  // 防止头文件被多次包含的宏定义
#define TIMETARGETOBJECT_H

#include "node/output/viewer/viewer.h"  // 引入 ViewerOutput 类的定义，用于时间目标

namespace olive {  // olive 命名空间开始

/**
 * @brief TimeTargetObject 类用于处理不同节点之间的时间转换和同步。
 *
 * 当需要在不同的时间上下文（例如，一个序列节点和其内部的剪辑节点）之间映射时间值时，
 * 此类提供了一个机制来指定一个“时间目标”节点 (通常是一个 ViewerOutput)，
 * 并根据这个目标来调整时间值。这对于确保在复杂节点图中时间的一致性非常重要，
 * 特别是在处理嵌套序列或具有不同时间基准的元素时。
 */
class TimeTargetObject {
 public:
  /**
   * @brief 构造一个 TimeTargetObject 对象。
   *
   * 初始化 time_target_ 为 nullptr 和 path_index_ 为 -1 (或默认值)。
   */
  TimeTargetObject();

  /**
   * @brief 获取当前设置的时间目标节点。
   * @return 指向 ViewerOutput 对象的指针，该对象是当前的时间目标。如果未设置，则为 nullptr。
   */
  [[nodiscard]] ViewerOutput* GetTimeTarget() const;
  /**
   * @brief 设置时间目标节点。
   *
   * 当时间目标改变时，会触发相应的事件回调 (TimeTargetChangedEvent, TimeTargetConnectEvent,
   * TimeTargetDisconnectEvent)。
   * @param target 指向新的时间目标 ViewerOutput 对象的指针。
   */
  void SetTimeTarget(ViewerOutput* target);

  /**
   * @brief 设置路径索引。
   *
   * 路径索引可能用于在多路径时间转换中指定特定的转换路径。
   * @param index 新的路径索引 (int)。
   */
  void SetPathIndex(int index);

  /**
   * @brief 根据当前时间目标，将给定的时间点从一个节点的上下文调整（转换）到另一个节点的上下文。
   * @param from 源节点 (Node*)，时间点 `r` 是相对于此节点的。
   * @param to 目标节点 (Node*)，返回的时间点将是相对于此节点的。
   * @param r 要调整的时间点 (rational)。
   * @param dir 时间转换的方向 (Node::TransformTimeDirection)，例如从输入到输出或从输出到输入。
   * @return 调整后的时间点 (rational)。
   */
  rational GetAdjustedTime(Node* from, Node* to, const rational& r, Node::TransformTimeDirection dir) const;
  /**
   * @brief 根据当前时间目标，将给定的时间范围从一个节点的上下文调整（转换）到另一个节点的上下文。
   * @param from 源节点 (Node*)，时间范围 `r` 是相对于此节点的。
   * @param to 目标节点 (Node*)，返回的时间范围将是相对于此节点的。
   * @param r 要调整的时间范围 (TimeRange)。
   * @param dir 时间转换的方向 (Node::TransformTimeDirection)。
   * @return 调整后的时间范围 (TimeRange)。
   */
  TimeRange GetAdjustedTime(Node* from, Node* to, const TimeRange& r, Node::TransformTimeDirection dir) const;

  // int GetNumberOfPathAdjustments(Node* from, NodeParam::Type direction) const; // 原始注释掉的代码行，保持原样

 protected:  // 受保护的虚函数，用于派生类处理时间目标变化事件
  /**
   * @brief 当时间目标断开连接时调用的虚函数。
   *
   * 派生类可以重写此方法以在时间目标被移除或设置为 nullptr 时执行特定操作。
   * @param target 指向被断开连接的 ViewerOutput 对象的指针。默认为空实现。
   */
  virtual void TimeTargetDisconnectEvent(ViewerOutput*) {}
  /**
   * @brief 当时间目标发生变化时调用的虚函数。
   *
   * 派生类可以重写此方法以在时间目标被更改为新的非空 ViewerOutput 时执行特定操作。
   * @param target 指向新的 ViewerOutput 时间目标的指针。默认为空实现。
   */
  virtual void TimeTargetChangedEvent(ViewerOutput*) {}
  /**
   * @brief 当时间目标首次连接（从 nullptr 变为非 nullptr）时调用的虚函数。
   *
   * 派生类可以重写此方法以在时间目标被设置时执行特定操作。
   * @param target 指向新连接的 ViewerOutput 时间目标的指针。默认为空实现。
   */
  virtual void TimeTargetConnectEvent(ViewerOutput*) {}

 private:                      // 私有成员变量
  ViewerOutput* time_target_;  ///< 指向当前时间目标 ViewerOutput 节点的指针。

  int path_index_;  ///< 存储路径索引，可能用于多路径时间转换。
};

}  // namespace olive

#endif  // TIMETARGETOBJECT_H
