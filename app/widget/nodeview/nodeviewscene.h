#ifndef NODEVIEWSCENE_H
#define NODEVIEWSCENE_H

#include <QGraphicsScene>  // Qt 图形场景基类
#include <QHash>           // Qt 哈希表容器 (用于 context_map_)
#include <QTimer>          // Qt 定时器类 (虽然在此头文件未直接使用，但可能在 .cpp 或相关类中使用)
#include <QVector>         // Qt 动态数组容器 (用于 GetSelectedItems 返回类型)

#include "node/project.h"     // 项目类定义 (graph_ 成员)
#include "nodeviewcommon.h"   // 节点视图通用定义 (例如 FlowDirection)
#include "nodeviewcontext.h"  // 节点视图上下文项类
#include "nodeviewedge.h"     // 节点视图连接线项类 (虽然未直接使用，但上下文会管理)
#include "nodeviewitem.h"     // 节点视图中的单个节点图形项类
#include "undo/undostack.h"   // 撤销栈类 (虽然未直接使用，但场景操作通常会与撤销栈交互)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QObject; // QGraphicsScene 的基类
// class QWidget; // QGraphicsScene 的构造函数参数可能需要

// 前向声明项目内部类 (根据用户要求，不添加)
// class Node; // 已在 node/project.h 或 nodeviewcontext.h 中包含

namespace olive {

/**
 * @brief NodeViewScene 类是用于节点视图 (NodeView) 的自定义图形场景。
 *
 * 它继承自 QGraphicsScene，负责管理节点图中的所有图形项，
 * 包括节点上下文 (NodeViewContext)、单个节点项 (NodeViewItem) 和连接它们的边 (NodeViewEdge)。
 * 此场景还处理选择逻辑、布局流向以及边的绘制样式（直线或曲线）。
 */
class NodeViewScene : public QGraphicsScene {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父 QObject 指针，默认为 nullptr。
      */
     explicit NodeViewScene(QObject *parent = nullptr);

  /**
   * @brief 选中场景中的所有可选图形项（通常是 NodeViewItem）。
   */
  void SelectAll();
  /**
   * @brief 取消选中场景中的所有图形项。
   */
  void DeselectAll();

  /**
   * @brief 获取当前场景中所有被选中的 NodeViewItem。
   * @return 返回一个包含选中的 NodeViewItem 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVector<NodeViewItem *> GetSelectedItems() const;

  /**
   * @brief 获取从 Node 指针到其对应的 NodeViewContext 指针的映射。
   * @return 返回一个 const 引用，指向 QHash<Node *, NodeViewContext *>。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QHash<Node *, NodeViewContext *> &context_map() const { return context_map_; }

  /**
   * @brief 获取当前场景中节点布局的 Qt::Orientation (水平或垂直)。
   * @return 返回 Qt::Orientation 枚举值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Qt::Orientation GetFlowOrientation() const;

  /**
   * @brief 获取当前场景中节点布局的详细流向。
   * @return 返回 NodeViewCommon::FlowDirection 枚举值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] NodeViewCommon::FlowDirection GetFlowDirection() const { return direction_; }

  /**
   * @brief 设置场景中节点布局的流向。
   * @param direction 新的 NodeViewCommon::FlowDirection。
   */
  void SetFlowDirection(NodeViewCommon::FlowDirection direction);

  /**
   * @brief 检查当前场景中的连接线是否被设置为曲线绘制模式。
   * @return 如果边应绘制为曲线，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool GetEdgesAreCurved() const { return curved_edges_; }

 public slots:
  /**
   * @brief 向场景中添加一个新的上下文（通常代表一个节点或节点组）。
   * @param node 要为其创建并添加上下文的 Node 指针。
   * @return 返回创建的 NodeViewContext 指针。
   */
  NodeViewContext *AddContext(Node *node);
  /**
   * @brief 从场景中移除一个上下文及其关联的所有图形项。
   * @param node 要移除其上下文的 Node 指针。
   */
  void RemoveContext(Node *node);

  /**
   * @brief 设置场景中的连接线是否应绘制为曲线。
   * @param curved 如果为 true，则边将绘制为曲线；否则为直线。
   */
  void SetEdgesAreCurved(bool curved);

 private:
  QHash<Node *, NodeViewContext *> context_map_;  ///< 存储从 Node 指针到其对应的 NodeViewContext 图形项的映射。

  Project *graph_{};  ///< 指向当前场景关联的项目 (Project) 或节点图 (NodeGraph) 的指针。初始化为 nullptr。
                      ///< 注意：变量名是 graph_，但类型是 Project*，这可能暗示 Project
                      ///< 类也扮演了图的角色，或者这里有待澄清。

  NodeViewCommon::FlowDirection direction_;  ///< 当前场景中节点布局的流向。

  bool curved_edges_;  ///< 标记场景中的连接线是否应绘制为曲线。
};

}  // namespace olive

#endif  // NODEVIEWSCENE_H
