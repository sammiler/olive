#ifndef TILEDISTORTNODE_H
#define TILEDISTORTNODE_H

#include "node/gizmo/point.h" // 引入点 Gizmo (交互控件) 的定义，可能用于控制平铺的中心或锚点
#include "node/node.h"        // 引入基类 Node 的定义

namespace olive {

/**
 * @brief 代表“平铺”或“瓦片”效果的节点。
 * 该节点可以将输入图像进行缩放、定位，并通过镜像等方式创建重复的平铺图案。
 * 类似于万花筒效果或者背景纹理的重复。
 */
class TileDistortNode : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief TileDistortNode 构造函数。
   */
  TileDistortNode();

  NODE_DEFAULT_FUNCTIONS(TileDistortNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "平铺")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "扭曲"、"风格化" 或 "纹理" 分类。
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
   * @brief 获取用于渲染此平铺效果的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（经过平铺处理后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及平铺效果参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的平铺图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 更新 Gizmo (可能用于控制平铺中心或锚点) 在屏幕上的位置。
   *  当节点参数变化或视图变化时调用。
   * @param row 当前节点的参数值。
   * @param globals 全局节点处理参数。
   */
  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;  ///< "Texture" - 输入纹理（图像）的参数键名。
  static const QString kScaleInput;    ///< "Scale" - 平铺单元的缩放比例参数键名。
  static const QString kPositionInput; ///< "Position" - 平铺图案的中心位置或偏移参数键名。
  static const QString kAnchorInput;   ///< "Anchor" - 平铺变换的锚点参数键名 (参照下面的 Anchor 枚举)。
  static const QString kMirrorXInput;  ///< "MirrorX" - 是否在X轴方向镜像平铺的布尔参数键名。
  static const QString kMirrorYInput;  ///< "MirrorY" - 是否在Y轴方向镜像平铺的布尔参数键名。

 protected slots:
  /**
   * @brief 当 Gizmo (可能用于控制平铺中心或锚点) 被拖动时调用的槽函数。
   * @param x Gizmo 在画布上的新 X 坐标。
   * @param y Gizmo 在画布上的新 Y 坐标。
   * @param modifiers 当前按下的键盘修饰键 (如 Shift, Ctrl)。
   */
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  /**
   * @brief 定义平铺变换的锚点位置。
   * 锚点决定了缩放和定位变换的参考中心。
   */
  enum Anchor {
    kTopLeft,      ///< 左上角锚点
    kTopCenter,    ///< 顶部中心锚点
    kTopRight,     ///< 右上角锚点
    kMiddleLeft,   ///< 中部左侧锚点
    kMiddleCenter, ///< 中心锚点
    kMiddleRight,  ///< 中部右侧锚点
    kBottomLeft,   ///< 左下角锚点
    kBottomCenter, ///< 底部中心锚点
    kBottomRight   ///< 右下角锚点
  };

  PointGizmo *gizmo_; ///< 指向控制平铺效果某个关键点（如中心或锚点）的 PointGizmo 对象的指针。
};

}  // namespace olive

#endif  // TILEDISTORTNODE_H