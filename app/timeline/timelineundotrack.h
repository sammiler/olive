

#ifndef TIMELINEUNDOTRACK_H
#define TIMELINEUNDOTRACK_H

#include "node/output/track/track.h"

namespace olive {

class TrackRippleRemoveBlockCommand : public UndoCommand {
 public:
  TrackRippleRemoveBlockCommand(Track* track, Block* block) : track_(track), block_(block) {}

  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  void redo() override {
    before_ = block_->previous();
    track_->RippleRemoveBlock(block_);
  }

  void undo() override { track_->InsertBlockAfter(block_, before_); }

 private:
  Track* track_;

  Block* block_;

  Block* before_{};
};

class TrackPrependBlockCommand : public UndoCommand {
 public:
  TrackPrependBlockCommand(Track* track, Block* block) : track_(track), block_(block) {}

  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  void redo() override { track_->PrependBlock(block_); }

  void undo() override { track_->RippleRemoveBlock(block_); }

 private:
  Track* track_;
  Block* block_;
};

class TrackInsertBlockAfterCommand : public UndoCommand {
 public:
  TrackInsertBlockAfterCommand(Track* track, Block* block, Block* before)
      : track_(track), block_(block), before_(before) {}

  [[nodiscard]] Project* GetRelevantProject() const override { return block_->project(); }

 protected:
  void redo() override { track_->InsertBlockAfter(block_, before_); }

  void undo() override { track_->RippleRemoveBlock(block_); }

 private:
  Track* track_;

  Block* block_;

  Block* before_;
};

/**
 * @brief Replaces Block `old` with Block `replace`
 *
 * Both blocks must have equal lengths.
 */
class TrackReplaceBlockCommand : public UndoCommand {
 public:
  TrackReplaceBlockCommand(Track* track, Block* old, Block* replace) : track_(track), old_(old), replace_(replace) {}

  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  void redo() override { track_->ReplaceBlock(old_, replace_); }

  void undo() override { track_->ReplaceBlock(replace_, old_); }

 private:
  Track* track_;
  Block* old_;
  Block* replace_;
};

}  // namespace olive

#endif  // TIMELINEUNDOTRACK_H
