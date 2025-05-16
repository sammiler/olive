#ifndef BLURFILTERNODE_H
#define BLURFILTERNODE_H

#include "node/gizmo/point.h"  // 引入点 Gizmo (交互控件) 的定义，用于径向模糊的中心点
#include "node/node.h"         // 引入基类 Node 的定义

namespace olive {

/**
 * @brief 代表“模糊”滤镜效果的节点。
 * 该节点可以对输入图像应用多种类型的模糊，如盒状模糊、高斯模糊、定向模糊和径向模糊。
 */
class BlurFilterNode : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief BlurFilterNode 构造函数。
      */
     BlurFilterNode();

  /**
   * @brief 定义模糊算法的类型。
   */
  enum Method {
    kBox,          ///< 盒状模糊 (Box Blur)
    kGaussian,     ///< 高斯模糊 (Gaussian Blur)
    kDirectional,  ///< 定向模糊 (Directional Blur)
    kRadial        ///< 径向模糊 (Radial Blur)
  };

  NODE_DEFAULT_FUNCTIONS(BlurFilterNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "模糊")。
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
   * @brief 获取用于应用此模糊效果的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如模糊类型、半径等)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（应用模糊处理后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及模糊参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的模糊后图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 获取当前选择的模糊方法/类型。
   * @return Method 枚举值，表示当前的模糊算法。
   */
  [[nodiscard]] Method GetMethod() const { return static_cast<Method>(GetStandardValue(kMethodInput).toInt()); }

  /**
   * @brief 更新 Gizmo (例如径向模糊的中心点) 在屏幕上的位置。
   *  当节点参数变化或视图变化时调用。
   * @param row 当前节点的参数值。
   * @param globals 全局节点处理参数。
   */
  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;  ///< "Texture" - 输入纹理（图像）的参数键名。
  static const QString kMethodInput;   ///< "Method" - 选择模糊方法（类型）的参数键名。
  static const QString kRadiusInput;   ///< "Radius" - 模糊半径（通用）的参数键名。
  static const QString kHorizInput;  ///< "HorizontalRadius" - 水平模糊半径的参数键名 (某些模糊类型如盒状可能分别控制)。
  static const QString kVertInput;   ///< "VerticalRadius" - 垂直模糊半径的参数键名 (某些模糊类型如盒状可能分别控制)。
  static const QString
      kRepeatEdgePixelsInput;  ///< "RepeatEdgePixels" - 是否重复边缘像素以避免模糊边缘变暗的布尔参数键名。

  static const QString kDirectionalDegreesInput;  ///< "DirectionalDegrees" - 定向模糊的角度参数键名。

  static const QString kRadialCenterInput;  ///< "RadialCenter" - 径向模糊的中心点位置参数键名。

 protected slots:
  /**
   * @brief 当 Gizmo (例如径向模糊的中心点) 被拖动时调用的槽函数。
   * @param x Gizmo 在画布上的新 X 坐标。
   * @param y Gizmo 在画布上的新 Y 坐标。
   * @param modifiers 当前按下的键盘修饰键 (如 Shift, Ctrl)。
   */
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 protected:
  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   *  例如，当模糊方法改变时，可能需要更新其他相关参数的可见性或可用性。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString &input, int element) override;

 private:
  /**
   * @brief 根据选择的模糊方法更新节点的输入参数（例如，显示/隐藏特定于某种模糊类型的参数）。
   * @param method 当前选择的模糊方法。
   */
  void UpdateInputs(Method method);

  PointGizmo *radial_center_gizmo_;  ///< 指向控制径向模糊中心点的 PointGizmo 对象的指针。
};

}  // namespace olive

#endif  // BLURFILTERNODE_H