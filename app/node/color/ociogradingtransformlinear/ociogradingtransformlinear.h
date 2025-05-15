#ifndef OCIOGRADINGTRANSFORMLINEARNODE_H
#define OCIOGRADINGTRANSFORMLINEARNODE_H

#include "node/color/ociobase/ociobase.h" // 引入基类 OCIOBaseNode 的定义
#include "render/colorprocessor.h"       // 引入 ColorProcessor 的定义

namespace olive {

/**
 * @brief 代表 OCIO 调色变换（线性操作）的节点。
 * 该节点允许通过线性数学运算（如对比度、偏移、曝光等）对图像进行色彩校正。
 * 这些操作通常在特定的色彩空间（通常是线性空间或场景参考空间）中进行。
 */
class OCIOGradingTransformLinearNode : public OCIOBaseNode {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief OCIOGradingTransformLinearNode 构造函数。
   */
  OCIOGradingTransformLinearNode();

  NODE_DEFAULT_FUNCTIONS(OCIOGradingTransformLinearNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "OCIO 线性调色")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，用于组织和归类节点 (例如，属于 "色彩" 或 "OCIO 调色" 分类)。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本。
   */
  void Retranslate() override;
  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   *  例如，当用户调整对比度、曝光等参数时，需要重新生成颜色处理器。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString &input, int element) override;
  /**
   * @brief 根据当前的节点输入参数（如对比度、偏移等）生成或更新内部的 OCIO 颜色处理器。
   */
  void GenerateProcessor();

  /**
   * @brief 计算并输出节点在特定时间点的值（经过调色变换后的图像数据）。
   *  此方法会使用通过 GenerateProcessor 创建的颜色处理器来处理输入图像。
   * @param value 当前输入行数据 (通常包含输入的图像/纹理)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (通常包含输出的图像/纹理)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kContrastInput;         ///< "Contrast" - 对比度参数的键名。
  static const QString kOffsetInput;           ///< "Offset" - 偏移（通常指 RGB 颜色偏移或亮度偏移）参数的键名。
  static const QString kExposureInput;         ///< "Exposure" - 曝光参数的键名。
  static const QString kSaturationInput;       ///< "Saturation" - 饱和度参数的键名。
  static const QString kPivotInput;            ///< "Pivot" - 对比度调整的轴心点参数的键名。
  static const QString kClampBlackEnableInput; ///< "ClampBlackEnable" - 是否启用暗部裁切的参数键名。
  static const QString kClampBlackInput;       ///< "ClampBlack" - 暗部裁切值的参数键名。
  static const QString kClampWhiteEnableInput; ///< "ClampWhiteEnable" - 是否启用亮部裁切的参数键名。
  static const QString kClampWhiteInput;       ///< "ClampWhite" - 亮部裁切值的参数键名。

 protected slots:
  /**
   * @brief 当 OCIO 配置发生变化时调用的槽函数。
   *  需要重新生成颜色处理器以匹配新的配置。
   */
  void ConfigChanged() override;

 private:
  /**
   * @brief 设置四分量 (Vec4) 类型的颜色输入参数。
   *  可能用于处理如 RGB 偏移（如果作为 Vec3 传入并扩展）或 RGBA 颜色。
   * @param input 要设置的输入参数的键名。
   */
  void SetVec4InputColors(const QString &input);
};

}  // namespace olive

#endif // OCIOGRADINGTRANSFORMLINEARNODE_H