#ifndef OPACITYEFFECT_H
#define OPACITYEFFECT_H

#include "node/group/group.h"  // 尽管包含了 group.h，但 OpacityEffect 直接继承自 Node
                               // 可能是为了某些类型定义或项目结构需要

namespace olive {

/**
 * @brief 代表“不透明度”效果的节点。
 * 该节点用于改变输入图像（纹理）的整体不透明度。
 */
class OpacityEffect : public Node {  // 直接继承自 Node，而不是 Group
 public:
  /**
   * @brief OpacityEffect 构造函数。
   */
  OpacityEffect();

  NODE_DEFAULT_FUNCTIONS(OpacityEffect)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "不透明度")，支持国际化翻译。
   */
  [[nodiscard]] QString Name() const override { return tr("Opacity"); }

  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID (例如 "org.olivevideoeditor.Olive.opacity")。
   */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.opacity"); }

  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "滤镜" (kCategoryFilter) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryFilter}; }

  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述 (例如
   * "改变视频的不透明度。\n\n这等效于将视频乘以一个介于0.0和1.0之间的数字。")，支持国际化翻译。
   */
  [[nodiscard]] QString Description() const override {
    return tr("Alter a video's opacity.\n\nThis is equivalent to multiplying a video by a number between 0.0 and 1.0.");
  }

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述)。
   */
  void Retranslate() override;

  /**
   * @brief 获取用于应用此不透明度效果的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码（通常是片段着色器修改alpha值）。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（应用不透明度调整后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及不透明度值)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的处理后图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;  ///< "Texture" - 输入纹理（图像）的参数键名。
  static const QString kValueInput;    ///< "Value" - 不透明度值（通常为 0.0 到 1.0）的参数键名。
};

}  // namespace olive

#endif  // OPACITYEFFECT_H