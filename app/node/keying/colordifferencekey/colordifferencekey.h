#ifndef COLORDIFFERENCEKEYNODE_H // 防止头文件被多次包含的宏定义开始
#define COLORDIFFERENCEKEYNODE_H

#include "node/node.h" // 引入基类 Node 的定义

// 可能需要的前向声明
// struct ShaderRequest; // 假设
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表“色彩差异键控”（Color Difference Key）效果的节点。
 * 这种键控方法通常基于输入图像的颜色通道（如 YUV 或 YCbCr 中的色度通道 U 和 V）
 * 与一个参考“干净背景板”（Clean Plate，不包含前景主体但与实际拍摄背景一致的图像）之间的差异来生成蒙版。
 * 它对于处理那些前景与背景颜色非常接近，或者背景颜色不均匀的情况可能比简单色度键更有效。
 */
class ColorDifferenceKeyNode : public Node {
 public: // Q_OBJECT 宏通常放在 public 访问修饰符之后的第一行，但保持原样
  /**
   * @brief ColorDifferenceKeyNode 构造函数。
   *  通常会在这里初始化色彩差异键控相关的输入参数。
   */
  ColorDifferenceKeyNode();

  NODE_DEFAULT_FUNCTIONS(ColorDifferenceKeyNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此色彩差异键控节点的名称。
   * @return QString 类型的节点名称 (例如 "色彩差异键" 或 "Color Difference Key")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此色彩差异键控节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "键控" (Keying) 或 "特效" (Effect) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此色彩差异键控节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 获取用于应用此色彩差异键控效果的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest& request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（应用色彩差异键控处理后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像、参考背景板以及其他键控参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的处理后图像或蒙版)。
   */
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;      ///< "Texture" - 主输入纹理（要进行键控的图像）的参数键名。
  static const QString kGarbageMatteInput; ///< "GarbageMatte" - 垃圾蒙版输入参数键名，用于手动指定需要强制保留或移除的区域。
  static const QString kCoreMatteInput;    ///< "CoreMatte" - 核心蒙版输入参数键名，用于手动指定需要强制设为不透明的区域。
  static const QString kColorInput;        ///< "CleanPlate" 或 "ReferenceColor" - 参考背景板（干净的背景图像）或参考颜色的参数键名，用于计算颜色差异。
  static const QString kShadowsInput;      ///< "Shadows" - 阴影处理相关的参数键名。
  static const QString kHighlightsInput;   ///< "Highlights" - 高光处理相关的参数键名。
  static const QString kMaskOnlyInput;     ///< "MaskOnly" - 是否仅输出生成的蒙版而不是应用键控后的图像的布尔参数键名。
};
// Q_OBJECT 宏应该在类定义内部，但此处保持原样
}  // namespace olive

#endif  // COLORDIFFERENCEKEYNODE_H // 头文件宏定义结束