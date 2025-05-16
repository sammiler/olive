#ifndef PATHGIZMO_H  // 防止头文件被多次包含的宏定义开始
#define PATHGIZMO_H

#include <QPainterPath>  // Qt 的 QPainterPath 类，用于表示和操作复杂的2D路径

#include "draggable.h"  // 引入基类 DraggableGizmo (或类似可拖动 Gizmo 基类) 的定义
                        // 假设 "draggable.h" 是正确的基类头文件名，通常可能是 "draggablegizmo.h"

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个路径 Gizmo (图形交互控件)。
 * 该 Gizmo 用于在视图中显示一个由 QPainterPath 定义的复杂2D路径。
 * 它继承自 DraggableGizmo，意味着整个路径本身可能支持拖动操作（例如，整体平移多边形）。
 */
class PathGizmo : public DraggableGizmo {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief PathGizmo 构造函数。
      * @param parent 父 QObject 对象，默认为 nullptr。
      */
     explicit PathGizmo(QObject *parent = nullptr);

  /**
   * @brief 获取此路径 Gizmo 代表的路径几何信息。
   * @return const QPainterPath& 对 QPainterPath 对象的常量引用。
   */
  [[nodiscard]] const QPainterPath &GetPath() const { return path_; }
  /**
   * @brief 设置此路径 Gizmo 代表的路径几何信息。
   * @param path QPainterPath 对象。
   */
  void SetPath(const QPainterPath &path) { path_ = path; }

  /**
   * @brief 绘制此路径 Gizmo。
   *  重写自基类，使用 QPainter 绘制存储的 path_。
   * @param p 指向 QPainter 对象的指针，用于进行绘图操作。
   */
  void Draw(QPainter *p) const override;

 private:
  QPainterPath path_;  ///< 存储路径的几何数据。QPainterPath 可以包含直线、曲线（如贝塞尔曲线）、子路径等。
};

}  // namespace olive

#endif  // PATHGIZMO_H // 头文件宏定义结束