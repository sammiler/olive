#ifndef NODEVIEWCOMMON_H
#define NODEVIEWCOMMON_H

#include <QtGlobal> // 包含 Qt 全局定义，例如 Qt::Orientation

#include "common/define.h" // 项目通用定义

namespace olive {

/**
 * @brief NodeViewCommon 类提供节点视图相关的通用枚举和静态辅助函数。
 *
 * 这个类本身不被实例化，它主要作为命名空间来组织与节点视图布局流向相关的常量和工具函数。
 */
class NodeViewCommon {
 public:
  /**
   * @brief FlowDirection 枚举定义了节点图中节点连接的流向（布局方向）。
   */
  enum FlowDirection {
    kInvalidDirection = -1, ///< 无效或未定义的流向。
    kTopToBottom,           ///< 从上到下。
    kBottomToTop,           ///< 从下到上。
    kLeftToRight,           ///< 从左到右。
    kRightToLeft            ///< 从右到左。
  };

  /**
   * @brief 根据给定的流向获取其主要的 Qt::Orientation (水平或垂直)。
   * @param dir FlowDirection 枚举值。
   * @return 如果流向是垂直的 (kTopToBottom, kBottomToTop)，则返回 Qt::Vertical；
   * 否则（对于水平流向 kLeftToRight, kRightToLeft），返回 Qt::Horizontal。
   * 对于 kInvalidDirection，行为是返回 Qt::Horizontal（基于 else 分支）。
   */
  static Qt::Orientation GetFlowOrientation(FlowDirection dir) {
    if (dir == kTopToBottom || dir == kBottomToTop) { // 检查是否为垂直流向
      return Qt::Vertical; // 返回垂直方向
    } else {
      return Qt::Horizontal; // 否则返回水平方向
    }
  }

  /**
   * @brief 检查给定的流向是否为垂直方向。
   * @param dir FlowDirection 枚举值。
   * @return 如果流向是 kTopToBottom 或 kBottomToTop，则返回 true；否则返回 false。
   */
  static bool IsFlowVertical(FlowDirection dir) { return dir == kTopToBottom || dir == kBottomToTop; }

  /**
   * @brief 检查给定的流向是否为水平方向。
   * @param dir FlowDirection 枚举值。
   * @return 如果流向是 kLeftToRight 或 kRightToLeft，则返回 true；否则返回 false。
   */
  static bool IsFlowHorizontal(FlowDirection dir) { return dir == kLeftToRight || dir == kRightToLeft; }

  /**
   * @brief 检查两个给定的流向是否互为相反方向。
   * @param a 第一个 FlowDirection 枚举值。
   * @param b 第二个 FlowDirection 枚举值。
   * @return 如果两个方向相反（例如，左到右与右到左，或上到下与下到上），则返回 true；否则返回 false。
   */
  static bool DirectionsAreOpposing(FlowDirection a, FlowDirection b) {
    return ((a == NodeViewCommon::kLeftToRight && b == NodeViewCommon::kRightToLeft) ||   // 左->右 与 右->左
            (a == NodeViewCommon::kRightToLeft && b == NodeViewCommon::kLeftToRight) ||   // 右->左 与 左->右
            (a == NodeViewCommon::kTopToBottom && b == NodeViewCommon::kBottomToTop) ||   // 上->下 与 下->上
            (a == NodeViewCommon::kBottomToTop && b == NodeViewCommon::kTopToBottom));  // 下->上 与 上->下
  }
};

}  // namespace olive

#endif  // NODEVIEWCOMMON_H
