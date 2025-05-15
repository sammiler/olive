#ifndef TIMELINEUNDOCOMMON_H
#define TIMELINEUNDOCOMMON_H

#include "node/node.h"
#include "node/nodeundo.h"

namespace olive {

inline bool NodeCanBeRemoved(Node* n) { return n->output_connections().empty(); }

inline UndoCommand* CreateRemoveCommand(Node* n) { return new NodeRemoveWithExclusiveDependenciesAndDisconnect(n); }

inline UndoCommand* CreateAndRunRemoveCommand(Node* n) {
  UndoCommand* command = CreateRemoveCommand(n);
  command->redo_now();
  return command;
}

}  // namespace olive

#endif  // TIMELINEUNDOCOMMON_H
