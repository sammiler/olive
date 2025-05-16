#ifndef LINEGIZMO_H  // 防止头文件被多次包含的宏定义开始
#define LINEGIZMO_H

#include <QLineF>  // Qt 的浮点数线段类

#include "gizmo.h"  // 引入基类 NodeGizmo (或类似 Gizmo 基类，这里可能是笔误，应为 "node/gizmo/gizmo.h" 或 "nodegizmo.h")
                    // 假设 "gizmo.h" 是正确的基类头文件

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个线段 Gizmo (图形交互控件)。
 * 该 Gizmo 用于在视图中显示一条线段，通常用于连接其他 Gizmo 元素（如贝塞尔曲线的控制点和顶点）
 * 或者表示方向、轴线等。
 */
class LineGizmo : public NodeGizmo {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief LineGizmo 构造函数。
      * @param parent 父 QObject 对象，默认为 nullptr。
      */
     explicit LineGizmo(QObject *parent = nullptr);

  /**
   * @brief 获取此线段 Gizmo 代表的线段几何信息。
   * @return const QLineF& 对 QLineF 对象的常量引用。
   */
  [[nodiscard]] const QLineF &GetLine() const { return line_; }
  /**
   * @brief 设置此线段 Gizmo 代表的线段几何信息。
   * @param line QLineF 对象。
   */
  void SetLine(const QLineF &line) { line_ = line; }

  /**
   * @brief 绘制此线段 Gizmo。
   *  重写自 NodeGizmo 基类，使用 QPainter 绘制存储的 line_。
   * @param p 指向 QPainter 对象的指针，用于进行绘图操作。
   */
  void Draw(QPainter *p) const override;

 private:
  QLineF line_;  ///< 存储线段的几何数据 (起点和终点坐标)。
};

}  // namespace olive

#endif  // LINEGIZMO_H // 头文件宏定义结束