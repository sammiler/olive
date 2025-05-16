#ifndef POLYGONGIZMO_H  // 防止头文件被多次包含的宏定义开始
#define POLYGONGIZMO_H

#include <QPolygonF>  // Qt 的浮点数多边形类，由一系列顶点定义

#include "draggable.h"  // 引入基类 DraggableGizmo (或类似可拖动 Gizmo 基类) 的定义
                        // 假设 "draggable.h" 是正确的基类头文件名

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个多边形 Gizmo (图形交互控件)。
 * 该 Gizmo 用于在视图中显示一个由 QPolygonF 定义的多边形。
 * 通常用于表示形状的边界框、可交互区域（如裁剪框、变换框的整体）等。
 * 它继承自 DraggableGizmo，意味着整个多边形本身可能支持拖动操作。
 */
class PolygonGizmo : public DraggableGizmo {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief PolygonGizmo 构造函数。
      * @param parent 父 QObject 对象，默认为 nullptr。
      */
     explicit PolygonGizmo(QObject *parent = nullptr);

  /**
   * @brief 获取此多边形 Gizmo 代表的多边形几何信息。
   * @return const QPolygonF& 对 QPolygonF 对象的常量引用。
   */
  [[nodiscard]] const QPolygonF &GetPolygon() const { return polygon_; }
  /**
   * @brief 设置此多边形 Gizmo 代表的多边形几何信息。
   * @param polygon QPolygonF 对象。
   */
  void SetPolygon(const QPolygonF &polygon) { polygon_ = polygon; }

  /**
   * @brief 绘制此多边形 Gizmo。
   *  重写自基类，使用 QPainter 绘制存储的 polygon_。
   * @param p 指向 QPainter 对象的指针，用于进行绘图操作。
   */
  void Draw(QPainter *p) const override;

 private:
  QPolygonF polygon_;  ///< 存储多边形的顶点数据。
};

}  // namespace olive

#endif  // POLYGONGIZMO_H // 头文件宏定义结束