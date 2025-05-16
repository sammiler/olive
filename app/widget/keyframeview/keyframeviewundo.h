#ifndef KEYFRAMEVIEWUNDO_H
#define KEYFRAMEVIEWUNDO_H

#include "node/keyframe.h"     // 关键帧数据结构和类型定义
#include "undo/undocommand.h"  // 撤销命令基类

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QPointF; // Qt 浮点数二维点类 (已在 node/keyframe.h 中通过 NodeKeyframe::BezierType 间接使用或定义)

namespace olive {

// 前向声明项目内部类 (根据用户要求，不添加)
// class Project; // 项目类 (UndoCommand 中可能需要)

/**
 * @brief KeyframeSetTypeCommand 类是一个撤销/重做命令，用于更改关键帧的插值类型。
 *
 * 当用户在关键帧视图中修改了某个关键帧的插值方式（例如从线性变为贝塞尔），
 * 此命令会被创建并推入撤销栈，以便能够撤销或重做该类型更改操作。
 */
class KeyframeSetTypeCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param key 指向要修改类型的 NodeKeyframe 对象的指针。
   * @param type 要设置的新的关键帧插值类型 (NodeKeyframe::Type)。
   */
  KeyframeSetTypeCommand(NodeKeyframe* key, NodeKeyframe::Type type);

  /**
   * @brief 获取与此命令相关的项目。
   *
   * 通常返回此关键帧所属节点所在的项目。
   * @return 指向 Project 对象的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行重做操作。
   *
   * 将关键帧的类型设置为新的类型 (new_type_)。
   */
  void redo() override;
  /**
   * @brief 执行撤销操作。
   *
   * 将关键帧的类型恢复为旧的类型 (old_type_)。
   */
  void undo() override;

 private:
  NodeKeyframe* key_;  ///< 指向被操作的关键帧的指针。

  NodeKeyframe::Type old_type_;  ///< 关键帧原始的插值类型。
  NodeKeyframe::Type new_type_;  ///< 关键帧要设置的新的插值类型。
};

/**
 * @brief KeyframeSetBezierControlPoint 类是一个撤销/重做命令，用于更改关键帧的贝塞尔控制点位置。
 *
 * 当用户在关键帧视图中拖动贝塞尔曲线的控制手柄时，此命令会被创建并推入撤销栈，
 * 以便能够撤销或重做对控制点位置的修改。
 */
class KeyframeSetBezierControlPoint : public UndoCommand {
 public:
  /**
   * @brief 构造函数（版本1）。
   *
   * 通常在此构造函数中，旧的控制点位置会从关键帧对象内部获取。
   * @param key 指向要修改其贝塞尔控制点的 NodeKeyframe 对象的指针。
   * @param mode 要修改的贝塞尔控制点的类型 (NodeKeyframe::BezierType)，例如入控制柄或出控制柄。
   * @param point 控制点的新位置 (QPointF)。
   */
  KeyframeSetBezierControlPoint(NodeKeyframe* key, NodeKeyframe::BezierType mode, const QPointF& point);
  /**
   * @brief 构造函数（版本2）。
   *
   * 此版本允许显式传入旧的控制点位置，可能用于更复杂的场景或在旧值不易直接获取时使用。
   * @param key 指向要修改其贝塞尔控制点的 NodeKeyframe 对象的指针。
   * @param mode 要修改的贝塞尔控制点的类型 (NodeKeyframe::BezierType)。
   * @param new_point 控制点的新位置 (QPointF)。
   * @param old_point 控制点的旧位置 (QPointF)。
   */
  KeyframeSetBezierControlPoint(NodeKeyframe* key, NodeKeyframe::BezierType mode, const QPointF& new_point,
                                const QPointF& old_point);

  /**
   * @brief 获取与此命令相关的项目。
   *
   * 通常返回此关键帧所属节点所在的项目。
   * @return 指向 Project 对象的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行重做操作。
   *
   * 将指定模式的贝塞尔控制点位置设置为新位置 (new_point_)。
   */
  void redo() override;
  /**
   * @brief 执行撤销操作。
   *
   * 将指定模式的贝塞尔控制点位置恢复为旧位置 (old_point_)。
   */
  void undo() override;

 private:
  NodeKeyframe* key_;  ///< 指向被操作的关键帧的指针。

  NodeKeyframe::BezierType mode_;  ///< 指示要修改的是哪个贝塞尔控制点（例如，入点、出点）。

  QPointF old_point_;  ///< 控制点原始的位置。
  QPointF new_point_;  ///< 控制点新的位置。
};

}  // namespace olive

#endif  // KEYFRAMEVIEWUNDO_H
