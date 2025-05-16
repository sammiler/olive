#ifndef WAVEDISTORTNODE_H
#define WAVEDISTORTNODE_H

#include "node/node.h"  // 引入基类 Node 的定义

namespace olive {

/**
 * @brief 代表“波浪”扭曲效果的节点。
 * 该节点可以在图像上模拟正弦波或余弦波的扭曲效果，可以控制波的频率、强度、相位（演化）以及方向（水平或垂直）。
 */
class WaveDistortNode : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief WaveDistortNode 构造函数。
      */
     WaveDistortNode();

  NODE_DEFAULT_FUNCTIONS(WaveDistortNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "波浪")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "扭曲" 或 "特效" 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述)。
   */
  void Retranslate() override;

  /**
   * @brief 获取用于渲染此波浪效果的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（经过波浪扭曲处理后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及波浪效果参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的扭曲后图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;    ///< "Texture" - 输入纹理（图像）的参数键名。
  static const QString kFrequencyInput;  ///< "Frequency" - 波浪频率（单位长度内的波数）的参数键名。
  static const QString kIntensityInput;  ///< "Intensity" - 波浪振幅（强度）的参数键名。
  static const QString kEvolutionInput;  ///< "Evolution" - 波浪相位（随时间演化/动画）的参数键名。
  static const QString kVerticalInput;   ///< "Vertical" - 波浪是否为垂直方向的布尔参数键名 (若false则为水平方向)。
};

}  // namespace olive

#endif  // WAVEDISTORTNODE_H