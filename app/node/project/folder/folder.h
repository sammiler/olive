#ifndef FOLDER_H // 防止头文件被多次包含的宏定义开始
#define FOLDER_H

#include "node/node.h" // 引入基类 Node 的定义

// 可能需要的前向声明
// class QVariant; // 假设
// class UndoCommand; // 假设
// class Project; // 假设
// class MultiUndoCommand; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表项目结构中的一个“文件夹”或“目录”。
 * 尽管 Node (或其基类 Item) 可能已经有支持子项的结构，但通常默认是禁用的。
 * Folder 类明确地启用了这种层级结构功能，允许用户将多个项目（节点）组织到一个集合中。
 */
class Folder : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief Folder 构造函数。
   */
  Folder();

  NODE_DEFAULT_FUNCTIONS(Folder) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此文件夹节点的名称。
   * @return QString 类型的节点名称 (例如 "文件夹")，支持国际化翻译。
   */
  [[nodiscard]] QString Name() const override { return tr("Folder"); }

  /**
   * @brief 获取此文件夹节点的唯一标识符。
   * @return QString 类型的节点 ID (例如 "org.olivevideoeditor.Olive.folder")。
   */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.folder"); }

  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "项目" (kCategoryProject) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryProject}; }

  /**
   * @brief 获取此文件夹节点的描述信息。
   * @return QString 类型的节点描述 (例如 "将多个项目组织成一个单一集合。")，支持国际化翻译。
   */
  [[nodiscard]] QString Description() const override { return tr("Organize several items into a single collection."); }

  /**
   * @brief 获取特定类型的数据（例如，文件夹特定的元数据或子项列表信息）。
   * @param d 要获取的数据类型标识。
   * @return QVariant 包含所请求数据的 QVariant 对象。
   */
  [[nodiscard]] QVariant data(const DataType& d) const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述)。
   */
  void Retranslate() override;

  /**
   * @brief 根据名称查找此文件夹下的直接子节点。
   * @param s 要查找的子节点的名称。
   * @return Node* 指向找到的子节点的指针，如果未找到则为 nullptr。
   */
  [[nodiscard]] Node* GetChildWithName(const QString& s) const;
  /**
   * @brief 检查此文件夹下是否存在指定名称的直接子节点。
   * @param s 要检查的子节点的名称。
   * @return 如果存在则返回 true，否则返回 false。
   */
  [[nodiscard]] bool ChildExistsWithName(const QString& s) const { return GetChildWithName(s); }

  /**
   * @brief 递归地检查指定的节点是否是此文件夹的子节点（直接或间接）。
   * @param child 要检查的节点。
   * @return 如果是子节点则返回 true，否则返回 false。
   */
  bool HasChildRecursive(Node* child) const;

  /** @brief 获取此文件夹直接子节点的数量。 */
  [[nodiscard]] int item_child_count() const { return item_children_.size(); }

  /**
   * @brief 根据索引获取此文件夹的直接子节点。
   * @param i 子节点的索引。
   * @return Node* 指向子节点的指针，如果索引无效则行为未定义（可能崩溃或返回 nullptr，取决于 QVector::at 的行为）。
   */
  [[nodiscard]] Node* item_child(int i) const { return item_children_.at(i); }

  /**
   * @brief 获取此文件夹所有直接子节点的列表。
   * @return const QVector<Node*>& 对子节点指针向量的常量引用。
   */
  [[nodiscard]] const QVector<Node*>& children() const { return item_children_; }

  /**
   * @brief 获取指定子节点在此文件夹直接子节点列表中的索引。
   * @param item 要查找索引的子节点。
   * @return int 子节点的索引，如果未找到则返回 -1。
   */
  int index_of_child(Node* item) const { return item_children_.indexOf(item); }

  /**
   * @brief 获取指定子节点在其父节点（即此文件夹）输入数组中的元素索引。
   *  这通常与子节点是如何连接到文件夹节点的输入端口相关。
   * @param item 要查找索引的子节点。
   * @return int 元素索引，如果未找到或不适用则可能返回特定值。
   */
  int index_of_child_in_array(Node* item) const;

  /**
   * @brief (模板函数) 递归地列出此文件夹及其子文件夹下所有指定类型的子节点。
   * @tparam T 要查找的子节点的类型。
   * @return QVector<T*> 包含所有找到的指定类型子节点指针的向量。
   */
  template <typename T>
  [[nodiscard]] QVector<T*> ListChildrenOfType() const {
    QVector<T*> list; // 初始化空列表

    foreach (Node* node, item_children_) { // 遍历所有直接子节点
      T* cast_test = dynamic_cast<T*>(node); // 尝试将子节点转换为类型 T
      if (cast_test) { // 如果转换成功
        list.append(cast_test); // 添加到列表中
      }

      auto* folder_test = dynamic_cast<Folder*>(node); // 尝试将子节点转换为 Folder 类型
      if (folder_test) { // 如果是子文件夹
        list.append(folder_test->ListChildrenOfType<T>()); // 递归调用并合并结果
      }
    }

    return list; // 返回结果列表
  }

  static const QString kChildInput; ///< "Children" - 用于连接子节点的输入端口（通常是一个数组输入）的键名。

  /**
   * @brief 用于从文件夹中移除一个子元素（节点）的撤销/重做命令类。
   */
  class RemoveElementCommand : public UndoCommand {
   public:
    /**
     * @brief RemoveElementCommand 构造函数。
     * @param folder 目标文件夹。
     * @param child 要移除的子节点。
     */
    RemoveElementCommand(Folder* folder, Node* child) : folder_(folder), child_(child), subcommand_(nullptr) {}

    /** @brief RemoveElementCommand 析构函数，删除可能存在的子命令。 */
    ~RemoveElementCommand() override { delete subcommand_; }

    /** @brief 获取与此撤销命令相关的项目。 */
    [[nodiscard]] Project* GetRelevantProject() const override { return folder_->project(); }

   protected:
    /** @brief 执行“重做”操作（即移除子节点）。 */
    void redo() override;

    /**
     * @brief 执行“撤销”操作（即重新添加之前移除的子节点）。
     *  如果移除操作内部产生了其他子命令（例如，断开连接），则也撤销这些子命令。
     */
    void undo() override {
      if (subcommand_) { // 如果有子命令
        subcommand_->undo_now(); // 立即撤销子命令
      }
    }

   private:
    Folder* folder_; ///< 指向目标文件夹的指针。
    Node* child_;    ///< 要移除/重新添加的子节点。
    int remove_index_{}; ///< 子节点被移除前在其父文件夹中的索引，用于撤销时恢复位置。
    MultiUndoCommand* subcommand_; ///< 可能用于聚合移除操作内部产生的多个子命令。
  };

 signals: // Qt 信号声明区域
  /**
   * @brief 在即将向文件夹插入一个子项之前发射此信号。
   * @param n 要插入的子项（节点）。
   * @param index 子项将被插入的位置索引。
   */
  void BeginInsertItem(Node* n, int index);

  /**
   * @brief 在向文件夹插入子项操作完成后发射此信号。
   */
  void EndInsertItem();

  /**
   * @brief 在即将从文件夹移除一个子项之前发射此信号。
   * @param n 要移除的子项（节点）。
   * @param index 子项当前的位置索引。
   */
  void BeginRemoveItem(Node* n, int index);

  /**
   * @brief 在从文件夹移除子项操作完成后发射此信号。
   */
  void EndRemoveItem();

 protected:
  /**
   * @brief 当文件夹的某个输入端口（通常是 kChildInput 数组的某个元素）连接上一个输出节点（即子节点）时调用的事件处理函数。
   * @param input 连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param output 连接到的输出节点 (即子节点)。
   */
  void InputConnectedEvent(const QString& input, int element, Node* output) override;

  /**
   * @brief 当文件夹的某个输入端口（子节点连接）断开时调用的事件处理函数。
   * @param input 断开连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param output 之前连接的输出节点 (即子节点)。
   */
  void InputDisconnectedEvent(const QString& input, int element, Node* output) override;

 private:
  /**
   * @brief (私有静态模板函数) 递归地列出给定文件夹节点及其子文件夹中，连接到其输出端口的特定类型的节点。
   *  这似乎是查找文件夹“输出”的节点，而不是其“子项”。
   * @tparam T 要查找的节点类型。
   * @param n 当前要检查的文件夹节点。
   * @param list (输出参数) 用于存储找到的节点指针的向量。
   * @param recursive 是否递归查找子文件夹。
   */
  template <typename T>
  static void ListOutputsOfTypeInternal(const Folder* n, QVector<T*>& list, bool recursive) {
    foreach (const Node::OutputConnection& c, n->output_connections()) { // 遍历所有输出连接
      Node* connected = c.second.node(); // 获取连接的节点

      T* cast_test = dynamic_cast<T*>(connected); // 尝试转换为类型 T
      if (cast_test) { // 如果成功
        // Avoid duplicates // 避免重复添加
        if (!list.contains(cast_test)) {
          list.append(cast_test);
        }
      }

      if (recursive) { // 如果需要递归
        auto* subfolder = dynamic_cast<Folder*>(connected); // 检查连接的是否是另一个文件夹
        if (subfolder) {
          ListOutputsOfTypeInternal(subfolder, list, recursive); // 递归调用
        }
      }
    }
  }

  QVector<Node*> item_children_;      ///< 存储此文件夹直接子节点的指针向量。
  QVector<int> item_element_index_; ///< 存储每个子节点对应在其父文件夹（即此 Folder 对象）的 kChildInput 输入数组中的元素索引。
};

/**
 * @brief 用于向文件夹添加一个子节点的撤销/重做命令类。
 */
class FolderAddChild : public UndoCommand {
 public:
  /**
   * @brief FolderAddChild 构造函数。
   * @param folder 目标文件夹。
   * @param child 要添加的子节点。
   */
  FolderAddChild(Folder* folder, Node* child);

  /** @brief 获取与此撤销命令相关的项目。 */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /** @brief 执行“重做”操作（即添加子节点）。 */
  void redo() override;

  /** @brief 执行“撤销”操作（即移除之前添加的子节点）。 */
  void undo() override;

 private:
  Folder* folder_; ///< 指向目标文件夹的指针。
  Node* child_;    ///< 要添加/移除的子节点。
};

}  // namespace olive

#endif  // FOLDER_H // 头文件宏定义结束