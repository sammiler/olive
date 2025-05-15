

#ifndef TIMELINEUNDOSPLIT_H
#define TIMELINEUNDOSPLIT_H

#include "node/output/track/track.h"

namespace olive {

class BlockSplitCommand : public UndoCommand {
 public:
  BlockSplitCommand(Block* block, rational point)
      : block_(block), new_block_(nullptr), point_(point), reconnect_tree_command_(nullptr) {}

  ~BlockSplitCommand() override { delete reconnect_tree_command_; }

  [[nodiscard]] Project* GetRelevantProject() const override { return block_->project(); }

  /**
   * @brief Access the second block created as a result. Only valid after redo().
   */
  Block* new_block() { return new_block_; }

 protected:
  void prepare() override;

  void redo() override;

  void undo() override;

 private:
  Block* block_;
  Block* new_block_;

  rational old_length_;
  rational point_;

  MultiUndoCommand* reconnect_tree_command_;

  NodeInput moved_transition_;
};

class BlockSplitPreservingLinksCommand : public UndoCommand {
 public:
  BlockSplitPreservingLinksCommand(const QVector<Block*>& blocks, const QList<rational>& times)
      : blocks_(blocks), times_(times) {}

  ~BlockSplitPreservingLinksCommand() override { qDeleteAll(commands_); }

  [[nodiscard]] Project* GetRelevantProject() const override { return blocks_.first()->project(); }

  Block* GetSplit(Block* original, int time_index) const;

 protected:
  void prepare() override;

  void redo() override {
    for (auto command : commands_) {
      command->redo_now();
    }
  }

  void undo() override {
    for (int i = commands_.size() - 1; i >= 0; i--) {
      commands_.at(i)->undo_now();
    }
  }

 private:
  QVector<Block*> blocks_;

  QList<rational> times_;

  QVector<UndoCommand*> commands_;

  QVector<QVector<Block*> > splits_;
};

class TrackSplitAtTimeCommand : public UndoCommand {
 public:
  TrackSplitAtTimeCommand(Track* track, rational point) : track_(track), point_(point), command_(nullptr) {}

  ~TrackSplitAtTimeCommand() override { delete command_; }

  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  void prepare() override;

  void redo() override {
    if (command_) {
      command_->redo_now();
    }
  }

  void undo() override {
    if (command_) {
      command_->undo_now();
    }
  }

 private:
  Track* track_;

  rational point_;

  UndoCommand* command_;
};

}  // namespace olive

#endif  // TIMELINEUNDOSPLIT_H
