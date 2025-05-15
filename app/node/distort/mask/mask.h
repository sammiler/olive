#ifndef MASKDISTORTNODE_H
#define MASKDISTORTNODE_H

#include "node/generator/polygon/polygon.h" // 引入基类 PolygonGenerator 的定义

namespace olive {

/**
 * @brief 代表“蒙版”效果的节点，使用多边形形状。
 * 该节点通过一个用户定义的多边形来创建一个遮罩，可以用于显示或隐藏图像的特定区域。
 * 它继承自 PolygonGenerator，表明其核心是一个多边形生成器。
 */
class MaskDistortNode : public PolygonGenerator {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief MaskDistortNode 构造函数。
   */
  MaskDistortNode();

  NODE_DEFAULT_FUNCTIONS(MaskDistortNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "蒙版")，支持国际化翻译。
   */
  [[nodiscard]] QString Name() const override { return tr("Mask"); }

  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID (例如 "org.olivevideoeditor.Olive.mask")。
   */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.mask"); }

  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "扭曲" (kCategoryDistort) 分类（蒙版常用于影响图像区域，有时归于此类）。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryDistort}; }

  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述 (例如 "应用一个多边形蒙版。")，支持国际化翻译。
   */
  [[nodiscard]] QString Description() const override { return tr("Apply a polygonal mask."); }

  /**
   * @brief 获取用于渲染此蒙版效果的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码，用于根据多边形数据生成蒙版。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值（通常是蒙版本身，或者应用了蒙版的图像）。
   *  具体行为取决于该节点是如何被使用的（例如，是作为独立的蒙版源还是直接修改输入图像的Alpha通道）。
   * @param value 当前输入行数据 (可能包含输入图像，以及多边形顶点数据和蒙版参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (输出蒙版图像或处理后的图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kInvertInput;  ///< "Invert" - 是否反转蒙版效果的布尔参数键名。
  static const QString kFeatherInput; ///< "Feather" - 蒙版边缘羽化量的参数键名。
};

}  // namespace olive

#endif  // MASKDISTORTNODE_H