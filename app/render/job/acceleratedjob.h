#ifndef ACCELERATEDJOB_H // 防止头文件被重复包含的宏
#define ACCELERATEDJOB_H // 定义 ACCELERATEDJOB_H 宏

#include "node/param.h"       // 可能包含 NodeInput 或其他参数相关定义 (虽然在此文件中不明显直接使用)
#include "node/valuedatabase.h" // 包含 NodeValueRow (QHash<QString, NodeValue>) 的定义，
                              // 也可能间接包含 NodeValue

namespace olive { // olive 项目的命名空间

/**
 * @brief AcceleratedJob 类是一个基类或通用结构，用于封装提交给加速处理单元 (如 GPU) 的任务所需的数据。
 *
 * 它主要包含一个 NodeValueRow (value_map_)，这个映射存储了任务执行时需要的输入参数值。
 * 输入参数的键是输入端口的ID (QString)，值是对应的 NodeValue。
 *
 * 这个类的实例通常由节点的 Value() 方法创建，然后由 NodeTraverser
 * (或其派生类，如一个GPU渲染器) 来处理。
 * 具体的加速任务类型 (如 ShaderJob, GenerateJob) 可能会继承自 AcceleratedJob，
 * 或者 AcceleratedJob 本身作为这些具体任务类型内部持有的一个数据成员。
 *
 * 它提供了插入和获取这些参数值的方法。
 */
class AcceleratedJob {
 public:
  // 默认构造函数
  AcceleratedJob() = default;

  // 虚析构函数，允许派生类正确析构
  virtual ~AcceleratedJob() = default;

  /**
   * @brief 根据输入端口ID获取参数值。
   * @param input 输入端口的ID。
   * @return 返回对应的 NodeValue。如果输入ID不存在，返回一个默认构造的 NodeValue (通常 Type 为 kNone)。
   */
  [[nodiscard]] NodeValue Get(const QString &input) const { return value_map_.value(input); }

  /**
   * @brief 从一个 NodeValueRow 中提取指定输入ID的值，并插入到当前任务的参数映射中。
   * @param input 要提取和插入的输入端口ID。
   * @param row 包含多个输入值的 NodeValueRow。
   */
  void Insert(const QString &input, const NodeValueRow &row) { value_map_.insert(input, row.value(input)); }

  /**
   * @brief 为指定的输入端口ID插入一个 NodeValue。
   * @param input 输入端口的ID。
   * @param value 要插入的 NodeValue。
   */
  void Insert(const QString &input, const NodeValue &value) { value_map_.insert(input, value); }

  /**
   * @brief 将一个 NodeValueRow 中的所有键值对插入到当前任务的参数映射中。
   * @param row 要插入的 NodeValueRow。
   */
  void Insert(const NodeValueRow &row) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0) // Qt 5.15 及更高版本 QHash 支持直接插入另一个 QHash
    value_map_.insert(row);
#else // 兼容旧版 Qt
    // 遍历 row 中的每个键值对并逐个插入
    for (auto it = row.cbegin(); it != row.cend(); it++) {
      value_map_.insert(it.key(), it.value());
    }
#endif
  }

  /**
   * @brief 获取存储所有参数值的 NodeValueRow 的常量引用。
   * @return 返回 value_map_ 的常量引用。
   */
  [[nodiscard]] const NodeValueRow &GetValues() const { return value_map_; }
  /**
   * @brief 获取存储所有参数值的 NodeValueRow 的引用 (允许修改)。
   * @return 返回 value_map_ 的引用。
   */
  NodeValueRow &GetValues() { return value_map_; }

 private:
  NodeValueRow value_map_; // 存储任务参数的映射 (输入ID -> NodeValue)
};

}  // namespace olive

#endif  // ACCELERATEDJOB_H