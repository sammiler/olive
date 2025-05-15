

#ifndef MAINWINDOWUNDO_H
#define MAINWINDOWUNDO_H

#include "node/project/sequence/sequence.h"

namespace olive {

class OpenSequenceCommand : public UndoCommand {
 public:
  explicit OpenSequenceCommand(Sequence* sequence) : sequence_(sequence) {}

  [[nodiscard]] Project* GetRelevantProject() const override { return nullptr; }

 protected:
  void redo() override;

  void undo() override;

 private:
  Sequence* sequence_;
};

class CloseSequenceCommand : public UndoCommand {
 public:
  explicit CloseSequenceCommand(Sequence* sequence) : sequence_(sequence) {}

  [[nodiscard]] Project* GetRelevantProject() const override { return nullptr; }

 protected:
  void redo() override;

  void undo() override;

 private:
  Sequence* sequence_;
};

}  // namespace olive

#endif  // MAINWINDOWUNDO_H
