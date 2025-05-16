#ifndef NODEVIEWITEMCONNECTOR_H
#define NODEVIEWITEMCONNECTOR_H

#include <QGraphicsPolygonItem> // Qt 图形多边形项基类，用于绘制自定义形状
#include <QPainterPath>         // Qt 绘制路径类 (用于 shape() 方法)
#include <QRectF>               // Qt 浮点数矩形类 (用于 boundingRect() 方法)

#include "nodeviewcommon.h"     // 节点视图通用定义 (例如 FlowDirection)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QGraphicsItem; // QGraphicsPolygonItem 的基类

namespace olive {

/**
 * @brief NodeViewItemConnector 类代表节点视图项 (NodeViewItem) 上的一个连接点（输入或输出端口）。
 *
 * 它是一个小型的图形项（通常显示为三角形或圆形），用户可以通过点击或拖拽它来创建节点之间的连接。
 * 此类继承自 QGraphicsPolygonItem，允许绘制自定义形状的连接器。
 */
class NodeViewItemConnector : public QGraphicsPolygonItem {
  // Q_OBJECT // QGraphicsPolygonItem 默认不是 QObject 的子类，除非需要信号/槽。遵循不修改原则。

public:
  /**
   * @brief 构造函数。
   * @param is_output 布尔值，指示此连接器是输出端口 (true) 还是输入端口 (false)。
   * @param parent 父 QGraphicsItem 指针，默认为 nullptr。
   */
  explicit NodeViewItemConnector(bool is_output, QGraphicsItem *parent = nullptr);

  /**
   * @brief 设置连接器的流向（布局方向）。
   *
   * 流向会影响连接器（通常是三角形）的朝向。
   * @param dir FlowDirection 枚举值。
   */
  void SetFlowDirection(NodeViewCommon::FlowDirection dir);

  /**
   * @brief 检查此连接器是否为输出端口。
   * @return 如果是输出端口，则返回 true；否则（如果是输入端口）返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsOutput() const { return output_; }

  /**
   * @brief 重写 QGraphicsItem 的 shape 方法，返回连接器的精确形状。
   *
   * 用于更精确的碰撞检测和交互。
   * @return 返回一个 QPainterPath 对象，描述了连接器的形状。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QPainterPath shape() const override;
  /**
   * @brief 重写 QGraphicsItem 的 boundingRect 方法，返回连接器的包围矩形。
   *
   * 定义了项的绘制和交互区域。
   * @return 返回一个 QRectF 对象，描述了连接器的边界。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QRectF boundingRect() const override;

private:
  bool output_; ///< 标记此连接器是输出端口 (true) 还是输入端口 (false)。
};

}  // namespace olive

#endif  // NODEVIEWITEMCONNECTOR_H
