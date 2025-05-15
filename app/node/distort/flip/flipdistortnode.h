#ifndef FLIPDISTORTNODE_H
#define FLIPDISTORTNODE_H

#include "node/node.h" // 引入基类 Node 的定义

namespace olive {

/**
 * @brief 代表“翻转”效果的节点。
 * 该节点可以将图像进行水平翻转、垂直翻转或同时进行两种翻转。
 */
class FlipDistortNode : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief FlipDistortNode 构造函数。
   */
  FlipDistortNode();

  NODE_DEFAULT_FUNCTIONS(FlipDistortNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "翻转")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "扭曲" 或 "变换" 分类。
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
   * @brief 获取用于渲染此翻转效果的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（经过翻转处理后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及翻转参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的翻转后图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;    ///< "Texture" - 输入纹理（图像）的参数键名。
  static const QString kHorizontalInput; ///< "Horizontal" - 是否进行水平翻转的布尔参数键名。
  static const QString kVerticalInput;   ///< "Vertical" - 是否进行垂直翻转的布尔参数键名。
};

}  // namespace olive

#endif  // FLIPDISTORTNODE_H