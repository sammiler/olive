#ifndef TIMELINEUNDOTRACK_H
#define TIMELINEUNDOTRACK_H

#include "node/output/track/track.h"  // 引入 Track 类的定义，此类中的命令主要操作轨道及其内容

namespace olive {

/**
 * @brief 从轨道上涟漪式移除一个区块（Block）的撤销命令。
 *
 * 当一个区块被涟漪式移除时，其后的所有区块会向前移动以填补空隙。
 */
class TrackRippleRemoveBlockCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param track 目标轨道。
   * @param block 要移除的区块。
   */
  TrackRippleRemoveBlockCommand(Track* track, Block* block) : track_(track), block_(block) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回轨道所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  /**
   * @brief 执行涟漪式移除区块的操作。
   *
   * 在移除前记录该区块的前一个区块，以便撤销时能够正确恢复位置。
   */
  void redo() override {
    before_ = block_->previous();       // 记录被移除区块的前一个区块
    track_->RippleRemoveBlock(block_);  // 在轨道上执行涟漪式移除
  }

  /**
   * @brief 撤销涟漪式移除区块的操作。
   *
   * 将之前移除的区块重新插入到其原始位置。
   */
  void undo() override { track_->InsertBlockAfter(block_, before_); }

 private:
  Track* track_;     // 指向目标轨道
  Block* block_;     // 指向要被移除的区块
  Block* before_{};  // 指向被移除区块之前的一个区块，用于撤销时定位
};

/**
 * @brief 在轨道开头前置（Prepend）一个区块的撤销命令。
 */
class TrackPrependBlockCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param track 目标轨道。
   * @param block 要前置的区块。
   */
  TrackPrependBlockCommand(Track* track, Block* block) : track_(track), block_(block) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回轨道所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  /**
   * @brief 执行在前置区块的操作。
   */
  void redo() override { track_->PrependBlock(block_); }

  /**
   * @brief 撤销前置区块的操作。
   *
   * 撤销时，通常执行涟漪式移除以保持后续区块的连贯性。
   */
  void undo() override { track_->RippleRemoveBlock(block_); }

 private:
  Track* track_;  // 指向目标轨道
  Block* block_;  // 指向要前置的区块
};

/**
 * @brief 在轨道上指定区块之后插入一个新区块的撤销命令。
 */
class TrackInsertBlockAfterCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param track 目标轨道。
   * @param block 要插入的新区块。
   * @param before 新区块将被插入到此区块之后；如果为 nullptr，则插入到轨道开头。
   */
  TrackInsertBlockAfterCommand(Track* track, Block* block, Block* before)
      : track_(track), block_(block), before_(before) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回被插入区块所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return block_->project(); }

 protected:
  /**
   * @brief 执行插入区块的操作。
   */
  void redo() override { track_->InsertBlockAfter(block_, before_); }

  /**
   * @brief 撤销插入区块的操作。
   *
   * 撤销时，通常执行涟漪式移除以保持后续区块的连贯性。
   */
  void undo() override { track_->RippleRemoveBlock(block_); }

 private:
  Track* track_;   // 指向目标轨道
  Block* block_;   // 指向要插入的区块
  Block* before_;  // 指向新区块插入位置的前一个区块
};

/**
 * @brief 将轨道上的一个旧区块替换为一个新区块的撤销命令。
 *
 * 两个区块的长度必须相等。
 */
class TrackReplaceBlockCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param track 目标轨道。
   * @param old 要被替换的旧区块。
   * @param replace 用于替换的新区块。
   */
  TrackReplaceBlockCommand(Track* track, Block* old, Block* replace) : track_(track), old_(old), replace_(replace) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回轨道所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  /**
   * @brief 执行替换操作，用 `replace_` 替换 `old_`。
   */
  void redo() override { track_->ReplaceBlock(old_, replace_); }

  /**
   * @brief 撤销替换操作，用 `old_` 替换 `replace_`。
   */
  void undo() override { track_->ReplaceBlock(replace_, old_); }

 private:
  Track* track_;    // 指向目标轨道
  Block* old_;      // 指向被替换的旧区块
  Block* replace_;  // 指向用于替换的新区块
};

}  // namespace olive

#endif  // TIMELINEUNDOTRACK_H
