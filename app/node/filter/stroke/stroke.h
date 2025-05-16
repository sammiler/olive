#ifndef STROKEFILTERNODE_H  // 防止头文件被多次包含的宏定义开始
#define STROKEFILTERNODE_H

#include "node/node.h"  // 引入基类 Node 的定义

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表“描边”滤镜效果的节点。
 * 该节点可以在输入图像（通常是带有 Alpha 通道的图像，如文本或形状）的边缘添加一个可自定义颜色、宽度和不透明度的描边。
 * 描边可以在对象内部、外部或居中。
 */
class StrokeFilterNode : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief StrokeFilterNode 构造函数。
      */
     StrokeFilterNode();

  NODE_DEFAULT_FUNCTIONS(StrokeFilterNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "描边")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "滤镜" (Filter) 分类。
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
   * @brief 计算并输出节点在特定时间点的值（应用描边处理后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及描边参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的处理后图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;
  /**
   * @brief 获取用于应用此描边效果的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如描边颜色、宽度等)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;  ///< "Texture" - 输入纹理（图像，通常需要Alpha通道）的参数键名。
  static const QString kColorInput;    ///< "Color" - 描边颜色的参数键名。
  static const QString kRadiusInput;   ///< "Radius" - 描边宽度（半径）的参数键名。
  static const QString kOpacityInput;  ///< "Opacity" - 描边不透明度的参数键名。
  static const QString kInnerInput;    ///< "Inner" - 描边是否在对象内部的布尔参数键名
                                       ///< (若为false，则可能表示外部或居中描边，具体取决于实现)。
};

}  // namespace olive

#endif  // STROKEFILTERNODE_H // 头文件宏定义结束