#ifndef VALUENODE_H // 防止头文件被多次包含的宏定义开始
#define VALUENODE_H

#include "node/node.h" // 引入基类 Node 的定义

// 可能需要的前向声明
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设
// namespace NodeValue { enum class Type; } // 假设 NodeValue::Type 枚举

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表一个“值节点”或“常量节点”。
 * 该节点用于生成一个用户定义的常量值，这个值可以是多种数据类型（如数字、颜色、向量等）。
 * 生成的值可以连接到其他节点的输入参数，方便统一控制或创建可复用的常量。
 */
class ValueNode : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief ValueNode 构造函数。
   *  通常会在这里初始化值的类型和默认值等参数。
   */
  ValueNode();

  NODE_DEFAULT_FUNCTIONS(ValueNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此值节点的名称。
   * @return QString 类型的节点名称 (例如 "值" 或 "Value")，支持国际化翻译。
   */
  [[nodiscard]] QString Name() const override { return tr("Value"); }

  /**
   * @brief 获取此值节点的唯一标识符。
   * @return QString 类型的节点 ID (例如 "org.olivevideoeditor.Olive.value")。
   */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.value"); }

  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "生成器" (kCategoryGenerator) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryGenerator}; }

  /**
   * @brief 获取此值节点的描述信息。
   * @return QString 类型的节点描述 (例如 "创建一个可以连接到其他各种输入的单一值。")，支持国际化翻译。
   */
  [[nodiscard]] QString Description() const override {
    return tr("Create a single value that can be connected to various other inputs.");
  }

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTypeInput;  ///< "Type" - 选择生成值的类型的参数键名 (例如，整数、浮点数、颜色等)。
  static const QString kValueInput; ///< "Value" - 实际存储用户定义的值的参数键名。其具体数据类型取决于 kTypeInput 的设置。

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值（即用户定义的常量值）。
   * @param value 当前输入行数据 (包含值的类型和实际值)。
   * @param globals 全局节点处理参数 (对于常量值节点，此参数可能不被直接使用)。
   * @param table 用于存储输出值的表 (输出用户定义的常量值)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

 protected:
  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   *  例如，当用户更改了 `kTypeInput`（值的类型）时，可能需要更新 `kValueInput` 参数的编辑器或约束。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString &input, int element) override;

 private:
  // 存储此值节点支持的数据类型的列表。
  // NodeValue::Type 可能是定义在 NodeValue 类或命名空间下的一个枚举，表示节点可以处理的各种数据类型。
  static const QVector<NodeValue::Type> kSupportedTypes; ///< 支持的值类型列表。
};

}  // namespace olive

#endif  // VALUENODE_H // 头文件宏定义结束