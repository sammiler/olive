#ifndef UNDOCOMMAND_H
#define UNDOCOMMAND_H

#include <QString>
#include <list>
#include <vector>

#include "common/define.h"

namespace olive {

class Project;

class UndoCommand {
 public:
  UndoCommand();

  virtual ~UndoCommand() = default;

  DISABLE_COPY_MOVE(UndoCommand)

  [[nodiscard]] bool has_prepared() const { return prepared_; }
  void set_prepared(bool e) { prepared_ = true; }

  void redo_now();
  void undo_now();

  void redo_and_set_modified();
  void undo_and_set_modified();

  [[nodiscard]] virtual Project* GetRelevantProject() const = 0;

 protected:
  virtual void prepare() {}
  virtual void redo() = 0;
  virtual void undo() = 0;

 private:
  bool modified_{};

  Project* project_{};

  bool prepared_;

  bool done_;
};

class MultiUndoCommand : public UndoCommand {
 public:
  MultiUndoCommand() = default;

  [[nodiscard]] Project* GetRelevantProject() const override { return nullptr; }

  void add_child(UndoCommand* command) { children_.push_back(command); }

  [[nodiscard]] int child_count() const { return children_.size(); }

  [[nodiscard]] UndoCommand* child(int i) const { return children_[i]; }

 protected:
  void redo() override;
  void undo() override;

 private:
  std::vector<UndoCommand*> children_;
};

}  // namespace olive

#endif  // UNDOCOMMAND_H
