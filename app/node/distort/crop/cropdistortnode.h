#ifndef CROPDISTORTNODE_H
#define CROPDISTORTNODE_H

#include <QVector2D> // Qt 二维向量类

#include "node/gizmo/point.h"    // 引入点 Gizmo (交互控件) 的定义
#include "node/gizmo/polygon.h"  // 引入多边形 Gizmo 的定义
#include "node/inputdragger.h"   // 引入输入拖动器相关定义
#include "node/node.h"           // 引入基类 Node 的定义

namespace olive {

/**
 * @brief 代表“裁剪”效果的节点。
 * 用户可以通过调整上下左右边界来裁剪图像的边缘，也可以选择羽化边缘。
 */
class CropDistortNode : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief CropDistortNode 构造函数。
   */
  CropDistortNode();

  NODE_DEFAULT_FUNCTIONS(CropDistortNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "裁剪")，支持国际化翻译。
   */
  [[nodiscard]] QString Name() const override { return tr("Crop"); }

  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID (例如 "org.olivevideoeditor.Olive.crop")。
   */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.crop"); }

  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "扭曲" (kCategoryDistort) 分类（尽管裁剪更像是变换，但有时归类于此）。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryDistort}; }

  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述 (例如 "裁剪图像的边缘。")，支持国际化翻译。
   */
  [[nodiscard]] QString Description() const override { return tr("Crop the edges of an image."); }

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值（经过裁剪处理后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及裁剪参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的裁剪后图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 获取用于渲染此裁剪效果的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  /**
   * @brief 更新 Gizmo (交互控件，如裁剪框的边缘和角点) 在屏幕上的位置。
   *  当节点参数变化或视图变化时调用。
   * @param row 当前节点的参数值。
   * @param globals 全局节点处理参数。
   */
  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput; ///< "Texture" - 输入纹理（图像）的参数键名。
  static const QString kLeftInput;    ///< "Left" - 左边界裁剪量的参数键名。
  static const QString kTopInput;     ///< "Top" - 上边界裁剪量的参数键名。
  static const QString kRightInput;   ///< "Right" - 右边界裁剪量的参数键名。
  static const QString kBottomInput;  ///< "Bottom" - 下边界裁剪量的参数键名。
  static const QString kFeatherInput; ///< "Feather" - 边缘羽化量的参数键名。

 protected slots:
  /**
   * @brief 当 Gizmo (交互控件，如裁剪框的边缘) 被拖动时调用的槽函数。
   * @param x_diff Gizmo 在画布上 X 方向的移动差量。
   * @param y_diff Gizmo 在画布上 Y 方向的移动差量。
   * @param modifiers 当前按下的键盘修饰键 (如 Shift, Ctrl)。
   */
  void GizmoDragMove(double x_diff, double y_diff, const Qt::KeyboardModifiers &modifiers) override;

 private:
  /**
   * @brief 创建用于控制裁剪边（上、下、左、右）的输入参数。
   * @param id 裁剪边参数的唯一标识符（例如，对应于 kLeftInput, kTopInput 等）。
   */
  void CreateCropSideInput(const QString &id);

  // --- Gizmo 相关私有成员变量 ---
  // static const int kGizmoScaleCount = 8; // 假设 kGizmoScaleCount 在 .cpp 文件中定义，表示 Gizmo 句柄数量
  PointGizmo *point_gizmo_[/*kGizmoScaleCount*/8]{}; ///< 指向各个 Gizmo 控制点 (PointGizmo) 的指针数组。数组大小可能在.cpp中定义或硬编码。
                                                   ///< 通常裁剪框有8个控制点（四角+四边中点）。
  PolygonGizmo *poly_gizmo_;                         ///< 指向代表整个裁剪区域的 Gizmo (PolygonGizmo) 的指针。
  QVector2D temp_resolution_;                        ///< 临时存储图像分辨率的变量，可能用于 Gizmo 计算。
};

}  // namespace olive

#endif  // CROPDISTORTNODE_H