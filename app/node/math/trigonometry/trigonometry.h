#ifndef TRIGNODE_H  // 防止头文件被多次包含的宏定义开始 (文件名是 TRIGNODE_H，类名是 TrigonometryNode)
#define TRIGNODE_H

#include "node/node.h"  // 引入基类 Node 的定义

// 可能需要的前向声明
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个“三角函数运算”节点。
 * 该节点用于对输入值执行各种三角函数运算，如正弦、余弦、正切，以及它们的反函数和双曲函数版本。
 */
class TrigonometryNode : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief TrigonometryNode 构造函数。
      *  通常会在这里初始化三角函数运算相关的输入参数（如操作类型和输入值）。
      */
     TrigonometryNode();

  NODE_DEFAULT_FUNCTIONS(TrigonometryNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此三角函数节点的名称。
   * @return QString 类型的节点名称 (例如 "三角函数" 或 "Trigonometry")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此三角函数节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "数学" (Math) 或 "工具" (Utility) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此三角函数节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名、操作类型名)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值（三角函数运算的结果）。
   *  输入值通常是一个数字（角度或弧度，取决于具体函数和实现），输出也是一个数字。
   * @param value 当前输入行数据 (包含操作类型和输入值 X)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含运算结果)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString
      kMethodIn;  ///< "Operation" 或 "Method" - 选择三角函数运算类型的参数键名 (参照下面的 Operation 枚举)。
  static const QString kXIn;  ///< "X" 或 "Input" - 输入值 (角度或弧度) 的参数键名。

 private:
  /**
   * @brief 定义支持的三角函数运算类型。
   */
  enum Operation {
    kOpSine,        ///< 正弦 (sin)
    kOpCosine,      ///< 余弦 (cos)
    kOpTangent,     ///< 正切 (tan)
    kOpArcSine,     ///< 反正弦 (asin)
    kOpArcCosine,   ///< 反余弦 (acos)
    kOpArcTangent,  ///< 反正切 (atan)
    kOpHypSine,     ///< 双曲正弦 (sinh)
    kOpHypCosine,   ///< 双曲余弦 (cosh)
    kOpHypTangent   ///< 双曲正切 (tanh)
  };
};

}  // namespace olive

#endif  // TRIGNODE_H // 头文件宏定义结束