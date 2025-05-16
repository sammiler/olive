#ifndef NODEGROUP_H  // 防止头文件被多次包含的宏定义开始
#define NODEGROUP_H

#include <utility>  // 引入 std::move 等工具，可能用于 NodeInput 对象的传递

#include "node/node.h"  // 引入基类 Node 的定义

// 可能需要的前向声明 (如果 QXmlStreamReader 等在此未完全定义)
// class QXmlStreamReader;
// class QXmlStreamWriter;
// class SerializedData;
// class NodeInput; // 假设 NodeInput 是一个类或结构体
// class UndoCommand; // 假设 UndoCommand 是一个基类
// class Project; // 假设

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个“节点组”。
 * 节点组允许将多个节点封装在一起，形成一个复合节点，这个复合节点可以像单个节点一样被使用。
 * 它通过“输入直通”(Input Passthrough) 和“输出直通”(Output Passthrough) 机制，
 * 将内部节点的输入和输出暴露给外部。
 */
class NodeGroup : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief NodeGroup 构造函数。
      */
     NodeGroup();

  NODE_DEFAULT_FUNCTIONS(NodeGroup)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点组的名称。
   * @return QString 类型的节点组名称 (例如 "我的节点组")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点组的唯一标识符。
   * @return QString 类型的节点组 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点组所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点组可能属于 "组" (Group) 或用户自定义分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此节点组的描述信息。
   * @return QString 类型的节点组描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 从 XML 流加载节点组的自定义数据（例如，内部节点连接和直通设置）。
   * @param reader 指向 QXmlStreamReader 的指针，用于读取 XML 数据。
   * @param data 指向 SerializedData 的指针，可能包含反序列化过程中的辅助数据。
   * @return 如果加载成功则返回 true，否则返回 false。
   */
  bool LoadCustom(QXmlStreamReader *reader, SerializedData *data) override;
  /**
   * @brief 将节点组的自定义数据保存到 XML 流。
   * @param writer 指向 QXmlStreamWriter 的指针，用于写入 XML 数据。
   */
  void SaveCustom(QXmlStreamWriter *writer) const override;
  /**
   * @brief 在所有节点和其基本数据加载完毕后调用的事件。
   *  用于处理节点组内部的连接恢复等依赖于其他对象已完全加载的操作。
   * @param data 指向 SerializedData 的指针。
   */
  void PostLoadEvent(SerializedData *data) override;

  /**
   * @brief 为节点组添加一个输入直通。
   *  这将使组内某个节点的特定输入参数暴露为该节点组自身的一个输入参数。
   * @param input 要设置为直通的内部节点的输入 (NodeInput 对象)。
   * @param force_id (可选) 强制指定该直通输入在节点组上的外部 ID。如果为空，则自动生成。
   * @return QString 新添加的直通输入在节点组上的外部 ID。
   */
  QString AddInputPassthrough(const NodeInput &input, const QString &force_id = QString());

  /**
   * @brief 移除一个已存在的输入直通。
   * @param input 要移除的内部节点的输入 (NodeInput 对象)。
   */
  void RemoveInputPassthrough(const NodeInput &input);

  /**
   * @brief 获取当前设置为输出直通的内部节点。
   *  该内部节点的输出将作为整个节点组的输出。
   * @return Node* 指向作为输出直通的内部节点的指针，如果没有设置则为 nullptr。
   */
  [[nodiscard]] Node *GetOutputPassthrough() const { return output_passthrough_; }

  /**
   * @brief 设置一个内部节点作为输出直通。
   * @param node 要设置为输出直通的内部节点。
   */
  void SetOutputPassthrough(Node *node);

  /** @brief 输入直通的类型别名，表示一个 QPair，包含直通的外部ID (QString) 和对应的内部节点输入 (NodeInput)。 */
  using InputPassthrough = QPair<QString, NodeInput>;
  /** @brief 存储多个输入直通的 QVector 的类型别名。 */
  using InputPassthroughs = QVector<InputPassthrough>;
  /**
   * @brief 获取当前节点组所有输入直通的列表。
   * @return const InputPassthroughs& 对输入直通列表的常量引用。
   */
  [[nodiscard]] const InputPassthroughs &GetInputPassthroughs() const { return input_passthroughs_; }

  /**
   * @brief 检查指定的内部节点输入是否已经存在一个对应的输入直通。
   * @param input 要检查的内部节点的输入 (NodeInput 对象)。
   * @return 如果存在直通则返回 true，否则返回 false。
   */
  [[nodiscard]] bool ContainsInputPassthrough(const NodeInput &input) const;

  /**
   * @brief 根据直通输入的外部 ID 获取其在界面上显示的名称。
   * @param id 直通输入的外部 ID。
   * @return QString 输入参数的名称。
   */
  [[nodiscard]] QString GetInputName(const QString &id) const override;

  /**
   * @brief (静态工具函数) 解析一个 NodeInput 对象，可能用于规范化或获取其最终目标。
   * @param input 待解析的 NodeInput 对象。
   * @return NodeInput 解析后的 NodeInput 对象。
   */
  static NodeInput ResolveInput(NodeInput input);
  /**
   * @brief (静态工具函数) 获取一个 NodeInput 对象内部的实际目标，可能用于处理嵌套的直通。
   * @param input 指向 NodeInput 对象的指针，函数可能会修改它以指向更深层次的输入。
   * @return 如果成功获取到内部目标则返回 true，否则返回 false。
   */
  static bool GetInner(NodeInput *input);

  /**
   * @brief 根据内部节点的输入 (NodeInput) 获取其对应的输入直通的外部 ID。
   * @param input 内部节点的输入。
   * @return QString 如果找到对应的直通，则返回其外部 ID；否则返回空 QString。
   */
  [[nodiscard]] QString GetIDOfPassthrough(const NodeInput &input) const {
    for (const auto &input_passthrough : input_passthroughs_) {  // 遍历所有输入直通
      if (input_passthrough.second == input) {                   // 如果内部节点输入匹配
        return input_passthrough.first;                          // 返回外部 ID
      }
    }
    return {};  // 未找到则返回空
  }

  /**
   * @brief 根据输入直通的外部 ID 获取其对应的内部节点输入 (NodeInput)。
   * @param id 输入直通的外部 ID。
   * @return NodeInput 如果找到对应的内部输入，则返回它；否则返回一个无效的 NodeInput 对象。
   */
  [[nodiscard]] NodeInput GetInputFromID(const QString &id) const {
    for (const auto &input_passthrough : input_passthroughs_) {  // 遍历所有输入直通
      if (input_passthrough.first == id) {                       // 如果外部 ID 匹配
        return input_passthrough.second;                         // 返回内部节点输入
      }
    }
    return {};  // 未找到则返回空（或默认构造的 NodeInput）
  }

 signals:  // Qt 信号声明区域
  /**
   * @brief 当节点组成功添加一个新的输入直通时发射此信号。
   * @param group 发生改变的 NodeGroup 对象。
   * @param input 新添加的直通对应的内部节点输入。
   */
  void InputPassthroughAdded(olive::NodeGroup *group, const olive::NodeInput &input);

  /**
   * @brief 当节点组成功移除一个输入直通时发射此信号。
   * @param group 发生改变的 NodeGroup 对象。
   * @param input 被移除的直通对应的内部节点输入。
   */
  void InputPassthroughRemoved(olive::NodeGroup *group, const olive::NodeInput &input);

  /**
   * @brief 当节点组的输出直通目标发生改变时发射此信号。
   * @param group 发生改变的 NodeGroup 对象。
   * @param output 新的输出直通节点指针。
   */
  void OutputPassthroughChanged(olive::NodeGroup *group, olive::Node *output);

 private:
  InputPassthroughs input_passthroughs_;  ///< 存储所有输入直通的列表。

  Node *output_passthrough_;  ///< 指向当前作为输出直通的内部节点的指针。
};

/**
 * @brief 用于添加节点组输入直通操作的撤销/重做命令类。
 */
class NodeGroupAddInputPassthrough : public UndoCommand {
 public:
  /**
   * @brief NodeGroupAddInputPassthrough 构造函数。
   * @param group 目标 NodeGroup 对象。
   * @param input 要添加为直通的内部节点输入。
   * @param force_id (可选) 强制指定的外部 ID。
   */
  NodeGroupAddInputPassthrough(NodeGroup *group, NodeInput input, const QString &force_id = QString())
      : group_(group), input_(std::move(input)), force_id_(force_id), actually_added_(false) {}  // 成员初始化列表

  /**
   * @brief 获取与此撤销命令相关的项目。
   * @return Project* 指向相关项目的指针。
   */
  [[nodiscard]] Project *GetRelevantProject() const override { return group_->project(); }

 protected:
  /**
   * @brief 执行“重做”操作（即添加输入直通）。
   */
  void redo() override;

  /**
   * @brief 执行“撤销”操作（即移除之前添加的输入直通）。
   */
  void undo() override;

 private:
  NodeGroup *group_;     ///< 指向目标 NodeGroup 对象的指针。
  NodeInput input_;      ///< 要添加/移除的内部节点输入。
  QString force_id_;     ///< (可选) 强制指定的外部 ID。
  bool actually_added_;  ///< 标记在 redo 操作中是否真的添加了新的直通（用于 undo 的正确性）。
};

/**
 * @brief 用于设置节点组输出直通操作的撤销/重做命令类。
 */
class NodeGroupSetOutputPassthrough : public UndoCommand {
 public:
  /**
   * @brief NodeGroupSetOutputPassthrough 构造函数。
   * @param group 目标 NodeGroup 对象。
   * @param output 要设置为新的输出直通的内部节点。
   */
  NodeGroupSetOutputPassthrough(NodeGroup *group, Node *output) : group_(group), new_output_(output) {}

  /**
   * @brief 获取与此撤销命令相关的项目。
   * @return Project* 指向相关项目的指针。
   */
  [[nodiscard]] Project *GetRelevantProject() const override { return group_->project(); }

 protected:
  /**
   * @brief 执行“重做”操作（即设置新的输出直通）。
   */
  void redo() override;

  /**
   * @brief 执行“撤销”操作（即恢复到旧的输出直通）。
   */
  void undo() override;

 private:
  NodeGroup *group_;    ///< 指向目标 NodeGroup 对象的指针。
  Node *new_output_;    ///< 新的输出直通节点。
  Node *old_output_{};  ///< 旧的输出直通节点，在 redo 时保存，用于 undo。
};

}  // namespace olive

#endif  // NODEGROUP_H // 头文件宏定义结束