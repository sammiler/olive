#ifndef PANNODE_H
#define PANNODE_H

#include "node/node.h" // 引入节点基类定义

namespace olive {

/**
 * @class PanNode
 * @brief 音频声像节点类。
 *
 * PanNode 继承自 Node 类，用于处理音频的声像效果，
 * 即控制音频在立体声或多声道输出中的左右平衡或位置。
 */
class PanNode : public Node {
  Q_OBJECT // Qt对象的元对象系统宏，用于信号和槽等特性
 public:
  /**
   * @brief PanNode 构造函数。
   *
   * 初始化声像节点，设置其输入端口等。
   */
  PanNode();

  /**
   * @brief 宏，可能用于生成节点类常用的默认函数，例如克隆、序列化等。
   * 具体功能取决于 NODE_DEFAULT_FUNCTIONS 的定义。
   */
  NODE_DEFAULT_FUNCTIONS(PanNode)

  /**
   * @brief 获取节点的显示名称。
   * @return [[nodiscard]] QString 节点的名称，例如 "声像"。
   * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取节点的唯一标识符。
   * @return [[nodiscard]] QString 节点的ID，例如 "olive.Pan"。
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
   * 对于声像节点，此函数可能不直接输出图像或音频，而是根据输入参数（如声像位置）
   * 准备处理音频样本所需的信息。
   * @param value 当前节点输入参数的值。
   * @param globals 全局节点处理信息，如当前时间、帧率等。
   * @param table 用于存储节点输出值的表。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 处理音频样本。
   *
   * 根据输入的声像值，对输入的音频样本进行声像处理，并将结果写入输出缓冲区。
   * @param values 当前节点输入参数的值，其中应包含声像位置。
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
   * @brief 表示声像控制参数输入的静态常量字符串ID。
   */
  static const QString kPanningInput;

 private:
  /**
   * @brief 指向音频样本输入端口的指针。
   */
  NodeInput *samples_input_{};
  /**
   * @brief 指向声像控制参数输入端口的指针。
   */
  NodeInput *panning_input_{};
};

}  // namespace olive

#endif  // PANNODE_H
