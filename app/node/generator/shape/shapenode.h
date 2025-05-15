#ifndef SHAPENODE_H // 防止头文件被多次包含的宏定义开始
#define SHAPENODE_H

#include "shapenodebase.h" // 引入基类 ShapeNodeBase 的定义

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表一个通用形状生成器节点，可以生成多种基本形状如矩形、椭圆等。
 * 继承自 ShapeNodeBase，提供了生成和渲染这些形状的基础功能。
 */
class ShapeNode : public ShapeNodeBase {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief ShapeNode 构造函数。
   */
  ShapeNode();

  /**
   * @brief 定义可生成的形状类型。
   */
  enum Type {
    kRectangle,        ///< 矩形
    kEllipse,          ///< 椭圆
    kRoundedRectangle  ///< 圆角矩形
  };

  NODE_DEFAULT_FUNCTIONS(ShapeNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "形状")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "生成器" (Generator) 或 "形状" (Shape) 分类。
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
   * @brief 获取用于渲染当前选定形状的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如形状类型、尺寸、颜色等)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（生成的形状图像）。
   * @param value 当前输入行数据 (包含形状类型、尺寸、圆角半径等参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的形状图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static QString kTypeInput;   ///< "Type" - 选择形状类型 (参照 Type 枚举) 的参数键名。
  static QString kRadiusInput; ///< "Radius" - 圆角矩形的圆角半径参数键名。

 protected:
  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   *  例如，当形状类型改变时，可能需要显示/隐藏特定于该形状的参数（如圆角半径）。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString &input, int element) override;
};

}  // namespace olive

#endif  // SHAPENODE_H // 头文件宏定义结束