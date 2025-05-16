#ifndef NODEEDGEITEM_H  // 文件名是 nodeedgeitem.h，宏定义是 NODEEDGEITEM_H，一致
#define NODEEDGEITEM_H

#include <QGraphicsPathItem>  // Qt 图形路径项基类，用于绘制复杂的2D图形路径
#include <QPalette>           // Qt 调色板类，用于颜色和样式信息

#include "node/node.h"       // 节点基类定义 (output_ 成员)
#include "node/param.h"      // 节点参数相关定义 (包含 NodeInput)
#include "nodeviewcommon.h"  // 节点视图通用定义 (可能包含 FlowDirection 等，虽然在此头文件未直接使用)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QPainter;
// class QStyleOptionGraphicsItem;
// class QWidget;
// class QPointF;
// class QGraphicsItem; // QGraphicsPathItem 的基类

namespace olive {

class NodeViewItem;  // 前向声明 NodeViewItem 类，因为它是边的起点和终点项类型

/**
 * @brief NodeViewEdge 类是 NodeEdge（节点间逻辑连接）在 NodeView 中的图形表示。
 *
 * 这是一个相当简单的线条控件，用于可视化两个节点参数之间的连接 (NodeEdge)。
 * 它继承自 QGraphicsPathItem，允许绘制直线或曲线来表示连接。
 * 该类还处理连接状态（已连接/未连接/高亮）的视觉反馈。
 */
class NodeViewEdge : public QGraphicsPathItem {
  // Q_OBJECT // QGraphicsPathItem 默认不是 QObject 的子类，除非需要信号/槽，否则不加。遵循不修改原则。
 public:
  /**
   * @brief 构造函数，创建一个表示已存在连接的边。
   * @param output 提供输出的 Node 指针。
   * @param input 接收输入的 NodeInput 对象。
   * @param from_item 连接的起始 NodeViewItem (通常是输出节点项)。
   * @param to_item 连接的目标 NodeViewItem (通常是输入节点项)。
   * @param parent 父 QGraphicsItem 指针，默认为 nullptr。
   */
  NodeViewEdge(Node* output, NodeInput input, NodeViewItem* from_item, NodeViewItem* to_item,
               QGraphicsItem* parent = nullptr);

  /**
   * @brief 构造函数，创建一个临时的或未完全定义的边（例如，在用户拖拽创建连接时）。
   * @param parent 父 QGraphicsItem 指针，默认为 nullptr。
   */
  explicit NodeViewEdge(QGraphicsItem* parent = nullptr);

  /**
   * @brief 析构函数。
   */
  ~NodeViewEdge() override;

  /**
   * @brief 获取此连接边的输出节点。
   * @return 返回输出 Node 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Node* output() const { return output_; }

  /**
   * @brief 获取此连接边的目标输入参数。
   * @return 返回对 NodeInput 对象的 const 引用。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const NodeInput& input() const { return input_; }

  /**
   * @brief 获取此连接边目标输入参数的元素索引（如果输入是多维的，如向量）。
   * @return 返回元素索引 (int 类型)。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int element() const { return element_; }

  /**
   * @brief 获取此连接边的起始图形项 (NodeViewItem)。
   * @return 返回起始 NodeViewItem 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] NodeViewItem* from_item() const { return from_item_; }

  /**
   * @brief 获取此连接边的目标图形项 (NodeViewItem)。
   * @return 返回目标 NodeViewItem 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] NodeViewItem* to_item() const { return to_item_; }

  /**
   * @brief 设置此连接边的起始图形项。
   * @param i 指向新的起始 NodeViewItem 的指针。
   */
  void set_from_item(NodeViewItem* i);

  /**
   * @brief 设置此连接边的目标图形项。
   * @param i 指向新的目标 NodeViewItem 的指针。
   */
  void set_to_item(NodeViewItem* i);

  /**
   * @brief 调整边的路径以正确连接其起点和终点项。
   *
   * 当起点或终点项移动时，需要调用此函数来更新边的绘制路径。
   */
  void Adjust();

  /**
   * @brief 设置此连接线的连接状态。
   *
   * 当边未连接时，它会通过将线条颜色变为灰色来直观地表示这一点。
   * 当边已连接或潜在连接有效时，线条颜色为白色。
   * 此函数设置线条应该是灰色 (false) 还是白色 (true)。
   *
   * 使用 SetEdge() (如果存在这样的方法，或者在构造时) 会自动将此设置为 true。
   * 在大多数情况下，应保持此状态不变，仅在创建/拖动边时进行设置。
   * @param c 布尔值，true 表示已连接（或有效），false 表示未连接。
   */
  void SetConnected(bool c);

  /**
   * @brief 检查此边当前是否被视为已连接状态。
   * @return 如果已连接，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsConnected() const { return connected_; }

  /**
   * @brief 设置边的高亮状态。
   *
   * 会改变边的颜色以指示高亮。
   * @param e 如果为 true，则高亮显示边；否则取消高亮。
   */
  void SetHighlighted(bool e);

  /**
   * @brief 设置用于创建曲线（或直线）的起点和终点。
   * @param start 边的起点坐标 (QPointF)。
   * @param end 边的终点坐标 (QPointF)。
   */
  void SetPoints(const QPointF& start, const QPointF& end);

  /**
   * @brief 设置边应该绘制为曲线还是直线。
   * @param e 如果为 true，则绘制为曲线；否则绘制为直线。
   */
  void SetCurved(bool e);

 protected:
  /**
   * @brief 重写 QGraphicsPathItem 的 paint 方法，用于自定义边的绘制。
   * @param painter QPainter 指针。
   * @param option 绘图选项。
   * @param widget 目标 QWidget 指针，默认为 nullptr。
   */
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

 private:
  /**
   * @brief 执行边的通用初始化操作。
   */
  void Init();

  /**
   * @brief 更新边的绘制路径（通常在点或弯曲状态改变后调用）。
   */
  void UpdateCurve();

  Node* output_{};  ///< 指向连接的输出端所属的 Node 对象。初始化为 nullptr。

  NodeInput input_;  ///< 描述连接的目标输入端点。

  int element_{};  ///< 如果目标输入是多维的（例如向量），则表示连接到哪个元素/分量。初始化为0。

  NodeViewItem* from_item_;  ///< 指向连接的起始节点图形项。
  NodeViewItem* to_item_;    ///< 指向连接的目标节点图形项。

  int edge_width_{};  ///< 边的绘制宽度。初始化为0。

  bool connected_{};    ///< 标记此边当前是否处于有效连接状态。初始化为 false。
  bool highlighted_{};  ///< 标记此边当前是否处于高亮状态。初始化为 false。
  bool curved_{};       ///< 标记此边是否应绘制为曲线。初始化为 false。

  QPointF cached_start_;  ///< 缓存的边起点坐标，用于优化重绘。
  QPointF cached_end_;    ///< 缓存的边终点坐标，用于优化重绘。
};

}  // namespace olive

#endif  // NODEEDGEITEM_H
