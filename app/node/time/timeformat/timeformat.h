#ifndef TIMEFORMAT_H  // 防止头文件被多次包含的宏定义开始
#define TIMEFORMAT_H

#include "node/node.h"  // 引入基类 Node 的定义

// 可能需要的前向声明
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设
// class rational; // 如果 kTimeInput 的类型是 rational

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个“时间格式化”节点。
 * 该节点接收一个时间值（可能是一个 rational 类型的数字，代表秒或帧数）
 * 和一个格式化字符串，然后将时间值按照指定的格式转换为一个文本字符串。
 * 例如，可以将时间转换为 "HH:MM:SS:FF" (时:分:秒:帧) 或其他自定义格式。
 */
class TimeFormatNode : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief TimeFormatNode 构造函数。
      *  通常会在这里初始化时间格式化相关的输入参数。
      */
     TimeFormatNode();

  NODE_DEFAULT_FUNCTIONS(TimeFormatNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此时间格式化节点的名称。
   * @return QString 类型的节点名称 (例如 "时间格式化" 或 "Time Format")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此时间格式化节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "文本" (Text)、"工具" (Utility) 或 "格式化" (Formatting) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此时间格式化节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值（即格式化后的时间字符串）。
   * @param value 当前输入行数据 (包含要格式化的时间值、格式字符串以及是否使用本地时间的标志)。
   * @param globals 全局节点处理参数 (可能包含帧率等信息，用于时间转换)。
   * @param table 用于存储输出值的表 (输出格式化后的时间字符串)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTimeInput;  ///< "Time" - 输入要格式化的时间值的参数键名。
  static const QString
      kFormatInput;  ///< "Format" - 时间格式化字符串的参数键名 (例如 "hh:mm:ss.zzz" 或 "%Y-%M-%DT%h:%m:%s")。
  static const QString kLocalTimeInput;  ///< "LocalTime" - 是否将输入时间解释为本地时间并据此格式化的布尔参数键名。
};

}  // namespace olive

#endif  // TIMEFORMAT_H // 头文件宏定义结束