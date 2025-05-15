#include "mainwindowundo.h"

#include "core.h"
#include "window/mainwindow/mainwindow.h"

namespace olive {

void OpenSequenceCommand::redo() { Core::instance()->main_window()->OpenSequence(sequence_); }

void OpenSequenceCommand::undo() { Core::instance()->main_window()->CloseSequence(sequence_); }

void CloseSequenceCommand::redo() { Core::instance()->main_window()->CloseSequence(sequence_); }

void CloseSequenceCommand::undo() { Core::instance()->main_window()->OpenSequence(sequence_); }

}  // namespace olive
