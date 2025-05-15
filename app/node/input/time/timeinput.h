#ifndef TIMEINPUT_H // 防止头文件被多次包含的宏定义开始
#define TIMEINPUT_H

#include "node/node.h" // 引入基类 Node 的定义

// 可能需要的前向声明
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表一个“时间输入”节点。
 * 该节点的主要功能是提供当前处理的全局时间信息作为其输出。
 * 它可以用于驱动动画、基于时间的效果，或者在节点图中将时间数据传递给其他节点。
 */
class TimeInput : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief TimeInput 构造函数。
   */
  TimeInput();

  NODE_DEFAULT_FUNCTIONS(TimeInput) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此时间输入节点的名称。
   * @return QString 类型的节点名称 (例如 "时间" 或 "Time Input")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此时间输入节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "输入" (Input) 或 "生成器" (Generator) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此时间输入节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 计算并输出节点在特定时间点的值（即当前时间）。
   *  它会从 NodeGlobals 对象中获取当前时间，并将其放入输出的 NodeValueTable 中。
   * @param value 当前输入行数据 (对于纯时间输入节点，此参数可能不被直接使用)。
   * @param globals 全局节点处理参数，其中包含当前时间信息。
   * @param table 用于存储输出值的表 (输出当前时间值)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;
};

}  // namespace olive

#endif  // TIMEINPUT_H // 头文件宏定义结束