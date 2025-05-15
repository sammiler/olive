#ifndef GAPBLOCK_H
#define GAPBLOCK_H

#include "node/block/block.h" // 引入基类 Block 的定义

namespace olive {

/**
 * @brief 代表在其对应轨道中一段持续时间内“无内容”的节点。
 * 这通常用于在时间线上创建空白区域或间隙。
 */
class GapBlock : public Block {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief GapBlock 构造函数。
   */
  GapBlock();

  NODE_DEFAULT_FUNCTIONS(GapBlock) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此 GapBlock 的名称。
   * @return QString 类型的节点名称（例如，可能会返回 "间隙" 或 "Gap"）。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此 GapBlock 的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此 GapBlock 的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;
};

}  // namespace olive

#endif  // GAPBLOCK_H // 原为 TIMELINEBLOCK_H，根据上下文推断应为 GAPBLOCK_H