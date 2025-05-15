#ifndef POINTGIZMO_H // 防止头文件被多次包含的宏定义开始
#define POINTGIZMO_H

#include <QPointF> // Qt 的浮点数点类

#include "draggable.h" // 引入基类 DraggableGizmo (或类似可拖动 Gizmo 基类) 的定义
                       // 假设 "draggable.h" 是正确的基类头文件名

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表一个点 Gizmo (图形交互控件)。
 * 该 Gizmo 用于在视图中显示一个可交互的点，通常表示位置、控制点（如贝塞尔曲线的控制点、形状的顶点）。
 * 用户可以拖动这个点来修改其关联的节点参数。
 * 它可以显示为不同的形状（方形、圆形等）。
 */
class PointGizmo : public DraggableGizmo {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief 定义点 Gizmo 可以显示的形状。
   */
  enum Shape {
    kSquare,      ///< 方形
    kCircle,      ///< 圆形
    kAnchorPoint  ///< 特殊的锚点形状 (通常用于表示贝塞尔曲线的顶点)
  };

  /**
   * @brief PointGizmo 构造函数。
   * @param shape 点 Gizmo 的显示形状。
   * @param smaller 是否以较小尺寸绘制此 Gizmo，默认为 false。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  explicit PointGizmo(const Shape &shape, bool smaller, QObject *parent = nullptr);
  /**
   * @brief PointGizmo 构造函数 (不指定 smaller，默认为 false)。
   * @param shape 点 Gizmo 的显示形状。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  explicit PointGizmo(const Shape &shape, QObject *parent = nullptr);
  /**
   * @brief PointGizmo 构造函数 (使用默认形状和大小)。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  explicit PointGizmo(QObject *parent = nullptr);

  /**
   * @brief 获取点 Gizmo 当前的显示形状。
   * @return const Shape& 对 Shape 枚举值的常量引用。
   */
  [[nodiscard]] const Shape &GetShape() const { return shape_; }
  /**
   * @brief 设置点 Gizmo 的显示形状。
   * @param s 新的 Shape 枚举值。
   */
  void SetShape(const Shape &s) { shape_ = s; }

  /**
   * @brief 获取点 Gizmo 当前的位置坐标。
   * @return const QPointF& 对 QPointF 对象的常量引用。
   */
  [[nodiscard]] const QPointF &GetPoint() const { return point_; }
  /**
   * @brief 设置点 Gizmo 的位置坐标。
   * @param pt 新的 QPointF 对象。
   */
  void SetPoint(const QPointF &pt) { point_ = pt; }

  /**
   * @brief 检查点 Gizmo 是否设置为较小尺寸绘制。
   * @return 如果设置为较小尺寸则返回 true，否则返回 false。
   */
  [[nodiscard]] bool GetSmaller() const { return smaller_; }
  /**
   * @brief 设置点 Gizmo 是否以较小尺寸绘制。
   * @param e true 表示较小尺寸，false 表示标准尺寸。
   */
  void SetSmaller(bool e) { smaller_ = e; }

  /**
   * @brief 绘制此点 Gizmo。
   *  重写自基类，根据 shape_ 和 point_ 使用 QPainter 绘制点。
   * @param p 指向 QPainter 对象的指针，用于进行绘图操作。
   */
  void Draw(QPainter *p) const override;

  /**
   * @brief 获取用于检测鼠标点击的矩形区域。
   *  考虑到当前的变换矩阵，计算出 Gizmo 在屏幕上的有效点击范围。
   * @param t 当前视图的变换矩阵 (从序列坐标到屏幕坐标)。
   * @return QRectF Gizmo 的可点击矩形区域。
   */
  [[nodiscard]] QRectF GetClickingRect(const QTransform &t) const;

 private:
  /**
   * @brief (静态工具函数) 获取点 Gizmo 的标准绘制半径。
   * @return double 标准半径值。
   */
  static double GetStandardRadius();

  /**
   * @brief 获取用于绘制 Gizmo 的矩形区域。
   *  考虑到当前的变换和指定的半径，计算出绘制 Gizmo 所需的边界框。
   * @param transform 当前视图的变换矩阵。
   * @param radius Gizmo 的绘制半径。
   * @return QRectF Gizmo 的绘制边界矩形。
   */
  [[nodiscard]] QRectF GetDrawingRect(const QTransform &transform, double radius) const;

  Shape shape_; ///< 点 Gizmo 的显示形状 (方形、圆形等)。

  QPointF point_; ///< 点 Gizmo 在序列（画布）坐标系中的位置。

  bool smaller_; ///< 标记是否以较小尺寸绘制此 Gizmo。
};

}  // namespace olive

#endif  // POINTGIZMO_H // 头文件宏定义结束