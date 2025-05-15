#ifndef MATHNODEBASE_H // 防止头文件被多次包含的宏定义开始
#define MATHNODEBASE_H

#include "node/node.h" // 引入基类 Node 的定义

// 可能需要的前向声明
// class NodeValueTable; // 假设
// class NodeValue; // 假设
// class SampleBuffer; // 假设
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class QVector4D; // 假设
// struct ShaderCode; // 假设
// namespace NodeValue { enum class Type; } // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 所有执行数学运算节点的基类。
 * 此类定义了支持的数学运算类型 (Operation)，以及处理不同数据类型之间运算的逻辑 (Pairing)。
 * 它还包含用于在 CPU 和 GPU (通过着色器代码) 上执行这些运算的辅助方法。
 */
class MathNodeBase : public Node {
 public: // Q_OBJECT 宏通常在此，但如果 MathNodeBase 自身不需要信号槽，则可以省略
  /**
   * @brief MathNodeBase 默认构造函数。
   */
  MathNodeBase() = default;

  /**
   * @brief 定义支持的数学运算类型。
   */
  enum Operation {
    kOpAdd,      ///< 加法运算
    kOpSubtract, ///< 减法运算
    kOpMultiply, ///< 乘法运算
    kOpDivide,   ///< 除法运算
    kOpPower     ///< 幂运算
  };

  /**
   * @brief (静态工具函数) 根据 Operation 枚举值获取其对应的可读名称字符串。
   * @param o Operation 枚举值。
   * @return QString 运算名称 (例如 "Add", "Multiply")。
   */
  static QString GetOperationName(Operation o);

 protected:
  /**
   * @brief 定义不同数据类型之间的运算配对。
   *  用于确定两个输入值应如何进行数学运算。
   */
  enum Pairing {
    kPairNone = -1, ///< 无效或未确定的配对

    kPairNumberNumber,   ///< 数字 - 数字
    kPairVecVec,         ///< 向量 - 向量
    kPairMatrixMatrix,   ///< 矩阵 - 矩阵
    kPairColorColor,     ///< 颜色 - 颜色
    kPairTextureTexture, ///< 纹理 - 纹理 (通常是逐像素运算)

    kPairVecNumber,       ///< 向量 - 数字 (数字通常会广播到向量的每个分量)
    kPairMatrixVec,       ///< 矩阵 - 向量 (矩阵乘法)
    kPairNumberColor,     ///< 数字 - 颜色
    kPairTextureNumber,   ///< 纹理 - 数字
    kPairTextureColor,    ///< 纹理 - 颜色
    kPairTextureMatrix,   ///< 纹理 - 矩阵 (可能用于变换纹理坐标)
    kPairSampleSample,    ///< 音频采样 - 音频采样
    kPairSampleNumber,    ///< 音频采样 - 数字

    kPairCount ///< 配对类型的总数，用于迭代或数组大小
  };

  /**
   * @brief 一个内部辅助类，用于根据两个输入表 (NodeValueTable) 推断最可能的运算配对类型。
   */
  class PairingCalculator {
   public:
    /**
     * @brief PairingCalculator 构造函数。
     * @param table_a 第一个输入值表。
     * @param table_b 第二个输入值表。
     */
    PairingCalculator(const NodeValueTable& table_a, const NodeValueTable& table_b);

    /** @brief 检查是否成功找到了最可能的配对。 */
    [[nodiscard]] bool FoundMostLikelyPairing() const;
    /** @brief 获取推断出的最可能的配对类型。 */
    [[nodiscard]] Pairing GetMostLikelyPairing() const;

    /** @brief 获取与最可能配对对应的第一个输入值。 */
    [[nodiscard]] const NodeValue& GetMostLikelyValueA() const;
    /** @brief 获取与最可能配对对应的第二个输入值。 */
    [[nodiscard]] const NodeValue& GetMostLikelyValueB() const;

   private:
    /** @brief (静态工具函数) 根据输入值表获取各种配对类型的可能性（或权重）。 */
    static QVector<int> GetPairLikelihood(const NodeValueTable& table);

    Pairing most_likely_pairing_; ///< 存储推断出的最可能的配对类型。
    NodeValue most_likely_value_a_; ///< 存储与最可能配对对应的第一个输入值。
    NodeValue most_likely_value_b_; ///< 存储与最可能配对对应的第二个输入值。
  };

  // --- 以下是一系列模板函数，用于在不同数据类型和不同操作组合下执行数学运算 ---
  // T 和 U 通常代表参与运算的数据类型（如 float, QVector4D, QMatrix4x4, QColor）

  /** @brief 对类型 T 和 U 的操作数 a 和 b 执行所有支持的数学运算 (加、减、乘、除、幂)。 */
  template <typename T, typename U>
  static T PerformAll(Operation operation, T a, U b);

  /** @brief 对类型 T 和 U 的操作数 a 和 b 执行乘法或除法运算。 */
  template <typename T, typename U>
  static T PerformMultDiv(Operation operation, T a, U b);

  /** @brief 对类型 T 和 U 的操作数 a 和 b 执行加法或减法运算。 */
  template <typename T, typename U>
  static T PerformAddSub(Operation operation, T a, U b);

  /** @brief 对类型 T 和 U 的操作数 a 和 b 执行乘法运算。 */
  template <typename T, typename U>
  static T PerformMult(Operation operation, T a, U b);

  /** @brief 对类型 T 和 U 的操作数 a 和 b 执行加法、减法或乘法运算。 */
  template <typename T, typename U>
  static T PerformAddSubMult(Operation operation, T a, U b);

  /** @brief 对类型 T 和 U 的操作数 a 和 b 执行加法、减法、乘法或除法运算。 */
  template <typename T, typename U>
  static T PerformAddSubMultDiv(Operation operation, T a, U b);

  /**
   * @brief 对浮点数缓冲区中的一段数据执行所有支持的数学运算。
   * @param operation 要执行的运算。
   * @param a 指向浮点数缓冲区的指针（此缓冲区会被修改）。
   * @param b 第二个操作数（浮点数）。
   * @param start 缓冲区中开始处理的索引。
   * @param end 缓冲区中结束处理的索引（不包含）。
   */
  static void PerformAllOnFloatBuffer(Operation operation, float* a, float b, int start, int end);

#if defined(Q_PROCESSOR_X86) || defined(Q_PROCESSOR_ARM) // 针对特定处理器架构的 SSE 优化版本
  /**
   * @brief (SSE优化版) 对浮点数缓冲区中的一段数据执行所有支持的数学运算。
   */
  static void PerformAllOnFloatBufferSSE(Operation operation, float* a, float b, int start, int end);
#endif

  /**
   * @brief (静态工具函数) 根据 NodeValue::Type 获取其在着色器代码中对应的 uniform 类型字符串。
   * @param type NodeValue 的数据类型。
   * @return QString 着色器 uniform 类型名 (例如 "float", "vec4", "sampler2D")。
   */
  static QString GetShaderUniformType(const NodeValue::Type& type);

  /**
   * @brief (静态工具函数) 生成在着色器代码中调用（访问）特定输入参数值的代码片段。
   * @param input_id 输入参数的 ID (通常是 uniform 变量名)。
   * @param type 输入参数的数据类型。
   * @param coord_op (可选) 对纹理坐标进行操作的代码片段 (例如，用于纹理采样的变换)。
   * @return QString 生成的着色器代码片段。
   */
  static QString GetShaderVariableCall(const QString& input_id, const NodeValue::Type& type,
                                       const QString& coord_op = QString());

  /** @brief (静态工具函数) 从 NodeValue 对象中提取一个 QVector4D 值 (如果类型兼容)。 */
  static QVector4D RetrieveVector(const NodeValue& val);

  /** @brief (静态工具函数) 从 NodeValue 对象中提取一个 float 值 (如果类型兼容)。 */
  static float RetrieveNumber(const NodeValue& val);

  /**
   * @brief (静态工具函数) 检查给定的数字操作数对于特定运算是否是“无操作”的。
   *  例如，乘以 1 或加上 0 通常是无操作。
   * @param op 数学运算类型。
   * @param number 数字操作数。
   * @return 如果是无操作则返回 true，否则返回 false。
   */
  static bool NumberIsNoOp(const Operation& op, const float& number);

  /**
   * @brief (静态保护函数) 获取用于数学运算的内部着色器代码。
   * @param shader_id 着色器的唯一标识符。
   * @param param_a_in 第一个参数在着色器中的输入名称。
   * @param param_b_in 第二个参数在着色器中的输入名称。
   * @return ShaderCode 对象。
   */
  [[nodiscard]] static ShaderCode GetShaderCodeInternal(const QString& shader_id, const QString& param_a_in,
                                                        const QString& param_b_in);

  /**
   * @brief 将一个 QVector4D 值根据指定的类型推送到输出表 (NodeValueTable)。
   * @param output 指向目标输出表的指针。
   * @param type 输出值的期望类型 (例如，如果 vec 是颜色，type 可能是 NodeValue::kColorType)。
   * @param vec 要推送的 QVector4D 值。
   */
  void PushVector(NodeValueTable* output, NodeValue::Type type, const QVector4D& vec) const;

  /**
   * @brief (保护函数) 执行数学运算的核心逻辑。
   *  根据推断的配对类型和选择的运算，对输入值 val_a 和 val_b 进行计算，并将结果放入 output 表。
   * @param operation 要执行的数学运算。
   * @param pairing 推断出的操作数配对类型。
   * @param param_a_in 第一个操作数参数的名称（用于错误报告或调试）。
   * @param val_a 第一个操作数的值。
   * @param param_b_in 第二个操作数参数的名称。
   * @param val_b 第二个操作数的值。
   * @param globals 全局节点处理参数。
   * @param output 指向存储运算结果的输出表的指针。
   */
  void ValueInternal(Operation operation, Pairing pairing, const QString& param_a_in, const NodeValue& val_a,
                     const QString& param_b_in, const NodeValue& val_b, const NodeGlobals& globals,
                     NodeValueTable* output) const;

  /**
   * @brief (静态保护函数) 处理音频采样的内部逻辑。
   *  对输入音频缓冲区的样本执行指定的数学运算。
   * @param values 包含操作数参数值的 NodeValueRow。
   * @param operation 要执行的数学运算。
   * @param param_a_in 第一个操作数参数的名称。
   * @param param_b_in 第二个操作数参数的名称。
   * @param input 输入的音频采样缓冲区。
   * @param output 输出的音频采样缓冲区，存储处理结果。
   * @param index 当前处理的音频通道或样本索引。
   */
  static void ProcessSamplesInternal(const NodeValueRow& values, Operation operation, const QString& param_a_in,
                                     const QString& param_b_in, const SampleBuffer& input, SampleBuffer& output,
                                     int index);
};

}  // namespace olive

#endif  // MATHNODEBASE_H // 头文件宏定义结束