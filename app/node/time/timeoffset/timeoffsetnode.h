#ifndef TIMEOFFSETNODE_H  // 防止头文件被多次包含的宏定义开始
#define TIMEOFFSETNODE_H

#include "node/node.h"  // 引入基类 Node 的定义

// 可能需要的前向声明
// class TimeRange; // 假设
// class rational; // 假设
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个“时间偏移”节点。
 * 该节点用于修改流经它的时间。当数据（如视频帧或音频样本）通过此节点时，
 * 它会根据用户设置的偏移量来调整请求这些数据的时间点。
 * 例如，如果偏移量是 +10 帧，当请求第 0 帧时，此节点会向上游请求第 10 帧的数据。
 * 这常用于制作慢动作、快动作（通过时间重映射）、或者简单的同步调整。
 */
class TimeOffsetNode : public Node {
 public
     :  // Q_OBJECT
        // 宏通常在此，但如果此类不需要信号槽，则可以省略（但这里没写，可能是由NODE_DEFAULT_FUNCTIONS宏处理，或不需要）
  /**
   * @brief TimeOffsetNode 构造函数。
   *  通常会在这里初始化时间偏移相关的输入参数。
   */
  TimeOffsetNode();

  NODE_DEFAULT_FUNCTIONS(TimeOffsetNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此时间偏移节点的名称。
   * @return QString 类型的节点名称 (例如 "时间偏移")，支持国际化翻译。
   */
  [[nodiscard]] QString Name() const override { return tr("Time Offset"); }

  /**
   * @brief 获取此时间偏移节点的唯一标识符。
   * @return QString 类型的节点 ID (例如 "org.olivevideoeditor.Olive.timeoffset")。
   */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.timeoffset"); }

  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "时间" (kCategoryTime) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryTime}; }

  /**
   * @brief 获取此时间偏移节点的描述信息。
   * @return QString 类型的节点描述 (例如 "偏移流经图的时间。")，支持国际化翻译。
   */
  [[nodiscard]] QString Description() const override { return tr("Offset time passing through the graph."); }

  /**
   * @brief 根据输入调整时间范围（从下游请求时间到上游请求时间）。
   *  此方法会应用时间偏移。
   * @param input 输入端口的名称。
   * @param element 输入端口的元素索引。
   * @param input_time 下游节点请求的时间范围。
   * @param clamp 是否将时间限制在有效范围内。
   * @return TimeRange 经过偏移调整后，向上游节点请求的时间范围。
   */
  [[nodiscard]] TimeRange InputTimeAdjustment(const QString& input, int element, const TimeRange& input_time,
                                              bool clamp) const override;
  /**
   * @brief 根据输出调整时间范围（从上游提供时间到下游提供时间）。
   *  此方法会反向应用时间偏移。
   * @param input 输入端口的名称。
   * @param element 输入端口的元素索引。
   * @param input_time 上游节点提供数据的时间范围。
   * @return TimeRange 经过偏移调整后，向下游节点提供数据的时间范围。
   */
  [[nodiscard]] TimeRange OutputTimeAdjustment(const QString& input, int element,
                                               const TimeRange& input_time) const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值。
   *  对于时间偏移节点，它通常只是将输入数据在经过时间调整后原样传递。
   * @param value 当前输入行数据。
   * @param globals 全局节点处理参数 (包含了经过此节点调整后的时间)。
   * @param table 用于存储输出值的表。
   */
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTimeInput;   ///< "Offset" 或 "TimeShift" - 时间偏移量的参数键名。
  static const QString kInputInput;  ///< "Input" - 连接上游数据源的输入端口键名。

 private:
  /**
   * @brief 根据当前设置的偏移量，获取重新映射（偏移）后的时间。
   * @param input 原始输入时间。
   * @return rational 经过偏移调整后的时间。
   */
  [[nodiscard]] rational GetRemappedTime(const rational& input) const;
};

}  // namespace olive

#endif  // TIMEOFFSETNODE_H // 头文件宏定义结束