#ifndef NODEVALUEDATABASE_H // 防止头文件被重复包含的宏
#define NODEVALUEDATABASE_H // 定义 NODEVALUEDATABASE_H 宏

#include "param.h" // 可能包含 NodeInput 或其他参数相关定义 (虽然在此文件中不明显直接使用)
#include "value.h" // 包含 NodeValueTable 等核心值类型的定义

namespace olive { // olive 项目的命名空间

/**
 * @brief NodeValueDatabase 类是一个容器，用于存储和管理多个 NodeValueTable。
 *
 * 它通常以输入端口的ID (QString) 作为键，每个键对应一个 NodeValueTable。
 * 这允许为节点的每个输入端口存储一个完整的值表 (NodeValueTable)，
 * 该值表可能包含了该输入在不同时间点或不同方面的值。
 *
 * 例如，在处理一个节点时，NodeTraverser 可能会为该节点的每个输入生成一个 NodeValueTable，
 * 然后将这些表存储在一个 NodeValueDatabase 中，键就是输入端口的ID。
 * 之后，当需要获取某个特定输入在某个时间点的值时，可以先从数据库中按输入ID找到对应的 NodeValueTable，
 * 然后再从该表中提取具体的值。
 */
class NodeValueDatabase {
 public:
  // 默认构造函数
  NodeValueDatabase() = default;

  /**
   * @brief 重载下标运算符 ([])，用于访问或创建与指定输入ID关联的 NodeValueTable。
   * 如果 `input_id` 不存在，会自动创建一个新的空 NodeValueTable 并插入。
   * @param input_id 输入端口的ID (作为键)。
   * @return 返回与 `input_id` 关联的 NodeValueTable 的引用。
   */
  NodeValueTable& operator[](const QString& input_id) { return tables_[input_id]; }

  /**
   * @brief向数据库中插入一个新的键值对 (输入ID -> NodeValueTable)。
   * 如果键已存在，其值将被替换。
   * @param key 输入端口的ID。
   * @param value 要插入的 NodeValueTable。
   */
  void Insert(const QString& key, const NodeValueTable& value) { tables_.insert(key, value); }

  /**
   * @brief 从数据库中获取并移除与指定键关联的 NodeValueTable。
   * @param key 要移除的 NodeValueTable 的键 (输入ID)。
   * @return 返回被移除的 NodeValueTable。如果键不存在，返回一个默认构造的 NodeValueTable。
   */
  NodeValueTable Take(const QString& key) { return tables_.take(key); }

  /**
   * @brief 将数据库中所有的 NodeValueTable 合并成一个单一的 NodeValueTable。
   * 合并的顺序和方式取决于 NodeValueTable::Merge 的实现。
   * @return 返回合并后的 NodeValueTable。
   */
  [[nodiscard]] NodeValueTable Merge() const;

  // 类型别名：Tables 代表内部存储结构，即从 QString (输入ID) 到 NodeValueTable 的哈希表。
  using Tables = QHash<QString, NodeValueTable>;
  // 类型别名：常量迭代器
  using const_iterator = Tables::const_iterator;
  // 类型别名：非常量迭代器
  using iterator = Tables::iterator;

  // 获取指向第一个元素的常量迭代器
  [[nodiscard]] inline const_iterator cbegin() const { return tables_.cbegin(); }

  // 获取指向末尾之后位置的常量迭代器
  [[nodiscard]] inline const_iterator cend() const { return tables_.cend(); }

  // 获取指向第一个元素的非常量迭代器
  inline iterator begin() { return tables_.begin(); }

  // 获取指向末尾之后位置的非常量迭代器
  inline iterator end() { return tables_.end(); }

  // 检查数据库是否包含指定键 (输入ID)
  [[nodiscard]] inline bool contains(const QString& s) const { return tables_.contains(s); }

 private:
  Tables tables_; // 内部存储，使用 QHash 将输入ID映射到 NodeValueTable
};

}  // namespace olive

// 声明 NodeValueDatabase 类型为元类型，以便在 QVariant 中使用或在信号槽中传递
Q_DECLARE_METATYPE(olive::NodeValueDatabase)

#endif  // NODEVALUEDATABASE_H