#ifndef MATHNODE_H // 防止头文件被多次包含的宏定义开始
#define MATHNODE_H

#include "mathbase.h" // 引入基类 MathNodeBase 的定义

// 可能需要的前向声明
// struct ShaderRequest; // 假设
// enum class Operation; // 假设 Operation 枚举在 MathNodeBase 中或全局定义
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设
// class SampleBuffer; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表一个通用的“数学运算”节点。
 * 该节点可以对输入参数（通常是数字、向量或颜色）执行多种数学运算，
 * 例如加、减、乘、除、幂、三角函数等。
 * 它继承自 MathNodeBase，后者可能定义了操作类型枚举和通用的处理逻辑。
 */
class MathNode : public MathNodeBase {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief MathNode 构造函数。
   *  通常会在这里初始化数学运算相关的输入参数（如操作类型、操作数A、B、C）。
   */
  MathNode();

  NODE_DEFAULT_FUNCTIONS(MathNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此数学节点的名称。
   * @return QString 类型的节点名称 (例如 "数学运算" 或 "Math")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此数学节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "数学" (Math) 或 "工具" (Utility) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此数学节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名、操作类型名)。
   */
  void Retranslate() override;

  /**
   * @brief 获取用于执行此数学运算的着色器代码（如果运算是在 GPU 上进行的）。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  /**
   * @brief 获取当前选择的数学运算类型。
   *  Operation 是一个枚举类型，定义在 MathNodeBase 或此文件中，列出所有支持的数学运算。
   * @return Operation 枚举值。
   */
  [[nodiscard]] Operation GetOperation() const { return static_cast<Operation>(GetStandardValue(kMethodIn).toInt()); }

  /**
   * @brief 设置要执行的数学运算类型。
   * @param o新的 Operation 枚举值。
   */
  void SetOperation(Operation o) { SetStandardValue(kMethodIn, static_cast<int>(o)); } // 假设 Operation 可以转为 int

  /**
   * @brief 计算并输出节点在特定时间点的值（数学运算的结果）。
   *  结果的数据类型取决于输入参数和所选操作。
   * @param value 当前输入行数据 (包含操作类型和操作数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含运算结果)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 处理音频采样数据。
   *  如果此数学节点也设计用于处理音频数据，此方法将对输入的音频样本执行选定的数学运算。
   * @param values 当前节点的参数值 (包含操作类型和操作数)。
   * @param input 输入的音频采样缓冲区。
   * @param output 输出的音频采样缓冲区，存储处理后的结果。
   * @param index 当前处理的音频通道或样本索引（如果适用）。
   */
  void ProcessSamples(const NodeValueRow &values, const SampleBuffer &input, SampleBuffer &output,
                      int index) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kMethodIn; ///< "Operation" 或 "Method" - 选择数学运算类型的参数键名。
  static const QString kParamAIn; ///< "A" 或 "InputA" - 第一个操作数 (参数A) 的参数键名。
  static const QString kParamBIn; ///< "B" 或 "InputB" - 第二个操作数 (参数B) 的参数键名。
  static const QString kParamCIn; ///< "C" 或 "InputC" - 第三个操作数 (参数C) 的参数键名 (某些运算可能需要)。
};

}  // namespace olive

#endif  // MATHNODE_H // 头文件宏定义结束