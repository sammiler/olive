#ifndef DESPILLNODE_H // 防止头文件被多次包含的宏定义开始
#define DESPILLNODE_H

#include "node/color/colormanager/colormanager.h" // 引入 ColorManager 定义，溢色去除可能需要色彩管理知识
#include "node/node.h"                            // 引入基类 Node 的定义

// 可能需要的前向声明
// struct ShaderRequest; // 假设
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表“溢色去除”（Despill）效果的节点。
 * 在色度键控（例如绿幕抠像）后，背景颜色（如绿色）常常会“溢出”或反射到前景对象的边缘。
 * 此节点用于检测并中和（去除）这些不必要的颜色溢出，使前景与新背景更自然地融合。
 */
class DespillNode : public Node {
 public: // Q_OBJECT 宏通常放在 public 访问修饰符之后的第一行，但保持原样
  /**
   * @brief DespillNode 构造函数。
   *  通常会在这里初始化溢色去除相关的输入参数。
   */
  DespillNode();

  NODE_DEFAULT_FUNCTIONS(DespillNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此溢色去除节点的名称。
   * @return QString 类型的节点名称 (例如 "溢色去除" 或 "Despill")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此溢色去除节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "键控" (Keying)、"色彩" (Color) 或 "特效" (Effect) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此溢色去除节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 获取用于应用此溢色去除效果的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如要去除的颜色、方法等)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest& request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（应用溢色去除处理后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及溢色去除参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的处理后图像)。
   */
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;           ///< "Texture" - 输入纹理（通常是已键控图像）的参数键名。
  static const QString kColorInput;             ///< "SpillColor" - 要去除的溢出颜色（例如，从绿幕溢出的绿色）的参数键名。
  static const QString kMethodInput;            ///< "Method" - 选择溢色去除算法/方法的参数键名。
  static const QString kPreserveLuminanceInput; ///< "PreserveLuminance" - 在去除溢色时是否尝试保持原始亮度的布尔参数键名。
};
// Q_OBJECT 宏应该在类定义内部，但此处保持原样
}  // namespace olive

#endif  // DESPILLNODE_H // 头文件宏定义结束