#ifndef NOISEGENERATORNODE_H  // 防止头文件被多次包含的宏定义开始
#define NOISEGENERATORNODE_H

#include "node/node.h"  // 引入基类 Node 的定义

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表“噪点生成器”的节点。
 * 该节点可以生成噪点图案，并可能将其与输入图像混合，或作为独立的噪点纹理输出。
 * 用户通常可以控制噪点的颜色和强度。
 */
class NoiseGeneratorNode : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief NoiseGeneratorNode 构造函数。
      */
     NoiseGeneratorNode();

  NODE_DEFAULT_FUNCTIONS(NoiseGeneratorNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "噪点生成器" 或 "噪点")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "生成器" (Generator) 或 "特效" (Effect) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 获取用于生成此噪点效果的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如噪点颜色、强度等)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（生成的噪点图像，或与输入混合后的图像）。
   * @param value 当前输入行数据 (可能包含基础输入图像以及噪点参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的处理后图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kBaseIn;         ///< "Base" - 基础输入纹理（图像）的参数键名，噪点可以叠加在此图像上。
  static const QString kColorInput;     ///< "Color" - 噪点颜色的参数键名（如果噪点是彩色的）。
  static const QString kStrengthInput;  ///< "Strength" - 噪点强度或不透明度的参数键名。
};

}  // namespace olive

#endif  // NOISEGENERATORNODE_H // 头文件宏定义结束