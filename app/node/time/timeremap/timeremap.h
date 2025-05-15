#ifndef TIMEREMAPNODE_H // 防止头文件被多次包含的宏定义开始
#define TIMEREMAPNODE_H

#include "node/node.h" // 引入基类 Node 的定义

// 可能需要的前向声明
// class TimeRange; // 假设
// class rational; // 假设
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表一个“时间重映射”节点。
 * 与 TimeOffsetNode 类似，此节点也用于修改流经它的时间。
 * 不同之处在于，TimeRemapNode 通常允许更复杂的时间变换，
 * 例如通过一条曲线或关键帧来定义输出时间与输入时间之间的映射关系，
 * 从而实现变速播放效果（如慢动作、快动作、时间冻结、倒放等）。
 * "kTimeInput" 参数可能不再是一个简单的偏移量，而是一个更复杂的数据结构或曲线输入。
 */
class TimeRemapNode : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief TimeRemapNode 构造函数。
   *  通常会在这里初始化时间重映射相关的输入参数。
   */
  TimeRemapNode();

  NODE_DEFAULT_FUNCTIONS(TimeRemapNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此时间重映射节点的名称。
   * @return QString 类型的节点名称 (例如 "时间重映射" 或 "Time Remap")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此时间重映射节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "时间" (Time) 或 "特效" (Effect) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此时间重映射节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 根据输入调整时间范围（从下游请求时间到上游请求时间）。
   *  此方法会应用时间重映射逻辑。
   * @param input 输入端口的名称。
   * @param element 输入端口的元素索引。
   * @param input_time 下游节点请求的时间范围。
   * @param clamp 是否将时间限制在有效范围内。
   * @return TimeRange 经过重映射调整后，向上游节点请求的时间范围。
   */
  [[nodiscard]] TimeRange InputTimeAdjustment(const QString& input, int element, const TimeRange& input_time,
                                              bool clamp) const override;
  /**
   * @brief 根据输出调整时间范围（从上游提供时间到下游提供时间）。
   *  此方法会反向应用时间重映射逻辑。
   * @param input 输入端口的名称。
   * @param element 输入端口的元素索引。
   * @param input_time 上游节点提供数据的时间范围。
   * @return TimeRange 经过重映射调整后，向下游节点提供数据的时间范围。
   */
  [[nodiscard]] TimeRange OutputTimeAdjustment(const QString& input, int element,
                                               const TimeRange& input_time) const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值。
   *  对于时间重映射节点，它通常只是将输入数据在经过时间调整后原样传递。
   * @param value 当前输入行数据。
   * @param globals 全局节点处理参数 (包含了经过此节点调整后的时间)。
   * @param table 用于存储输出值的表。
   */
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTimeInput;  ///< "TimeMap" 或 "RemapCurve" - 定义时间映射关系的参数键名 (可能是一个曲线或关键帧序列)。
  static const QString kInputInput; ///< "Input" - 连接上游数据源的输入端口键名。

 private:
  /**
   * @brief 根据当前设置的时间映射关系，获取重新映射后的时间。
   * @param input 原始输入时间 (通常是全局时间线上的时间)。
   * @return rational 经过重映射逻辑计算得到的、用于向上游请求数据的时间。
   */
  [[nodiscard]] rational GetRemappedTime(const rational& input) const;
};

}  // namespace olive

#endif  // TIMEREMAPNODE_H // 头文件宏定义结束