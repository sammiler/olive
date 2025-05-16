#ifndef NODEPARAMVIEWCONNECTEDLABEL_H
#define NODEPARAMVIEWCONNECTEDLABEL_H

#include <QWidget> // Qt 控件基类

#include "node/param.h"                             // 节点参数相关定义 (包含 NodeInput)
#include "widget/clickablelabel/clickablelabel.h"   // 可点击标签控件
#include "widget/nodevaluetree/nodevaluetree.h"     // 节点值树形视图控件

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QMouseEvent; // 如果 mouseDoubleClickEvent 被重写，则需要

// 前向声明项目内部类 (根据用户要求，不添加)
// class Node;
// class ViewerOutput;

namespace olive {

/**
 * @brief NodeParamViewConnectedLabel 类是一个用于在节点参数视图中显示输入连接信息的控件。
 *
 * 当一个节点的输入参数连接到另一个节点的输出时，此控件会显示连接的源节点信息。
 * 它通常包含一个可点击的标签，点击后可能会选中源节点或显示源节点的输出值。
 * 内部可能使用一个 NodeValueTree 来展示连接源的详细数值（如果适用）。
 */
class NodeParamViewConnectedLabel : public QWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param input 此标签关联的节点输入参数 (NodeInput)。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeParamViewConnectedLabel(NodeInput input, QWidget *parent = nullptr);

  /**
   * @brief 设置关联的查看器输出。
   *
   * 查看器输出可能用于获取上下文信息或触发某些与查看器相关的操作。
   * @param viewer 指向 ViewerOutput 对象的指针。
   */
  void SetViewerNode(ViewerOutput *viewer);

 signals:
  /**
   * @brief 当用户请求选中某个节点时（通常通过点击此标签）发出此信号。
   * @param n 指向要选中的 Node 对象的指针。
   */
  void RequestSelectNode(Node *n);

 private slots:
  /**
   * @brief 当此标签所代表的输入参数被连接到另一个节点的输出时调用的槽函数。
   * @param output 提供输出的节点指针。
   * @param input 被连接的输入参数 (NodeInput)，用于确认是此标签对应的输入。
   */
  void InputConnected(Node *output, const NodeInput &input);

  /**
   * @brief 当此标签所代表的输入参数的连接被断开时调用的槽函数。
   * @param output 先前提供输出的节点指针。
   * @param input 被断开连接的输入参数 (NodeInput)。
   */
  void InputDisconnected(Node *output, const NodeInput &input);

  /**
   * @brief 显示标签的上下文菜单。
   *
   * 用户右键点击标签时可能会调用此槽。
   */
  void ShowLabelContextMenu();

  /**
   * @brief 当连接标签被点击时调用的槽函数。
   *
   * 通常会发出 RequestSelectNode 信号以选中连接的源节点。
   */
  void ConnectionClicked();

 private:
  /**
   * @brief 更新标签显示的文本。
   *
   * 例如，根据当前连接状态显示“未连接”或连接到的节点名称和输出名称。
   */
  void UpdateLabel();

  /**
   * @brief 更新内部 NodeValueTree 的内容。
   *
   * 如果输入已连接，可能会显示连接源节点的输出值。
   */
  void UpdateValueTree();

  /**
   * @brief 创建内部的 NodeValueTree 实例。
   */
  void CreateTree();

  ClickableLabel *connected_to_lbl_; ///< 用于显示连接信息的自定义可点击标签。

  NodeInput input_; ///< 此控件关联的节点输入参数。

  Node *connected_node_; ///< 指向当前连接到的源节点的指针 (如果已连接)。

  NodeValueTree *value_tree_; ///< 用于显示连接源节点输出值的树形视图 (可能默认隐藏)。

  ViewerOutput *viewer_; ///< 指向关联的查看器输出对象的指针。

 private slots:
  /**
   * @brief 设置 NodeValueTree 的可见性。
   *
   * 通常由上下文菜单中的某个动作触发，用于展开/折叠显示连接源的详细值。
   * @param e 如果为 true，则显示值树；否则隐藏。
   */
  void SetValueTreeVisible(bool e);
};

}  // namespace olive

#endif  // NODEPARAMVIEWCONNECTEDLABEL_H
