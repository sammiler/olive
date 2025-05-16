#ifndef VOLUMENODE_H
#define VOLUMENODE_H

#include "node/math/math/mathbase.h"  // 引入数学节点基类定义

namespace olive {

/**
 * @class VolumeNode
 * @brief 音频音量节点类。
 *
 * VolumeNode 继承自 MathNodeBase (数学节点基类的一个特化，可能用于音频处理)，
 * 用于调整音频信号的音量大小。
 */
class VolumeNode : public MathNodeBase {
 Q_OBJECT  // Qt对象的元对象系统宏，用于信号和槽等特性
     public :
     /**
      * @brief VolumeNode 构造函数。
      *
      * 初始化音量节点，设置其输入端口（如音频样本输入、音量控制输入）等。
      */
     VolumeNode();

  /**
   * @brief 宏，可能用于生成节点类常用的默认函数，例如克隆、序列化等。
   * 具体功能取决于 NODE_DEFAULT_FUNCTIONS 的定义。
   */
  NODE_DEFAULT_FUNCTIONS(VolumeNode)

  /**
   * @brief 获取节点的显示名称。
   * @return [[nodiscard]] QString 节点的名称，例如 "音量"。
   * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取节点的唯一标识符。
   * @return [[nodiscard]] QString 节点的ID，例如 "olive.Volume"。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取节点所属的类别ID列表。
   * @return [[nodiscard]] QVector<CategoryID> 节点类别ID的向量。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取节点的描述信息。
   * @return [[nodiscard]] QString 节点的描述文本。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 计算节点在特定时间点的值。
   *
   * 对于音量节点，此函数可能不直接输出音频，而是根据输入参数（如音量大小）
   * 准备处理音频样本所需的信息或直接传递音量值。
   * @param value 当前节点输入参数的值。
   * @param globals 全局节点处理信息，如当前时间、帧率等。
   * @param table 用于存储节点输出值的表。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 处理音频样本。
   *
   * 根据输入的音量值，对输入的音频样本进行音量调整，并将结果写入输出缓冲区。
   * @param values 当前节点输入参数的值，其中应包含音量大小。
   * @param input 输入的音频样本缓冲区。
   * @param output 输出的音频样本缓冲区。
   * @param index 当前处理的样本帧索引或时间点。
   */
  void ProcessSamples(const NodeValueRow &values, const SampleBuffer &input, SampleBuffer &output,
                      int index) const override;

  /**
   * @brief 重新翻译节点的名称和描述等用户可见的文本。
   *
   * 当应用程序语言更改时调用此函数。
   */
  void Retranslate() override;

  /**
   * @brief 表示音频样本输入的静态常量字符串ID。
   */
  static const QString kSamplesInput;
  /**
   * @brief 表示音量控制参数输入的静态常量字符串ID。
   */
  static const QString kVolumeInput;

  // 注意：此类的私有成员变量（如 NodeInput 指针）没有在头文件中声明，
  // 这通常意味着它们在基类 MathNodeBase 中或者在 .cpp 文件中以某种方式处理。
  // 如果它们在 .cpp 文件中定义为私有成员，则注释会在 .cpp 文件中添加。
};

}  // namespace olive

#endif  // VOLUMENODE_H
