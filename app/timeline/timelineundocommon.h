#ifndef TIMELINEUNDOCOMMON_H
#define TIMELINEUNDOCOMMON_H

#include "node/node.h"         // 引入节点类的定义 (Node)
#include "node/nodeundo.h"     // 引入节点撤销/重做命令类的定义 (例如 NodeRemoveWithExclusiveDependenciesAndDisconnect)

namespace olive {

/**
 * @brief 检查一个节点是否可以被移除。
 *
 * 通常，一个节点只有在没有输出连接（即没有其他节点依赖于它的输出）时才能被安全移除。
 * @param n 指向要检查的 Node 对象的指针。
 * @return 如果节点的输出连接为空，则返回 true，表示可以移除；否则返回 false。
 */
inline bool NodeCanBeRemoved(Node* n) { return n->output_connections().empty(); }

/**
 * @brief 创建一个用于移除节点的撤销命令。
 *
 * 此函数创建一个特定类型的撤销命令 (NodeRemoveWithExclusiveDependenciesAndDisconnect)，
 * 该命令不仅会移除节点本身，还会处理其独占的依赖关系并断开连接。
 * @param n 指向要为其创建移除命令的 Node 对象的指针。
 * @return 返回一个指向新创建的 UndoCommand 对象的指针。调用者负责管理此对象的生命周期或将其添加到撤销栈。
 */
inline UndoCommand* CreateRemoveCommand(Node* n) { return new NodeRemoveWithExclusiveDependenciesAndDisconnect(n); }

/**
 * @brief 创建并立即执行（重做）一个用于移除节点的撤销命令。
 *
 * 这个辅助函数首先调用 CreateRemoveCommand 来创建一个移除节点的命令，
 * 然后立即调用该命令的 redo_now() 方法来执行移除操作。
 * 这通常用于用户直接执行删除操作，同时希望该操作能够被撤销的场景。
 * @param n 指向要移除的 Node 对象的指针。
 * @return 返回一个指向已执行（重做）的 UndoCommand 对象的指针。
 * 调用者通常会将此命令添加到撤销栈中。
 */
inline UndoCommand* CreateAndRunRemoveCommand(Node* n) {
  UndoCommand* command = CreateRemoveCommand(n); // 创建移除命令
  command->redo_now();                           // 立即执行（重做）该命令
  return command;                                // 返回命令对象
}

}  // namespace olive

#endif  // TIMELINEUNDOCOMMON_H
