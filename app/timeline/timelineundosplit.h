#ifndef TIMELINEUNDOSPLIT_H
#define TIMELINEUNDOSPLIT_H

#include "node/output/track/track.h" // 引入 Track 类的定义，分割操作通常在轨道上进行

namespace olive {

/**
 * @brief 在指定点分割一个区块（Block）的撤销命令。
 *
 * 当一个区块被分割时，它会被缩短到分割点，并创建一个新的区块来表示分割点之后的部分。
 * 此命令负责处理这个过程以及其撤销/重做。
 */
class BlockSplitCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param block 指向要被分割的 Block 对象的指针。
   * @param point 分割点的时间（相对于区块的起始时间，或者是一个绝对时间轴时间，具体取决于实现）。
   */
  BlockSplitCommand(Block* block, rational point)
      : block_(block), new_block_(nullptr), point_(point), reconnect_tree_command_(nullptr) {}

  /**
   * @brief 析构函数。
   *
   * 负责清理 reconnect_tree_command_（如果已创建）。
   */
  ~BlockSplitCommand() override { delete reconnect_tree_command_; }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 Block 所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return block_->project(); }

  /**
   * @brief 获取分割后创建的第二个区块。
   *
   * 只有在 redo() 执行后才有效。
   * @return 返回指向新创建的 Block 对象的指针。
   */
  Block* new_block() { return new_block_; }

 protected:
  /**
   * @brief 命令准备阶段，在首次 redo() 或 undo() 前调用。
   *
   * 通常用于保存命令执行所需的状态，例如原始区块的长度。
   */
  void prepare() override;

  /**
   * @brief 执行分割区块的操作。
   */
  void redo() override;

  /**
   * @brief 撤销分割区块的操作。
   */
  void undo() override;

 private:
  Block* block_;        // 指向被分割的原始区块
  Block* new_block_;    // 指向分割后产生的新区块

  rational old_length_; // 原始区块的长度
  rational point_;      // 分割点

  MultiUndoCommand* reconnect_tree_command_; // 可能用于处理分割后节点树连接关系的复合撤销命令

  NodeInput moved_transition_; // 如果分割点处有转场，可能需要移动或处理该转场的输入
};

/**
 * @brief 分割一组区块并保留其链接（例如节点图中的连接）的撤销命令。
 *
 * 这个命令允许在多个区块的多个时间点上执行分割操作，同时尝试维护
 * 分割产生的区块之间的逻辑连接关系。
 */
class BlockSplitPreservingLinksCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param blocks 要分割的 Block 对象列表。
   * @param times 每个区块对应的分割时间点列表。
   */
  BlockSplitPreservingLinksCommand(const QVector<Block*>& blocks, const QList<rational>& times)
      : blocks_(blocks), times_(times) {}

  /**
   * @brief 析构函数。
   *
   * 负责清理 commands_ 向量中的所有命令对象。
   */
  ~BlockSplitPreservingLinksCommand() override { qDeleteAll(commands_); }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 如果区块列表不为空，返回第一个区块所属的 Project 指针；否则返回 nullptr。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return blocks_.first()->project(); }

  /**
   * @brief 获取特定原始区块在特定时间索引处分割后产生的新区块。
   * @param original 指向原始被分割的区块。
   * @param time_index 分割时间的索引（如果一个区块在多个时间点被分割）。
   * @return 返回分割后对应的新 Block 指针，如果未找到则返回 nullptr。
   */
  Block* GetSplit(Block* original, int time_index) const;

 protected:
  /**
   * @brief 命令准备阶段。
   *
   * 在此阶段，可能会为每个实际的分割操作创建单独的 BlockSplitCommand 子命令。
   */
  void prepare() override;

  /**
   * @brief 执行所有分割操作。
   */
  void redo() override {
    for (auto command : commands_) { // 遍历所有子命令
      command->redo_now();           // 执行子命令的 redo
    }
  }

  /**
   * @brief 撤销所有分割操作。
   */
  void undo() override {
    for (int i = commands_.size() - 1; i >= 0; i--) { // 反向遍历子命令
      commands_.at(i)->undo_now();                   // 执行子命令的 undo
    }
  }

 private:
  QVector<Block*> blocks_; // 要被分割的区块列表

  QList<rational> times_; // 对应的分割时间点列表

  QVector<UndoCommand*> commands_; // 存储实际执行分割的子命令（可能是 BlockSplitCommand 实例）

  QVector<QVector<Block*> > splits_; // 可能用于存储每个原始区块分割后产生的所有新区块片段
};

/**
 * @brief 在轨道的指定时间点分割区块的撤销命令。
 *
 * 此命令会找到在指定时间点跨越的区块（如果有的话），并对其执行分割操作。
 */
class TrackSplitAtTimeCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param track 目标轨道。
   * @param point 分割的时间点。
   */
  TrackSplitAtTimeCommand(Track* track, rational point) : track_(track), point_(point), command_(nullptr) {}

  /**
   * @brief 析构函数。
   *
   * 负责清理 command_（如果已创建）。
   */
  ~TrackSplitAtTimeCommand() override { delete command_; }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回轨道所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  /**
   * @brief 命令准备阶段。
   *
   * 查找在 `point_` 时间点上的区块，并创建相应的 `BlockSplitCommand`。
   */
  void prepare() override;

  /**
   * @brief 执行分割操作。
   */
  void redo() override {
    if (command_) { // 如果存在实际的分割子命令
      command_->redo_now(); // 执行它
    }
  }

  /**
   * @brief 撤销分割操作。
   */
  void undo() override {
    if (command_) { // 如果存在实际的分割子命令
      command_->undo_now(); // 撤销它
    }
  }

 private:
  Track* track_; // 目标轨道

  rational point_; // 分割的时间点

  UndoCommand* command_; // 指向实际执行分割的 BlockSplitCommand（或类似命令）的指针
};

}  // namespace olive

#endif  // TIMELINEUNDOSPLIT_H
