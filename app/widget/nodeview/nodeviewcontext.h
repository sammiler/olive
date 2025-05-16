#ifndef NODEVIEWCONTEXT_H
#define NODEVIEWCONTEXT_H

#include <QGraphicsRectItem>  // Qt 图形矩形项基类
#include <QGraphicsTextItem>  // Qt 图形文本项基类 (虽然未直接使用，但可能在实现中用于显示标签)
#include <QMap>               // Qt 映射容器 (用于 item_map_)
#include <QObject>            // Qt 对象模型基类 (NodeViewContext 的父类之一)
#include <QVector>            // Qt 动态数组容器 (用于 edges_)

#include "node/node.h"       // 节点基类定义
#include "node/nodeundo.h"   // 节点相关的撤销命令 (例如 NodeViewDeleteCommand)
#include "nodeviewcommon.h"  // 节点视图通用定义 (例如 FlowDirection)
#include "nodeviewedge.h"    // 节点视图中的连接线项
#include "nodeviewitem.h"    // 节点视图中的单个节点图形项 (NodeViewContext 管理这些项)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QPainter;
// class QStyleOptionGraphicsItem;
// class QWidget;
// class QGraphicsItem;
// class QPointF;
// class QVariant;
// class QGraphicsSceneMouseEvent;
// class QString; // Node 类可能使用

// 前向声明项目内部类 (根据用户要求，不添加)
// class NodeInput; // NodeViewEdge 可能使用
// class NodeViewDeleteCommand; // 已包含 node/nodeundo.h

namespace olive {

/**
 * @brief NodeViewContext 类代表节点视图 (NodeView) 中的一个图形上下文。
 *
 * 这个上下文通常对应一个节点，尤其是节点组 (NodeGroup)，它在场景中显示为一个矩形区域，
 * 内部可以包含其他的节点项 (NodeViewItem) 和它们之间的连接线 (NodeViewEdge)。
 * 它负责管理这些子项的布局、连接和用户交互。
 * 此类继承自 QObject 以支持信号和槽，并继承自 QGraphicsRectItem 以便在 QGraphicsScene 中绘制为一个矩形。
 */
class NodeViewContext : public QObject, public QGraphicsRectItem {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param context 此图形上下文所代表的实际 Node 对象（例如一个 NodeGroup）。
      * @param item 父 QGraphicsItem 指针，默认为 nullptr。
      */
     explicit NodeViewContext(Node *context, QGraphicsItem *item = nullptr);

  /**
   * @brief 析构函数。
   */
  ~NodeViewContext() override;

  /**
   * @brief 获取此图形上下文所代表的 Node 对象。
   * @return 返回 Node 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Node *GetContext() const { return context_; }

  /**
   * @brief 更新此上下文矩形的大小以适应其内容。
   */
  void UpdateRect();

  /**
   * @brief 设置此上下文中子项的布局流向。
   * @param dir FlowDirection 枚举值，指定新的流向。
   */
  void SetFlowDirection(NodeViewCommon::FlowDirection dir);

  /**
   * @brief 设置此上下文中连接线是否应绘制为曲线。
   * @param e 如果为 true，则绘制曲线连接；否则绘制直线。
   */
  void SetCurvedEdges(bool e);

  /**
   * @brief 删除此上下文中所有选中的节点项。
   * @param command 用于记录删除操作的 NodeViewDeleteCommand 指针（以便撤销）。
   * @return 返回被删除项的数量。
   */
  int DeleteSelected(NodeViewDeleteCommand *command);

  /**
   * @brief 选中此上下文中的指定节点列表对应的图形项。
   * @param nodes 包含要选中的 Node 指针的 QVector。
   */
  void Select(const QVector<Node *> &nodes);

  /**
   * @brief 获取此上下文中当前所有选中的节点图形项。
   * @return 返回一个包含 NodeViewItem 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVector<NodeViewItem *> GetSelectedItems() const;

  /**
   * @brief 将场景坐标系中的点映射到此上下文内部节点使用的相对坐标。
   * @param pos 场景坐标系中的 QPointF 点。
   * @return 返回映射后的 QPointF 点。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QPointF MapScenePosToNodePosInContext(const QPointF &pos) const;

  /**
   * @brief 根据 Node 指针从此上下文的项映射中获取对应的 NodeViewItem 指针。
   * @param node 要查找其图形项的 Node 指针。
   * @return 如果找到，则返回 NodeViewItem 指针；否则返回 nullptr。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  NodeViewItem *GetItemFromMap(Node *node) const { return item_map_.value(node); }

  /**
   * @brief 重写 QGraphicsItem 的 paint 方法，用于绘制此上下文的背景和可能的标签。
   * @param painter QPainter 指针。
   * @param option 绘图选项。
   * @param widget 目标 QWidget 指针，默认为 nullptr。
   */
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

 public slots:
  /**
   * @brief 向此上下文中添加一个子节点（并创建其图形项）。
   * @param node 要添加的 Node 指针。
   */
  void AddChild(Node *node);

  /**
   * @brief 设置此上下文中某个子节点图形项的位置。
   * @param node 目标 Node 指针。
   * @param pos 新的位置 (QPointF)。
   */
  void SetChildPosition(Node *node, const QPointF &pos);

  /**
   * @brief 从此上下文中移除一个子节点（及其图形项）。
   * @param node 要移除的 Node 指针。
   */
  void RemoveChild(Node *node);

  /**
   * @brief 当此上下文中的某个子节点的输入被连接时调用的槽函数。
   * @param output 提供输出的节点。
   * @param input 接收连接的输入参数。
   */
  void ChildInputConnected(Node *output, const NodeInput &input);

  /**
   * @brief 当此上下文中的某个子节点的输入连接被断开时调用的槽函数。
   * @param output 先前提供输出的节点。
   * @param input 被断开连接的输入参数。
   * @return 如果成功处理断开连接，则返回 true。
   */
  bool ChildInputDisconnected(Node *output, const NodeInput &input);

 signals:
  /**
   * @brief 当一个节点图形项即将从此上下文中被删除时发出此信号。
   * @param item 指向即将被删除的 NodeViewItem 的指针。
   */
  void ItemAboutToBeDeleted(NodeViewItem *item);

 protected:
  /**
   * @brief 重写 QGraphicsItem 的 itemChange 方法，用于响应项的特定状态变化。
   * @param change 正在发生的改变类型 (QGraphicsItem::GraphicsItemChange)。
   * @param value 与改变相关的值。
   * @return 返回处理后的值。
   */
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

  /**
   * @brief 重写 QGraphicsItem 的 mousePressEvent 方法，用于处理鼠标按下事件。
   * @param event 鼠标场景事件指针。
   */
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

 private:
  /**
   * @brief 内部函数，用于将一个 Node 对象及其对应的 NodeViewItem 添加到此上下文中。
   * @param node 要添加的 Node 指针。
   * @param item 与 node 对应的 NodeViewItem 指针。
   */
  void AddNodeInternal(Node *node, NodeViewItem *item);

  /**
   * @brief 内部函数，用于在此上下文中添加一条表示节点连接的边。
   * @param output 输出节点。
   * @param input 接收输入的参数。
   * @param from 连接的起始 NodeViewItem。
   * @param to 连接的目标 NodeViewItem。
   */
  void AddEdgeInternal(Node *output, const NodeInput &input, NodeViewItem *from, NodeViewItem *to);

  Node *context_;  ///< 此图形上下文所代表的实际 Node 对象（通常是 NodeGroup）。

  QString lbl_;  ///< 可能用于显示在上下文矩形上的标签文本（例如组名）。

  NodeViewCommon::FlowDirection flow_dir_;  ///< 此上下文中子项的布局流向。

  bool curved_edges_{};  ///< 标记此上下文中的连接线是否应绘制为曲线。默认为 false。

  int last_titlebar_height_{};  ///< 上一次计算的标题栏高度，可能用于布局。

  QMap<Node *, NodeViewItem *> item_map_;  ///< 存储此上下文中包含的 Node 与其对应的 NodeViewItem 的映射。

  QVector<NodeViewEdge *> edges_;  ///< 存储此上下文中所有连接线的列表。

 private slots:
  /**
   * @brief 当此上下文（如果是一个 NodeGroup）添加了一个新的内部节点时调用的槽函数。
   * @param node 被添加到组中的 Node 指针。
   */
  void GroupAddedNode(Node *node);

  /**
   * @brief 当此上下文（如果是一个 NodeGroup）移除了一个内部节点时调用的槽函数。
   * @param node 从组中被移除的 Node 指针。
   */
  void GroupRemovedNode(Node *node);
};

}  // namespace olive

#endif  // NODEVIEWCONTEXT_H
