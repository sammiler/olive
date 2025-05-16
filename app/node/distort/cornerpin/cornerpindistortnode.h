#ifndef CORNERPINDISTORTNODE_H
#define CORNERPINDISTORTNODE_H

#include <QVector2D>  // Qt 二维向量类

#include "node/gizmo/point.h"    // 引入点 Gizmo (交互控件) 的定义
#include "node/gizmo/polygon.h"  // 引入多边形 Gizmo 的定义
#include "node/inputdragger.h"   // 引入输入拖动器相关定义
#include "node/node.h"           // 引入基类 Node 的定义

namespace olive {
/**
 * @brief 代表“边角定位”（Corner Pin）扭曲效果的节点。
 * 用户可以通过拖动图像的四个角点来扭曲图像，常用于将图像贴合到场景中的四边形表面。
 */
class CornerPinDistortNode : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief CornerPinDistortNode 构造函数。
      */
     CornerPinDistortNode();

  NODE_DEFAULT_FUNCTIONS(CornerPinDistortNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "边角定位")，支持国际化翻译。
   */
  [[nodiscard]] QString Name() const override { return tr("Corner Pin"); }

  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID (例如 "org.olivevideoeditor.Olive.cornerpin")。
   */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.cornerpin"); }

  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "扭曲" (kCategoryDistort) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryDistort}; }

  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述 (例如 "通过拖动角点来扭曲图像。")，支持国际化翻译。
   */
  [[nodiscard]] QString Description() const override { return tr("Distort the image by dragging the corners."); }

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值（经过边角定位扭曲后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及各角点位置参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的扭曲图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 获取用于渲染此扭曲效果的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  /**
   * @brief 更新 Gizmo (交互控件，如角点) 在屏幕上的位置。
   *  当节点参数变化或视图变化时调用。
   * @param row 当前节点的参数值。
   * @param globals 全局节点处理参数。
   */
  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  /**
   * @brief (静态工具函数) 将二维滑动条的值（通常表示相对于图像中心或某个基准点的偏移）转换为实际的像素坐标值。
   * @param value 滑动条的原始整数值。
   * @param row 当前节点的参数值，可能包含图像尺寸等信息。
   * @param resolution 图像的分辨率 (宽和高)。
   * @return QPointF 类型的像素坐标。
   */
  [[nodiscard]] static QPointF ValueToPixel(int value, const NodeValueRow &row, const QVector2D &resolution);

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput;      ///< "Texture" - 输入纹理（图像）的参数键名。
  static const QString kPerspectiveInput;  ///< "Perspective" - 是否启用透视校正的参数键名。
  static const QString kTopLeftInput;      ///< "TopLeft" - 左上角控制点的参数键名。
  static const QString kTopRightInput;     ///< "TopRight" - 右上角控制点的参数键名。
  static const QString kBottomRightInput;  ///< "BottomRight" - 右下角控制点的参数键名。
  static const QString kBottomLeftInput;   ///< "BottomLeft" - 左下角控制点的参数键名。

 protected slots:
  /**
   * @brief 当 Gizmo (交互控件，如角点) 被拖动时调用的槽函数。
   * @param x Gizmo 在画布上的新 X 坐标。
   * @param y Gizmo 在画布上的新 Y 坐标。
   * @param modifiers 当前按下的键盘修饰键 (如 Shift, Ctrl)。
   */
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  // --- Gizmo 相关私有成员变量 ---
  static const int kGizmoCornerCount = 4;                 ///< Gizmo 角点数量，固定为4。
  PointGizmo *gizmo_resize_handle_[kGizmoCornerCount]{};  ///< 指向四个角点 Gizmo (PointGizmo) 的指针数组。
  PolygonGizmo *gizmo_whole_rect_;  ///< 指向代表整个可拖动四边形区域的 Gizmo (PolygonGizmo) 的指针。
};

}  // namespace olive

#endif  // CORNERPINDISTORTNODE_H