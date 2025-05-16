#ifndef SHAPENODEBASE_H  // 防止头文件被多次包含的宏定义开始
#define SHAPENODEBASE_H

#include "generatorwithmerge.h"  // 引入基类 GeneratorWithMerge，表明形状可以与基础输入合并
#include "node/gizmo/point.h"    // 引入点 Gizmo (交互控件) 的定义
#include "node/gizmo/polygon.h"  // 引入多边形 Gizmo 的定义，通常用于表示形状的边界框
#include "node/inputdragger.h"   // 引入输入拖动器相关定义，用于 Gizmo 交互
#include "node/node.h"           // 引入基类 Node 的定义 (虽然 GeneratorWithMerge 已继承 Node，但显式包含无害)

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表所有基本形状生成器节点的基类。
 * 提供了处理形状通用属性（如位置、大小、颜色）以及与之交互的 Gizmo 的基础功能。
 * 它继承自 GeneratorWithMerge，意味着生成的形状可以与一个“基础”输入（如背景图像）合并。
 */
class ShapeNodeBase : public GeneratorWithMerge {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief ShapeNodeBase 构造函数。
      * @param create_color_input 是否为此形状节点创建颜色输入参数，默认为 true。
      */
     explicit ShapeNodeBase(bool create_color_input = true);

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 更新 Gizmo (交互控件，如形状的边界框和控制点) 在屏幕上的位置。
   *  当节点参数变化或视图变化时调用。
   * @param row 当前节点的参数值。
   * @param globals 全局节点处理参数。
   */
  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  /**
   * @brief 设置形状的矩形区域（位置和大小）。
   * @param rect QRectF 对象，定义了新的矩形区域。
   * @param sequence_res 序列（画布）的分辨率，用于坐标转换。
   * @param command 指向 MultiUndoCommand 的指针，用于将此操作添加到撤销/重做栈。
   */
  void SetRect(QRectF rect, const VideoParams &sequence_res, MultiUndoCommand *command);

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kPositionInput;  ///< "Position" - 形状位置（通常是中心点或左上角）的参数键名。
  static const QString kSizeInput;      ///< "Size" - 形状尺寸（宽度和高度）的参数键名。
  static const QString kColorInput;     ///< "Color" - 形状填充颜色的参数键名 (如果构造时 create_color_input 为 true)。

 protected:
  /**
   * @brief 获取指向多边形 Gizmo (通常代表形状的边界框) 的指针。
   * @return PolygonGizmo* 指向多边形 Gizmo 对象的指针。
   */
  [[nodiscard]] PolygonGizmo *poly_gizmo() const { return poly_gizmo_; }

 protected slots:
  /**
   * @brief 当 Gizmo (例如形状的边界控制点) 被拖动时调用的槽函数。
   * @param x Gizmo 在画布上的新 X 坐标或差量。
   * @param y Gizmo 在画布上的新 Y 坐标或差量。
   * @param modifiers 当前按下的键盘修饰键 (如 Shift, Ctrl)。
   */
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  /**
   * @brief 根据给定的位置、尺寸和 Gizmo 对象，生成 Gizmo 的锚点。
   *  用于确定 Gizmo 拖动时的参考点。
   * @param pos 形状的位置。
   * @param size 形状的尺寸。
   * @param gizmo 当前被操作的 Gizmo 对象。
   * @param pt (可选) 指向 QVector2D 的指针，用于存储计算出的锚点坐标。
   * @return QVector2D 计算出的锚点。
   */
  QVector2D GenerateGizmoAnchor(const QVector2D &pos, const QVector2D &size, NodeGizmo *gizmo,
                                QVector2D *pt = nullptr) const;

  // --- Gizmo 位置判断辅助函数 ---
  /** @brief 检查给定的 Gizmo 是否是顶部控制点/边。 */
  bool IsGizmoTop(NodeGizmo *g) const;
  /** @brief 检查给定的 Gizmo 是否是底部控制点/边。 */
  bool IsGizmoBottom(NodeGizmo *g) const;
  /** @brief 检查给定的 Gizmo 是否是左侧控制点/边。 */
  bool IsGizmoLeft(NodeGizmo *g) const;
  /** @brief 检查给定的 Gizmo 是否是右侧控制点/边。 */
  bool IsGizmoRight(NodeGizmo *g) const;
  /** @brief 检查给定的 Gizmo 是否是水平中心控制点/边。 */
  bool IsGizmoHorizontalCenter(NodeGizmo *g) const;
  /** @brief 检查给定的 Gizmo 是否是垂直中心控制点/边。 */
  bool IsGizmoVerticalCenter(NodeGizmo *g) const;
  /** @brief 检查给定的 Gizmo 是否是角控制点。 */
  bool IsGizmoCorner(NodeGizmo *g) const;

  // --- Gizmo 相关私有成员变量 ---
  // static const int kGizmoScaleCount = 8; // 假设 kGizmoScaleCount 在 .cpp 文件中定义，通常为8个边框控制点
  static const int kGizmoWholeRect =
      /*kGizmoScaleCount*/ 8;  ///< 用于标识代表整个矩形区域的 Gizmo 的索引，通常是控制点数组后的一个。
  PointGizmo *point_gizmo_[/*kGizmoScaleCount*/ 8]{};  ///< 指向各个边界控制点 Gizmo (PointGizmo)
                                                       ///< 的指针数组。大小通常为8（四角+四边中点）。
  PolygonGizmo *poly_gizmo_;                           ///< 指向代表整个形状可交互边界框的 Gizmo (PolygonGizmo) 的指针。
};

}  // namespace olive

#endif  // SHAPENODEBASE_H // 头文件宏定义结束