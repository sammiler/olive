#ifndef RIPPLEDISTORTNODE_H
#define RIPPLEDISTORTNODE_H

#include "node/gizmo/point.h"  // 引入点 Gizmo (交互控件) 的定义，用于控制波纹中心
#include "node/node.h"         // 引入基类 Node 的定义

namespace olive {

/**
 * @brief 代表“波纹”扭曲效果的节点。
 * 该节点可以在图像上模拟水面波纹或类似的涟漪效果。
 */
class RippleDistortNode : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief RippleDistortNode 构造函数。
      */
     RippleDistortNode();

  NODE_DEFAULT_FUNCTIONS(RippleDistortNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "波纹")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "扭曲" 或 "特效" 分类。
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
   * @brief 获取用于渲染此波纹效果的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（经过波纹扭曲处理后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及波纹效果参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的扭曲后图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 更新 Gizmo (通常是控制波纹中心位置的交互点) 在屏幕上的位置。
   *  当节点参数变化或视图变化时调用。
   * @param row 当前节点的参数值。
   * @param globals 全局节点处理参数。
   */
  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;    ///< "Texture" - 输入纹理（图像）的参数键名。
  static const QString kEvolutionInput;  ///< "Evolution" - 波纹随时间演化（动画）的参数键名。
  static const QString kIntensityInput;  ///< "Intensity" - 波纹强度的参数键名。
  static const QString kFrequencyInput;  ///< "Frequency" - 波纹频率（疏密程度）的参数键名。
  static const QString kPositionInput;   ///< "Position" - 波纹中心位置的参数键名。
  static const QString kStretchInput;    ///< "Stretch" - 波纹在水平或垂直方向上的拉伸参数键名。

 protected slots:
  /**
   * @brief 当 Gizmo (通常是控制波纹中心位置的交互点) 被拖动时调用的槽函数。
   * @param x Gizmo 在画布上的新 X 坐标。
   * @param y Gizmo 在画布上的新 Y 坐标。
   * @param modifiers 当前按下的键盘修饰键 (如 Shift, Ctrl)。
   */
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  PointGizmo *gizmo_;  ///< 指向控制波纹中心位置的 PointGizmo 对象的指针。
};

}  // namespace olive

#endif  // RIPPLEDISTORTNODE_H