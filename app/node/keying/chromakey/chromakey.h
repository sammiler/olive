#ifndef CHROMAKEYNODE_H // 防止头文件被多次包含的宏定义开始
#define CHROMAKEYNODE_H

#include "node/color/ociobase/ociobase.h" // 引入基类 OCIOBaseNode 的定义，表明此节点与 OCIO 色彩管理相关

// 可能需要的前向声明
// struct ShaderRequest; // 假设
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表“色度键控”（Chroma Key）效果的节点。
 * 该节点用于移除图像中特定颜色（通常是绿色或蓝色背景），使其变为透明，
 * 从而可以将前景对象叠加到其他背景上。它继承自 OCIOBaseNode，
 * 可能意味着键控过程中的颜色处理会考虑色彩空间。
 */
class ChromaKeyNode : public OCIOBaseNode {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief ChromaKeyNode 构造函数。
   *  通常会在这里初始化色度键控相关的输入参数。
   */
  ChromaKeyNode();

  NODE_DEFAULT_FUNCTIONS(ChromaKeyNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此色度键控节点的名称。
   * @return QString 类型的节点名称 (例如 "色度键" 或 "Chroma Key")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此色度键控节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "键控" (Keying) 或 "特效" (Effect) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此色度键控节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   *  例如，当键控颜色或容差改变时，需要重新生成颜色处理器。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString& input, int element) override;

  /**
   * @brief 获取用于应用此色度键控效果的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如键控颜色、容差等)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest& request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（应用色度键控处理后的图像数据）。
   *  通常是原始图像的Alpha通道被修改，或者输出一个表示蒙版的图像。
   * @param value 当前输入行数据 (包含输入图像及键控参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的处理后图像或蒙版)。
   */
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  /**
   * @brief 当 OCIO 配置发生变化时调用的槽函数 (重写自 OCIOBaseNode)。
   *  需要重新生成颜色处理器以适应新的色彩配置。
   */
  void ConfigChanged() override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kColorInput;          ///< "KeyColor" - 要移除的键控颜色（例如，绿色屏幕的绿色）的参数键名。
  static const QString kInvertInput;         ///< "Invert" - 是否反转键控结果（即保留键控颜色，移除其他部分）的布尔参数键名。
  static const QString kMaskOnlyInput;       ///< "MaskOnly" - 是否仅输出生成的蒙版（灰度图）而不是应用键控后的图像的布尔参数键名。
  static const QString kUpperToleranceInput; ///< "UpperTolerance" - 上容差（或相似颜色范围的上限）参数键名，用于控制键控的敏感度。
  static const QString kLowerToleranceInput; ///< "LowerTolerance" - 下容差（或相似颜色范围的下限）参数键名。
  static const QString kGarbageMatteInput;   ///< "GarbageMatte" - 垃圾蒙版输入参数键名，用于手动指定需要强制保留或移除的区域。
  static const QString kCoreMatteInput;      ///< "CoreMatte" - 核心蒙版输入参数键名，用于手动指定需要强制设为不透明的区域。
  static const QString kShadowsInput;        ///< "Shadows" - 阴影处理相关的参数键名，可能用于调整从背景拾取的阴影。
  static const QString kHighlightsInput;     ///< "Highlights" - 高光处理相关的参数键名，可能用于调整从背景拾取的高光或溢色。

 private:
  /**
   * @brief 根据当前的节点输入参数（如键控颜色、容差等）生成或更新内部的颜色处理器 (ColorProcessor)。
   *  对于色度键控，这个处理器将执行实际的颜色比较和蒙版生成逻辑。
   */
  void GenerateProcessor();
};

}  // namespace olive

#endif  // CHROMAKEYNODE_H // 头文件宏定义结束