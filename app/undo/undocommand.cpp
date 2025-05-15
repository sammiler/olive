

#include "undocommand.h"

#include <ranges>

#include "core.h"

namespace olive {

void MultiUndoCommand::redo() {
  for (auto it : children_) {
    it->redo_and_set_modified();
  }
}

void MultiUndoCommand::undo() {
  for (auto it : std::ranges::reverse_view(children_)) {
    it->undo_and_set_modified();
  }
}

UndoCommand::UndoCommand() {
  prepared_ = false;
  done_ = false;
}

void UndoCommand::redo_and_set_modified() {
  project_ = GetRelevantProject();

  redo_now();

  if (project_) {
    modified_ = project_->is_modified();
    project_->set_modified(true);
  }
}

void UndoCommand::undo_and_set_modified() {
  undo_now();

  if (project_) {
    project_->set_modified(modified_);
  }
}

void UndoCommand::redo_now() {
  if (!done_) {
    if (!prepared_) {
      prepare();
      prepared_ = true;
    }

    redo();
    done_ = true;
  }
}

void UndoCommand::undo_now() {
  if (done_) {
    undo();
    done_ = false;
  }
}

}  // namespace olive
