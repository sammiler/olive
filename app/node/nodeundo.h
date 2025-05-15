#ifndef NODEUNDO_H  // 防止头文件被重复包含的宏
#define NODEUNDO_H  // 定义 NODEUNDO_H 宏

#include <utility>  // 标准库 utility 头文件，提供 std::move, std::pair 等

#include "node/node.h"         // 节点基类定义
#include "node/project.h"      // 项目类定义
#include "undo/undocommand.h"  // 撤销命令基类定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief 设置节点在特定上下文中的位置的撤销命令。
 */
class NodeSetPositionCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 要设置位置的节点。
   * @param context 节点所在的上下文 (通常是父节点或图)。
   * @param pos 新的位置信息 (坐标和展开状态)。
   */
  NodeSetPositionCommand(Node* node, Node* context, const Node::Position& pos) {
    node_ = node;        // 目标节点
    context_ = context;  // 目标上下文
    pos_ = pos;          // 新的位置
  }

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return node_->project(); }

 protected:
  /**
   * @brief 执行命令 (重做)。
   */
  void redo() override;

  /**
   * @brief 撤销命令。
   */
  void undo() override;

 private:
  Node* node_;              // 被操作的节点
  Node* context_;           // 节点所在的上下文
  Node::Position pos_;      // 节点的新位置
  Node::Position old_pos_;  // 节点的旧位置 (用于撤销)
  bool added_{};            // 标记在执行 redo 时，节点是否是新添加到上下文的 (用于撤销时正确移除或恢复)
};

/**
 * @brief 递归设置节点及其依赖项在特定上下文中的位置的撤销命令。
 */
class NodeSetPositionAndDependenciesRecursivelyCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 要设置位置的根节点。
   * @param context 节点所在的上下文。
   * @param pos 根节点的新位置信息。
   */
  NodeSetPositionAndDependenciesRecursivelyCommand(Node* node, Node* context, const Node::Position& pos)
      : node_(node), context_(context), pos_(pos) {}

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回根节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return node_->project(); }

 protected:
  /**
   * @brief 命令执行前的准备工作 (例如，收集旧位置)。
   */
  void prepare() override;

  /**
   * @brief 执行命令 (重做)。
   */
  void redo() override;

  /**
   * @brief 撤销命令。
   */
  void undo() override;

 private:
  /**
   * @brief 递归移动节点及其依赖。
   * @param node 当前要移动的节点。
   * @param diff 位置变化的差值。
   */
  void move_recursively(Node* node, const QPointF& diff);

  Node* node_;                      // 被操作的根节点
  Node* context_;                   // 节点所在的上下文
  Node::Position pos_;              // 根节点的新位置
  QVector<UndoCommand*> commands_;  // 存储所有子移动命令的列表 (用于原子化操作)
};

/**
 * @brief 从特定上下文中移除节点位置信息的撤销命令。
 */
class NodeRemovePositionFromContextCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 要移除位置信息的节点。
   * @param context 节点所在的上下文。
   */
  NodeRemovePositionFromContextCommand(Node* node, Node* context) : node_(node), context_(context) {}

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return node_->project(); }

 protected:
  /**
   * @brief 执行命令 (重做)。
   */
  void redo() override;

  /**
   * @brief 撤销命令。
   */
  void undo() override;

 private:
  Node* node_;              // 被操作的节点
  Node* context_;           // 节点所在的上下文
  Node::Position old_pos_;  // 节点被移除前的旧位置 (用于撤销)
  bool contained_{};        // 标记在执行 redo 前，上下文是否确实包含此节点的位置信息
};

/**
 * @brief 从所有上下文中移除节点位置信息的撤销命令。
 */
class NodeRemovePositionFromAllContextsCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 要移除位置信息的节点。
   */
  explicit NodeRemovePositionFromAllContextsCommand(Node* node) : node_(node) {}

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return node_->project(); }

 protected:
  /**
   * @brief 执行命令 (重做)。
   */
  void redo() override;

  /**
   * @brief 撤销命令。
   */
  void undo() override;

 private:
  Node* node_;                         // 被操作的节点
  std::map<Node*, QPointF> contexts_;  // 存储节点在被移除前存在于哪些上下文及其位置 (用于撤销)
};

/**
 * @brief 节点输入数组插入元素的撤销命令。
 */
class NodeArrayInsertCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 目标节点。
   * @param input 目标输入端口的ID。
   * @param index 要插入元素的位置索引。
   */
  NodeArrayInsertCommand(Node* node, QString input, int index) : node_(node), input_(std::move(input)), index_(index) {}

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：在数组指定位置插入元素。
   */
  void redo() override { node_->InputArrayInsert(input_, index_); }

  /**
   * @brief 撤销命令：从数组指定位置移除元素。
   */
  void undo() override { node_->InputArrayRemove(input_, index_); }

 private:
  Node* node_;     // 目标节点
  QString input_;  // 目标输入端口的ID (数组类型)
  int index_;      // 插入/移除的索引
};

/**
 * @brief 调整节点输入数组大小的撤销命令。
 */
class NodeArrayResizeCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 目标节点。
   * @param input 目标输入端口的ID。
   * @param size 新的数组大小。
   */
  NodeArrayResizeCommand(Node* node, QString input, int size) : node_(node), input_(std::move(input)), size_(size) {}

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：调整数组大小。
   */
  void redo() override {
    old_size_ = node_->InputArraySize(input_);  // 记录旧大小

    if (old_size_ > size_) {  // 如果是缩小尺寸
      // 断开多余元素的连接
      for (int i = size_; i < old_size_; i++) {
        try {
          NodeInput input(node_, input_, i);                    // 创建 NodeInput 对象
          Node* output = node_->input_connections().at(input);  // 获取连接到此输入的上游节点

          removed_connections_[input] = output;  // 保存被移除的连接以备撤销

          Node::DisconnectEdge(output, input);  // 断开连接
        } catch (std::out_of_range&) {
          // 如果指定元素没有连接，会抛出 out_of_range，这里忽略
        }
      }
    }

    node_->ArrayResizeInternal(input_, size_);  // 实际调整数组大小
  }

  /**
   * @brief 撤销命令：恢复数组到旧大小，并重新连接之前断开的边。
   */
  void undo() override {
    // 重新连接之前因缩小尺寸而断开的连接
    for (const auto& removed_connection : removed_connections_) {
      Node::ConnectEdge(removed_connection.second, removed_connection.first);
    }
    removed_connections_.clear();  // 清空已恢复的连接记录

    node_->ArrayResizeInternal(input_, old_size_);  // 恢复到旧的数组大小
  }

 private:
  Node* node_;      // 目标节点
  QString input_;   // 目标输入端口的ID
  int size_;        // 新的数组大小
  int old_size_{};  // 旧的数组大小 (用于撤销)

  Node::InputConnections removed_connections_;  // 存储因数组缩小而被移除的连接
};

/**
 * @brief 从节点输入数组中移除元素的撤销命令。
 */
class NodeArrayRemoveCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 目标节点。
   * @param input 目标输入端口的ID。
   * @param index 要移除元素的索引。
   */
  NodeArrayRemoveCommand(Node* node, QString input, int index) : node_(node), input_(std::move(input)), index_(index) {}

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：移除数组元素，并保存其状态 (值、关键帧)。
   */
  void redo() override {
    // 保存被移除元素的立即值数据
    if (node_->IsInputKeyframable(input_)) {                      // 如果该输入可设置关键帧
      is_keyframing_ = node_->IsInputKeyframing(input_, index_);  // 保存是否启用关键帧的状态
    }
    standard_value_ = node_->GetSplitStandardValue(input_, index_);  // 保存标准值
    keyframes_ = node_->GetKeyframeTracks(input_, index_);           // 保存所有关键帧轨道
    // 从 NodeInputImmediate 对象中删除所有关键帧，并将它们的所有权转移给 memory_manager_
    // 这样当 memory_manager_ 析构时，这些 keyframe 会被删除 (如果undo没有恢复它们)
    node_->GetImmediate(input_, index_)->delete_all_keyframes(&memory_manager_);

    node_->InputArrayRemove(input_, index_);  // 实际移除数组元素
  }

  /**
   * @brief 撤销命令：重新插入数组元素，并恢复其状态。
   */
  void undo() override {
    node_->InputArrayInsert(input_, index_);  // 重新插入元素到原位置

    // 恢复关键帧
    foreach (const NodeKeyframeTrack& track, keyframes_) {
      foreach (NodeKeyframe* key, track) {
        key->setParent(node_);  // 将关键帧的父对象设置回节点 (或其立即值对象)
      }
    }
    // 恢复标准值
    node_->SetSplitStandardValue(input_, standard_value_, index_);

    // 恢复关键帧启用状态
    if (node_->IsInputKeyframable(input_)) {
      node_->SetInputIsKeyframing(input_, is_keyframing_, index_);
    }
    // memory_manager_ 会自动清理之前 delete_all_keyframes 时保存的未恢复的 keyframe 对象
    // 如果关键帧被恢复，它们已重新设置父对象，不会被 memory_manager_ 析构。
  }

 private:
  Node* node_;     // 目标节点
  QString input_;  // 目标输入端口ID
  int index_;      // 被移除/恢复的元素索引

  SplitValue standard_value_;             // 保存的标准值
  bool is_keyframing_{};                  // 保存的是否启用关键帧的状态
  QVector<NodeKeyframeTrack> keyframes_;  // 保存的关键帧轨道
  QObject memory_manager_;                // 用于管理临时移除的关键帧对象的生命周期
};

/**
 * @brief断开两个节点参数 (NodeParam，通常指节点的输入/输出) 连接的撤销命令。
 *可以认为是 NodeParam::DisconnectEdge() 的 UndoCommand 包装器。
 */
class NodeEdgeRemoveCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param output 提供输出的上游节点。
   * @param input 接收输入的下游节点的输入端口。
   */
  NodeEdgeRemoveCommand(Node* output, NodeInput input);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回下游节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：断开连接。
   */
  void redo() override;
  /**
   * @brief 撤销命令：重新连接。
   */
  void undo() override;

 private:
  Node* output_;     // 输出节点 (上游)
  NodeInput input_;  // 输入端口 (下游节点的输入)
};

/**
 * @brief 连接两个节点参数的撤销命令。
 * 可以认为是 NodeParam::ConnectEdge() 的 UndoCommand 包装器。
 */
class NodeEdgeAddCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param output 提供输出的节点。
   * @param input 接收输入的节点的输入端口。
   */
  NodeEdgeAddCommand(Node* output, NodeInput input);

  /**
   * @brief 析构函数，需要释放可能创建的 remove_command_。
   */
  ~NodeEdgeAddCommand() override;

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回下游节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：建立连接。
   */
  void redo() override;
  /**
   * @brief 撤销命令：断开连接 (如果之前有连接，则恢复旧连接)。
   */
  void undo() override;

 private:
  Node* output_;     // 输出节点
  NodeInput input_;  // 输入端口

  // 如果添加连接时替换了现有连接，则此命令用于撤销该替换操作
  NodeEdgeRemoveCommand* remove_command_;
};

/**
 * @brief 添加节点到图 (项目) 的撤销命令。
 */
class NodeAddCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param graph 节点要添加到的图 (项目)。
   * @param node 要添加的节点。
   */
  NodeAddCommand(Project* graph, Node* node);

  /**
   * @brief (可选) 将节点相关的对象移动到指定线程。
   * @param thread 目标线程。
   */
  void PushToThread(QThread* thread);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点被添加到的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：将节点添加到图。
   */
  void redo() override;
  /**
   * @brief 撤销命令：从图中移除节点。
   */
  void undo() override;

 private:
  QObject memory_manager_;  // 用于管理节点在命令生命周期中的父对象关系

  Project* graph_;  // 目标图 (项目)
  Node* node_;      // 要添加/移除的节点
};

/**
 * @brief 移除节点并断开其所有连接的撤销命令。
 */
class NodeRemoveAndDisconnectCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 要移除的节点。
   */
  explicit NodeRemoveAndDisconnectCommand(Node* node) : node_(node), graph_(nullptr), command_(nullptr) {}

  /**
   * @brief 析构函数，释放内部的 MultiUndoCommand。
   */
  ~NodeRemoveAndDisconnectCommand() override { delete command_; }

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点原所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return graph_; }

 protected:
  /**
   * @brief 命令执行前的准备工作 (例如，创建断开连接的子命令)。
   */
  void prepare() override;

  /**
   * @brief 执行命令 (重做)：执行所有断开连接的子命令，并将节点从图中移除。
   */
  void redo() override {
    command_->redo_now();  // 执行所有断开连接的子命令

    graph_ = node_->parent();            // 记录节点原所在的图
    node_->setParent(&memory_manager_);  // 将节点的父对象设置为内存管理器，使其脱离图
  }

  /**
   * @brief 撤销命令：将节点重新设置父对象到原图，并撤销所有断开连接的子命令。
   */
  void undo() override {
    node_->setParent(graph_);  // 将节点的父对象设置回原图
    graph_ = nullptr;          // 清除记录

    command_->undo_now();  // 撤销所有断开连接的子命令
  }

 private:
  QObject memory_manager_;  // 用于管理节点在命令生命周期中（当它被临时移除时）的父对象

  Node* node_;      // 要移除/恢复的节点
  Project* graph_;  // 节点原所在的图

  MultiUndoCommand* command_;  // 包含所有断开连接子命令的复合命令
};

/**
 * @brief 移除节点及其独占依赖项，并断开所有连接的撤销命令。
 * 独占依赖项是指那些仅被当前节点及其依赖链使用的节点。
 */
class NodeRemoveWithExclusiveDependenciesAndDisconnect : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 要移除的根节点。
   */
  explicit NodeRemoveWithExclusiveDependenciesAndDisconnect(Node* node) : node_(node), command_(nullptr) {}

  /**
   * @brief 析构函数，释放内部的 MultiUndoCommand。
   */
  ~NodeRemoveWithExclusiveDependenciesAndDisconnect() override { delete command_; }

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回根节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override {
    if (command_ && command_->child_count() > 0) {  // 确保 command_ 已初始化且有子命令
      // 尝试获取第一个子命令（通常是根节点的移除命令）的相关项目
      const auto* first_child_command = dynamic_cast<const NodeRemoveAndDisconnectCommand*>(command_->child(0));
      if (first_child_command) {
        return first_child_command->GetRelevantProject();
      }
    }
    // Fallback，如果 command_ 未准备好或类型不符
    return node_ ? node_->project() : nullptr;
  }

 protected:
  /**
   * @brief 命令执行前的准备工作：创建移除根节点和所有独占依赖项的子命令。
   */
  void prepare() override {
    command_ = new MultiUndoCommand();  // 创建复合命令

    // 添加移除根节点的命令
    command_->add_child(new NodeRemoveAndDisconnectCommand(node_));

    // 移除独占依赖项
    QVector<Node*> deps = node_->GetExclusiveDependencies();  // 获取所有独占依赖
    foreach (Node* d, deps) {
      command_->add_child(new NodeRemoveAndDisconnectCommand(d));  // 为每个依赖添加移除命令
    }
  }

  /**
   * @brief 执行命令 (重做)：执行所有子命令。
   */
  void redo() override { command_->redo_now(); }

  /**
   * @brief 撤销命令：撤销所有子命令。
   */
  void undo() override { command_->undo_now(); }

 private:
  Node* node_;                 // 要移除的根节点
  MultiUndoCommand* command_;  // 包含所有移除操作的复合命令
};

/**
 * @brief 链接或取消链接两个节点的撤销命令。
 * “链接”是一种特殊的节点间关系，不同于数据流连接。
 */
class NodeLinkCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param a 第一个节点。
   * @param b 第二个节点。
   * @param link true 表示链接，false 表示取消链接。
   */
  NodeLinkCommand(Node* a, Node* b, bool link) : a_(a), b_(b), link_(link) {}

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回第一个节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return a_->project(); }

 protected:
  /**
   * @brief 执行命令 (重做)：执行链接或取消链接操作。
   */
  void redo() override {
    if (link_) {                     // 如果是链接操作
      done_ = Node::Link(a_, b_);    // 执行链接
    } else {                         // 如果是取消链接操作
      done_ = Node::Unlink(a_, b_);  // 执行取消链接
    }
  }

  /**
   * @brief 撤销命令：执行相反的链接或取消链接操作。
   */
  void undo() override {
    if (done_) {    // 仅当 redo 成功执行时才撤销
      if (link_) {  // 如果 redo 是链接，则 undo 是取消链接
        Node::Unlink(a_, b_);
      } else {  // 如果 redo 是取消链接，则 undo 是链接
        Node::Link(a_, b_);
      }
    }
  }

 private:
  Node* a_;      // 第一个节点
  Node* b_;      // 第二个节点
  bool link_;    // true 表示链接，false 表示取消链接
  bool done_{};  // 标记 redo 操作是否成功执行
};

/**
 * @brief 取消一个节点所有链接的撤销命令。
 */
class NodeUnlinkAllCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 要取消所有链接的节点。
   */
  explicit NodeUnlinkAllCommand(Node* node) : node_(node) {}

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return node_->project(); }

 protected:
  /**
   * @brief 执行命令 (重做)：保存所有链接，然后取消它们。
   */
  void redo() override {
    unlinked_ = node_->links();  // 保存当前所有链接的节点列表

    foreach (Node* link, unlinked_) {  // 遍历所有链接的节点
      Node::Unlink(node_, link);       // 取消与每个节点的链接
    }
  }

  /**
   * @brief 撤销命令：重新建立之前保存的所有链接。
   */
  void undo() override {
    foreach (Node* link, unlinked_) {  // 遍历之前保存的链接节点列表
      Node::Link(node_, link);         // 重新链接
    }
    unlinked_.clear();  // 清空保存的列表
  }

 private:
  Node* node_;               // 被操作的节点
  QVector<Node*> unlinked_;  // 保存被取消的链接节点，用于撤销
};

/**
 * @brief 链接或取消链接多对节点的复合撤销命令。
 */
class NodeLinkManyCommand : public MultiUndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param nodes 要进行操作的节点列表。
   * @param link true 表示链接，false 表示取消链接。
   */
  NodeLinkManyCommand(const QVector<Node*>& nodes, bool link) : nodes_(nodes) {
    // 为列表中的每对不同节点创建一个 NodeLinkCommand 子命令
    foreach (Node* a, nodes_) {
      foreach (Node* b, nodes_) {
        if (a != b) {                                  // 确保不是同一个节点
          add_child(new NodeLinkCommand(a, b, link));  // 添加链接/取消链接子命令
        }
      }
    }
  }

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点列表中第一个节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override {
    return nodes_.isEmpty() ? nullptr : nodes_.first()->project();
  }

 private:
  QVector<Node*> nodes_;  // 参与操作的节点列表
};

/**
 * @brief 重命名一个或多个节点 (修改其标签) 的撤销命令。
 */
class NodeRenameCommand : public UndoCommand {
 public:
  NodeRenameCommand() = default;  // 默认构造函数
  /**
   * @brief 构造函数，并添加第一个要重命名的节点。
   * @param node 要重命名的节点。
   * @param new_name 新的标签名称。
   */
  NodeRenameCommand(Node* node, const QString& new_name) { AddNode(node, new_name); }

  /**
   * @brief 添加一个要重命名的节点及其新名称。
   * @param node 要重命名的节点。
   * @param new_name 新的标签名称。
   */
  void AddNode(Node* node, const QString& new_name);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回第一个被重命名节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：将节点标签设置为新名称。
   */
  void redo() override;

  /**
   * @brief 撤销命令：将节点标签恢复为旧名称。
   */
  void undo() override;

 private:
  QVector<Node*> nodes_;  // 要重命名的节点列表

  QStringList new_labels_;  // 对应的新标签列表
  QStringList old_labels_;  // 对应的旧标签列表 (用于撤销)
};

/**
 * @brief 设置节点覆盖颜色的撤销命令。
 */
class NodeOverrideColorCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 要设置颜色的节点。
   * @param index 新的颜色索引 (-1 表示使用默认颜色)。
   */
  NodeOverrideColorCommand(Node* node, int index);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：设置新的覆盖颜色。
   */
  void redo() override;

  /**
   * @brief 撤销命令：恢复旧的覆盖颜色。
   */
  void undo() override;

 private:
  Node* node_;  // 被操作的节点

  int old_index_{};  // 旧的颜色索引
  int new_index_;    // 新的颜色索引
};

/**
 * @brief 节点视图中删除操作的撤销命令。
 * 此命令可以处理删除多个节点和多条边。
 */
class NodeViewDeleteCommand : public UndoCommand {
 public:
  NodeViewDeleteCommand();  // 构造函数

  /**
   * @brief 添加一个要从特定上下文中删除的节点。
   * @param node 要删除的节点。
   * @param context 节点所在的上下文。
   */
  void AddNode(Node* node, Node* context);

  /**
   * @brief 添加一条要删除的边。
   * @param output 输出节点。
   * @param input 输入端口。
   */
  void AddEdge(Node* output, const NodeInput& input);

  /**
   * @brief 检查命令中是否已包含要从特定上下文中删除的指定节点。
   * @param node 要检查的节点。
   * @param context 节点所在的上下文。
   * @return 如果包含则返回 true。
   */
  bool ContainsNode(Node* node, Node* context);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回第一个受影响节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：执行删除操作。
   */
  void redo() override;

  /**
   * @brief 撤销命令：恢复被删除的节点和边。
   */
  void undo() override;

 private:
  // 存储要从其上下文中移除位置信息的节点对
  QVector<Node::ContextPair> nodes_;
  // 存储要删除的边
  QVector<Node::OutputConnection> edges_;

  // 内部结构体，用于存储被移除节点的信息以备撤销
  struct RemovedNode {
    Node* node{};                   // 被移除的节点
    Node* context{};                // 节点原所在的上下文
    QPointF pos;                    // 节点在上下文中的原位置
    Project* removed_from_graph{};  // 如果节点从整个图中被移除，则记录原图
  };

  QVector<RemovedNode> removed_nodes_;  // 存储实际被移除的节点信息

  QObject memory_manager_;  // 用于管理临时移除的对象的生命周期
};

/**
 * @brief 设置节点参数 (输入) 是否启用关键帧的撤销命令。
 */
class NodeParamSetKeyframingCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param input 目标节点输入端口。
   * @param setting true 表示启用关键帧，false 表示禁用。
   */
  NodeParamSetKeyframingCommand(NodeInput input, bool setting);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回输入端口所属节点相关的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：设置新的关键帧启用状态。
   */
  void redo() override;
  /**
   * @brief 撤销命令：恢复旧的关键帧启用状态。
   */
  void undo() override;

 private:
  NodeInput input_;     // 目标输入端口
  bool new_setting_;    // 新的启用状态
  bool old_setting_{};  // 旧的启用状态 (用于撤销)
};

/**
 * @brief 向节点参数插入关键帧的撤销命令。
 */
class NodeParamInsertKeyframeCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 关键帧所属的节点 (实际上是 NodeInputImmediate 的所有者)。
   * @param keyframe 要插入的关键帧。
   */
  NodeParamInsertKeyframeCommand(Node* node, NodeKeyframe* keyframe);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：插入关键帧。
   */
  void redo() override;
  /**
   * @brief 撤销命令：移除关键帧。
   */
  void undo() override;

 private:
  Node* input_;  // 关键帧所属的节点 (应该是 NodeInputImmediate 的父节点)

  NodeKeyframe* keyframe_;  // 要插入/移除的关键帧

  QObject memory_manager_;  // 用于管理关键帧在命令生命周期中的父对象关系
};

/**
 * @brief 从节点参数移除关键帧的撤销命令。
 */
class NodeParamRemoveKeyframeCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param keyframe 要移除的关键帧。
   */
  explicit NodeParamRemoveKeyframeCommand(NodeKeyframe* keyframe);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回关键帧所属节点相关的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：移除关键帧。
   */
  void redo() override;
  /**
   * @brief 撤销命令：重新插入关键帧。
   */
  void undo() override;

 private:
  Node* input_;  // 关键帧所属的节点 (应该是 NodeInputImmediate 的父节点)

  NodeKeyframe* keyframe_;  // 要移除/恢复的关键帧

  QObject memory_manager_;  // 用于管理关键帧在命令生命周期中的父对象关系
};

/**
 * @brief 设置节点参数关键帧时间的撤销命令。
 */
class NodeParamSetKeyframeTimeCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数 (自动获取旧时间)。
   * @param key 要修改的关键帧。
   * @param time 新的时间。
   */
  NodeParamSetKeyframeTimeCommand(NodeKeyframe* key, const rational& time);
  /**
   * @brief 构造函数 (手动提供旧时间)。
   * @param key 要修改的关键帧。
   * @param new_time 新的时间。
   * @param old_time 旧的时间。
   */
  NodeParamSetKeyframeTimeCommand(NodeKeyframe* key, const rational& new_time, const rational& old_time);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回关键帧所属节点相关的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：设置新的关键帧时间。
   */
  void redo() override;
  /**
   * @brief 撤销命令：恢复旧的关键帧时间。
   */
  void undo() override;

 private:
  NodeKeyframe* key_;  // 被操作的关键帧

  rational old_time_;  // 旧的时间
  rational new_time_;  // 新的时间
};

/**
 * @brief 设置节点参数关键帧值的撤销命令。
 */
class NodeParamSetKeyframeValueCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数 (自动获取旧值)。
   * @param key 要修改的关键帧。
   * @param value 新的值。
   */
  NodeParamSetKeyframeValueCommand(NodeKeyframe* key, QVariant value);
  /**
   * @brief 构造函数 (手动提供旧值)。
   * @param key 要修改的关键帧。
   * @param new_value 新的值。
   * @param old_value 旧的值。
   */
  NodeParamSetKeyframeValueCommand(NodeKeyframe* key, QVariant new_value, QVariant old_value);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回关键帧所属节点相关的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：设置新的关键帧值。
   */
  void redo() override;
  /**
   * @brief 撤销命令：恢复旧的关键帧值。
   */
  void undo() override;

 private:
  NodeKeyframe* key_;  // 被操作的关键帧

  QVariant old_value_;  // 旧的值
  QVariant new_value_;  // 新的值
};

/**
 * @brief 设置节点参数标准值 (非关键帧、非连接时的值) 的撤销命令。
 */
class NodeParamSetStandardValueCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数 (自动获取旧值)。
   * @param input 目标参数的引用 (包含节点、输入ID、轨道、元素等信息)。
   * @param value 新的标准值。
   */
  NodeParamSetStandardValueCommand(NodeKeyframeTrackReference input, QVariant value);
  /**
   * @brief 构造函数 (手动提供旧值)。
   * @param input 目标参数的引用。
   * @param new_value 新的标准值。
   * @param old_value 旧的标准值。
   */
  NodeParamSetStandardValueCommand(NodeKeyframeTrackReference input, QVariant new_value, QVariant old_value);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回参数所属节点相关的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：设置新的标准值。
   */
  void redo() override;
  /**
   * @brief 撤销命令：恢复旧的标准值。
   */
  void undo() override;

 private:
  NodeKeyframeTrackReference ref_;  // 目标参数的引用

  QVariant old_value_;  // 旧的标准值
  QVariant new_value_;  // 新的标准值
};

/**
 * @brief 设置节点参数的分离形式标准值的撤销命令。
 * 分离值 (SplitValue) 用于处理多通道数据，如颜色 (RGBA) 或向量 (XY)。
 */
class NodeParamSetSplitStandardValueCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数 (手动提供新旧值)。
   * @param input 目标输入端口。
   * @param new_value 新的分离标准值。
   * @param old_value 旧的分离标准值。
   */
  NodeParamSetSplitStandardValueCommand(NodeInput input, SplitValue new_value, SplitValue old_value)
      : ref_(std::move(input)), old_value_(std::move(old_value)), new_value_(std::move(new_value)) {}

  /**
   * @brief 构造函数 (自动获取旧值)。
   * @param input 目标输入端口。
   * @param value 新的分离标准值。
   */
  NodeParamSetSplitStandardValueCommand(const NodeInput& input, const SplitValue& value)
      : NodeParamSetSplitStandardValueCommand(input, value,
                                              input.node()->GetSplitStandardValue(input.input(), input.element())) {
  }  // 修正: 应该传递 element

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回输入端口所属节点相关的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return ref_.node()->project(); }

 protected:
  /**
   * @brief 执行命令 (重做)：设置新的分离标准值。
   */
  void redo() override { ref_.node()->SetSplitStandardValue(ref_.input(), new_value_, ref_.element()); }

  /**
   * @brief 撤销命令：恢复旧的分离标准值。
   */
  void undo() override { ref_.node()->SetSplitStandardValue(ref_.input(), old_value_, ref_.element()); }

 private:
  NodeInput ref_;  // 目标输入端口

  SplitValue old_value_;  // 旧的分离标准值
  SplitValue new_value_;  // 新的分离标准值
};

/**
 * @brief 向节点参数的数组末尾追加元素的撤销命令。
 */
class NodeParamArrayAppendCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param node 目标节点。
   * @param input 目标数组输入端口的ID。
   */
  NodeParamArrayAppendCommand(Node* node, QString input);

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回节点所属的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行命令 (重做)：在数组末尾追加元素。
   */
  void redo() override;

  /**
   * @brief 撤销命令：移除数组末尾的元素。
   */
  void undo() override;

 private:
  Node* node_;     // 目标节点
  QString input_;  // 目标数组输入端口的ID
  // 注意: 此命令可能需要内部保存被移除元素的数据以便正确撤销，当前实现比较简单。
  // 如果append操作会创建带默认值的元素，则undo时只需resize。
  // 如果需要恢复复杂数据，则应像 NodeArrayRemoveCommand 一样保存和恢复。
};

/**
 * @brief 设置节点输入值提示 (ValueHint) 的撤销命令。
 */
class NodeSetValueHintCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param input 目标输入端口。
   * @param hint 新的值提示。
   */
  NodeSetValueHintCommand(NodeInput input, Node::ValueHint hint)
      : input_(std::move(input)), new_hint_(std::move(hint)) {}

  /**
   * @brief 构造函数 (通过节点、输入ID和元素索引)。
   * @param node 目标节点。
   * @param input 输入端口ID。
   * @param element 元素索引。
   * @param hint 新的值提示。
   */
  NodeSetValueHintCommand(Node* node, const QString& input, int element, const Node::ValueHint& hint)
      : NodeSetValueHintCommand(NodeInput(node, input, element), hint) {}

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回输入端口所属节点相关的项目指针。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return input_.node()->project(); }

 protected:
  /**
   * @brief 执行命令 (重做)：设置新的值提示，并保存旧提示。
   */
  void redo() override;

  /**
   * @brief 撤销命令：恢复旧的值提示。
   */
  void undo() override;

 private:
  NodeInput input_;  // 目标输入端口

  Node::ValueHint new_hint_;  // 新的值提示
  Node::ValueHint old_hint_;  // 旧的值提示 (用于撤销)
};

/**
 * @brief 移除 NodeInputImmediate 对象所有关键帧的撤销命令。
 * NodeInputImmediate 用于管理未连接输入端口的值和关键帧。
 */
class NodeImmediateRemoveAllKeyframesCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param immediate 目标 NodeInputImmediate 对象。
   */
  explicit NodeImmediateRemoveAllKeyframesCommand(NodeInputImmediate* immediate) : immediate_(immediate) {}

  /**
   * @brief 获取此命令相关的项目。
   * @return 返回 nullptr，因为 NodeInputImmediate 通常不直接属于项目，而是节点的一部分。
   *         或者可以返回 immediate_->node()->project() 如果需要。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return nullptr; }

 protected:
  /**
   * @brief 命令执行前的准备工作：保存所有将被移除的关键帧。
   */
  void prepare() override;

  /**
   * @brief 执行命令 (重做)：移除所有关键帧。
   */
  void redo() override;

  /**
   * @brief 撤销命令：恢复所有之前保存的关键帧。
   */
  void undo() override;

 private:
  NodeInputImmediate* immediate_;  // 目标 NodeInputImmediate 对象

  QObject memory_manager_;  // 用于管理临时移除的关键帧对象的生命周期

  QVector<NodeKeyframe*> keys_;  // 保存被移除的关键帧列表
};

}  // namespace olive

#endif  // NODEUNDO_H