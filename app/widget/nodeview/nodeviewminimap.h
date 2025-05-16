#ifndef NODEVIEWMINIMAP_H
#define NODEVIEWMINIMAP_H

#include <QGraphicsView>  // Qt 图形视图基类
#include <QPoint>         // Qt 整数二维点类
#include <QPointF>        // Qt 浮点数二维点类
#include <QPolygonF>      // Qt 浮点数多边形类 (用于表示视口矩形)
#include <QRectF>         // Qt 浮点数矩形类 (用于绘制和场景边界)

#include "nodeviewscene.h"  // 节点视图的自定义 QGraphicsScene

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QPainter;
// class QResizeEvent;
// class QMouseEvent;

namespace olive {

/**
 * @brief NodeViewMiniMap 类是 NodeView 的一个小地图概览控件。
 *
 * 它继承自 QGraphicsView，并显示与主 NodeView 相同的 NodeViewScene，
 * 但以缩小的形式。用户可以通过点击或拖动小地图来导航主视图。
 * 小地图上还会绘制一个矩形来指示主视图当前的可见区域 (视口)。
 * 小地图本身可以通过拖动其右下角的调整大小手柄来改变尺寸。
 */
class NodeViewMiniMap : public QGraphicsView {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param scene 指向与此小地图共享的 NodeViewScene 对象的指针。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit NodeViewMiniMap(NodeViewScene *scene, QWidget *parent = nullptr);

 public slots:
  /**
   * @brief 设置并更新在小地图上表示主视图视口的矩形。
   * @param rect 代表视口区域的 QPolygonF (通常是一个矩形)。
   */
  void SetViewportRect(const QPolygonF &rect);

 signals:
  /**
   * @brief 当小地图的大小被用户调整后发出此信号。
   */
  void Resized();

  /**
   * @brief 当用户在小地图上点击或拖动以请求主视图导航到特定场景点时发出此信号。
   * @param pos 目标场景点的 QPointF 坐标。
   */
  void MoveToScenePoint(const QPointF &pos);

 protected:
  /**
   * @brief 重写 QGraphicsView 的 drawForeground 方法，用于在场景内容之上绘制额外元素。
   *
   * 在此控件中，主要用于绘制表示主视图视口的矩形。
   * @param painter QPainter 指针。
   * @param rect 需要重绘的区域。
   */
  void drawForeground(QPainter *painter, const QRectF &rect) override;

  /**
   * @brief 重写 QWidget 的 resizeEvent 方法，处理控件大小改变事件。
   * @param event 尺寸调整事件指针。
   */
  void resizeEvent(QResizeEvent *event) override;

  /**
   * @brief 重写 QWidget 的 mousePressEvent 方法，处理鼠标按下事件。
   *
   * 用于开始小地图的拖拽导航或右下角的调整大小操作。
   * @param event 鼠标事件指针。
   */
  void mousePressEvent(QMouseEvent *event) override;
  /**
   * @brief 重写 QWidget 的 mouseMoveEvent 方法，处理鼠标移动事件。
   *
   * 用于在拖拽时更新主视图的位置或调整小地图的大小。
   * @param event 鼠标事件指针。
   */
  void mouseMoveEvent(QMouseEvent *event) override;
  /**
   * @brief 重写 QWidget 的 mouseReleaseEvent 方法，处理鼠标释放事件。
   *
   * 用于结束拖拽导航或调整大小操作。
   * @param event 鼠标事件指针。
   */
  void mouseReleaseEvent(QMouseEvent *event) override;
  /**
   * @brief 重写 QWidget 的 mouseDoubleClickEvent 方法。
   *
   * 当前实现为空，表示双击小地图无特定操作。
   * @param event 鼠标事件指针。
   */
  void mouseDoubleClickEvent(QMouseEvent *event) override {}

 private slots:
  /**
   * @brief 当关联的 QGraphicsScene 的边界发生改变时调用的槽函数。
   *
   * 用于更新小地图的显示范围以匹配整个场景。
   * @param bounding 场景新的边界矩形。
   */
  void SceneChanged(const QRectF &bounding);

  /**
   * @brief 设置小地图的默认（或初始）大小。
   */
  void SetDefaultSize();

 private:
  /**
   * @brief 检查鼠标事件是否发生在右下角的调整大小三角形区域内。
   * @param event 鼠标事件指针。
   * @return 如果鼠标在调整大小区域内，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  bool MouseInsideResizeTriangle(QMouseEvent *event) const;

  /**
   * @brief 根据鼠标事件发出 MoveToScenePoint 信号，请求主视图导航。
   * @param event 鼠标事件指针。
   */
  void EmitMoveSignal(QMouseEvent *event);

  int resize_triangle_sz_;  ///< 右下角用于调整大小的三角形手柄的尺寸（像素）。

  QPolygonF viewport_rect_;  ///< 存储当前主视图在场景坐标系中的视口矩形。

  bool resizing_;  ///< 标记当前是否正在通过拖动调整小地图的大小。

  QPoint resize_anchor_;  ///< 在开始调整小地图大小时，鼠标按下的锚点位置（通常是小地图的左上角或一个固定点）。
};

}  // namespace olive

#endif  // NODEVIEWMINIMAP_H
