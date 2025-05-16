#ifndef DROPSHADOWFILTER_H
#define DROPSHADOWFILTER_H

#include "node/node.h"  // 引入基类 Node 的定义

namespace olive {

/**
 * @brief 代表“投影”或“阴影”滤镜效果的节点。
 * 该节点可以在输入图像的下方或周围添加一个可自定义颜色、距离、角度、柔和度和不透明度的阴影。
 */
class DropShadowFilter : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief DropShadowFilter 构造函数。
      */
     DropShadowFilter();

  NODE_DEFAULT_FUNCTIONS(DropShadowFilter)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "投影")，支持国际化翻译。
   */
  [[nodiscard]] QString Name() const override { return tr("Drop Shadow"); }
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID (例如 "org.olivevideoeditor.Olive.dropshadow")。
   */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.dropshadow"); }
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "滤镜" (kCategoryFilter) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryFilter}; }
  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述 (例如 "为图像添加投影效果。")，支持国际化翻译。
   */
  [[nodiscard]] QString Description() const override { return tr("Adds a drop shadow to an image."); }

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 获取用于应用此投影效果的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如阴影颜色、距离、角度等)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（应用投影处理后的图像数据）。
   *  通常是将原始图像与生成的阴影进行混合。
   * @param value 当前输入行数据 (包含输入图像及投影参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的处理后图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;   ///< "Texture" - 输入纹理（图像，通常是带有Alpha通道的）的参数键名。
  static const QString kColorInput;     ///< "Color" - 阴影颜色的参数键名。
  static const QString kDistanceInput;  ///< "Distance" - 阴影与原图像的距离参数键名。
  static const QString kAngleInput;     ///< "Angle" - 阴影相对于原图像的角度参数键名。
  static const QString kSoftnessInput;  ///< "Softness" - 阴影边缘的柔和度（模糊程度）参数键名。
  static const QString kOpacityInput;   ///< "Opacity" - 阴影的不透明度参数键名。
  static const QString kFastInput;      ///< "Fast" - 是否使用快速（可能质量较低）阴影算法的布尔参数键名。
};

}  // namespace olive

#endif  // DROPSHADOWFILTER_H