#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QGraphicsView>  // Qt 图形视图基类
#include <QTimer>         // Qt 定时器类 (用途待确认，可能用于延迟更新等)

#include "core.h"                                    // 项目核心定义 (可能包含 Project, Node::ContextPair 等)
#include "node/group/group.h"                        // 节点组类
#include "nodeviewcontext.h"                         // 节点视图上下文项 (可能代表一个显示的节点或组)
#include "nodeviewedge.h"                            // 节点视图中的连接线项
#include "nodeviewminimap.h"                         // 节点视图的迷你地图控件
#include "nodeviewscene.h"                           // 节点视图的自定义 QGraphicsScene
#include "widget/handmovableview/handmovableview.h"  // 可手型工具平移的视图基类 (NodeView 的父类)
#include "widget/menu/menu.h"                        // 自定义菜单类

// Qt 类的前向声明 (根据用户要求，不添加)
// class QWidget;
// class QKeyEvent;
// class QMouseEvent;
// class QDragEnterEvent;
// class QDragMoveEvent;
// class QDropEvent;
// class QDragLeaveEvent;
// class QResizeEvent;
// class QWheelEvent;
// class QEvent;
// class QObject;
// class QPoint;
// class QPointF;
// class QAction;
// class QRectF; // NodeViewScene 可能使用
// class QMap;   // 成员变量中使用
// class QVector; // 成员变量和函数参数中使用
// class QString; // Node::ContextPair 可能使用
// class MultiUndoCommand; // 用于撤销/重做

// 项目内部类的前向声明 (根据用户要求，不添加)
// class Node; // 已在 "node/node.h" (通过 group.h 间接包含) 或 "core.h" 中定义
// class NodeViewItem; // NodeViewScene 或 NodeViewContext 可能定义或使用
// class NodeInput; // NodeViewEdge 可能使用
// class Project; // 已在 "core.h" 中定义 (假设)

namespace olive {

/**
 * @brief NodeView 类是一个用于查看和编辑节点图的控件。
 *
 * 此控件接收一个或多个上下文（通常是 NodeGraph 或 NodeGroup 对象），
 * 并构造一个 QGraphicsScene 来表示其数据，允许用户查看和修改节点图。
 * 它支持节点的创建、连接、删除、分组、复制粘贴、通过小地图导航、
 * 拖放操作以及上下文菜单等功能。
 */
class NodeView : public HandMovableView {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit NodeView(QWidget *parent = nullptr);

  /**
   * @brief 析构函数。
   */
  ~NodeView() override;

  /**
   * @brief 设置要在视图中显示的上下文（通常是节点图或节点组）。
   * @param nodes 包含要显示的 Node 指针的 QVector。
   */
  void SetContexts(const QVector<Node *> &nodes);

  /**
   * @brief 获取当前视图正在显示的上下文列表。
   *
   * 如果存在覆盖视图 (overlay_view_)，则返回覆盖视图的上下文。
   * @return 返回一个 const 引用，指向包含 Node 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<Node *> &GetContexts() const {
    if (overlay_view_) {                    // 如果存在覆盖视图
      return overlay_view_->GetContexts();  // 返回覆盖视图的上下文
    } else {
      return contexts_;  // 否则返回主视图的上下文
    }
  }

  /**
   * @brief 检查当前是否处于组覆盖模式（即正在显示一个节点组的内部）。
   * @return 如果 overlay_view_ 存在，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsGroupOverlay() const { return overlay_view_; }

  /**
   * @brief 关闭属于指定项目的所有上下文。
   *
   * 当项目关闭时，用于清理与该项目相关的节点图显示。
   * @param project 指向 Project 对象的指针。
   */
  void CloseContextsBelongingToProject(Project *project);

  /**
   * @brief 清除当前节点图中的所有图形项。
   */
  void ClearGraph();

  /**
   * @brief 从图中删除选中的节点（用户友好的/可撤销的操作）。
   */
  void DeleteSelected();

  /**
   * @brief 选中视图中的所有节点项。
   */
  void SelectAll();
  /**
   * @brief 取消选中视图中的所有节点项。
   */
  void DeselectAll();

  /**
   * @brief 选中指定的节点列表。
   * @param nodes 包含 Node::ContextPair 的 QVector，指定要选中的节点及其上下文。
   * @param center_view_on_item 布尔值，指示是否在选中后将视图居中到选中的项。
   */
  void Select(const QVector<Node::ContextPair> &nodes, bool center_view_on_item);

  /**
   * @brief 复制当前选中的节点（支持剪切操作）。
   * @param cut 如果为 true，则执行剪切操作（复制后删除）。
   */
  void CopySelected(bool cut);
  /**
   * @brief 粘贴之前复制或剪切的节点。
   */
  void Paste();

  /**
   * @brief 复制并粘贴当前选中的节点（创建副本）。
   */
  void Duplicate();

  /**
   * @brief 为当前选中的节点设置颜色标签。
   * @param index 颜色标签的索引。
   */
  void SetColorLabel(int index);

  /**
   * @brief 放大视图。
   */
  void ZoomIn();

  /**
   * @brief 缩小视图。
   */
  void ZoomOut();

  /**
   * @brief 获取当前主视图（非覆盖视图）的上下文列表。
   * @return 返回一个 const 引用，指向包含 Node 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<Node *> &GetCurrentContexts() const { return contexts_; }

 public slots:
  /**
   * @brief 设置迷你地图的启用状态。
   * @param e 如果为 true，则显示迷你地图；否则隐藏。
   */
  void SetMiniMapEnabled(bool e) { minimap_->setVisible(e); }

  /**
   * @brief 显示“添加节点”菜单。
   *
   * 菜单会显示在当前鼠标光标位置。
   */
  void ShowAddMenu() {
    Menu *m = CreateAddMenu(nullptr);  // 创建添加菜单
    m->exec(QCursor::pos());           // 在鼠标位置执行菜单
    delete m;                          // 删除菜单对象
  }

  /**
   * @brief 将视图居中到所有图形项的包围盒。
   */
  void CenterOnItemsBoundingRect();

  /**
   * @brief 将视图居中到指定的节点。
   * @param n 指向要居中显示的 olive::Node 对象的指针。
   */
  void CenterOnNode(olive::Node *n);

  /**
   * @brief 为选中的节点应用标签（可能指颜色标签或其他标识）。
   */
  void LabelSelectedNodes();

 signals:
  /**
   * @brief 当一组节点被选中时发出此信号。
   * @param nodes 包含被选中的 Node 指针的 QVector。
   */
  void NodesSelected(const QVector<Node *> &nodes);

  /**
   * @brief 当一组节点被取消选中时发出此信号。
   * @param nodes 包含被取消选中的 Node 指针的 QVector。
   */
  void NodesDeselected(const QVector<Node *> &nodes);

  /**
   * @brief 当节点的选择状态发生改变时发出此信号。
   * @param nodes 包含当前所有被选中的 Node 指针的 QVector。
   */
  void NodeSelectionChanged(const QVector<Node *> &nodes);
  /**
   * @brief 当节点的选择状态（包含上下文信息）发生改变时发出此信号。
   * @param nodes 包含 Node::ContextPair 的 QVector，表示选中的节点及其上下文。
   */
  void NodeSelectionChangedWithContexts(const QVector<Node::ContextPair> &nodes);

  /**
   * @brief 当一个节点组被打开（其内部内容在视图中显示，可能通过覆盖视图）时发出此信号。
   * @param group 指向被打开的 NodeGroup 对象的指针。
   */
  void NodeGroupOpened(NodeGroup *group);
  /**
   * @brief 当一个节点组被关闭（其内部内容不再显示）时发出此信号。
   */
  void NodeGroupClosed();

  /**
   * @brief 当用户在视图中按下 Esc 键时发出此信号。
   */
  void EscPressed();

 protected:
  /**
   * @brief 重写键盘按下事件处理函数。
   * @param event 键盘事件指针。
   */
  void keyPressEvent(QKeyEvent *event) override;

  /**
   * @brief 重写鼠标按下事件处理函数。
   * @param event 鼠标事件指针。
   */
  void mousePressEvent(QMouseEvent *event) override;
  /**
   * @brief 重写鼠标移动事件处理函数。
   * @param event 鼠标事件指针。
   */
  void mouseMoveEvent(QMouseEvent *event) override;
  /**
   * @brief 重写鼠标释放事件处理函数。
   * @param event 鼠标事件指针。
   */
  void mouseReleaseEvent(QMouseEvent *event) override;
  /**
   * @brief 重写鼠标双击事件处理函数。
   * @param event 鼠标事件指针。
   */
  void mouseDoubleClickEvent(QMouseEvent *event) override;

  /**
   * @brief 重写拖拽进入事件处理函数。
   * @param event 拖拽进入事件指针。
   */
  void dragEnterEvent(QDragEnterEvent *event) override;
  /**
   * @brief 重写拖拽移动事件处理函数。
   * @param event 拖拽移动事件指针。
   */
  void dragMoveEvent(QDragMoveEvent *event) override;
  /**
   * @brief 重写放下事件处理函数。
   * @param event 放下事件指针。
   */
  void dropEvent(QDropEvent *event) override;
  /**
   * @brief 重写拖拽离开事件处理函数。
   * @param event 拖拽离开事件指针。
   */
  void dragLeaveEvent(QDragLeaveEvent *event) override;

  /**
   * @brief 重写控件大小调整事件处理函数。
   * @param event 尺寸调整事件指针。
   */
  void resizeEvent(QResizeEvent *event) override;

  /**
   * @brief 重写基类的 ZoomIntoCursorPosition 方法，实现围绕光标位置进行缩放。
   * @param event 原始的鼠标滚轮事件指针。
   * @param multiplier 缩放乘数。
   * @param cursor_pos 鼠标光标在场景中的位置。
   */
  void ZoomIntoCursorPosition(QWheelEvent *event, double multiplier, const QPointF &cursor_pos) override;

  /**
   * @brief 重写 QObject 的 event 方法，用于通用事件处理。
   * @param event 事件指针。
   * @return 如果事件被处理，则返回 true；否则返回 false。
   */
  bool event(QEvent *event) override;

  /**
   * @brief 重写 QObject 的 eventFilter 方法，用于过滤其他对象的事件。
   * @param object 被观察的对象。
   * @param event 发生的事件。
   * @return 如果事件被过滤，则返回 true；否则返回 false。
   */
  bool eventFilter(QObject *object, QEvent *event) override;

  /**
   * @brief 重写 QWidget 的 changeEvent 方法，用于响应状态改变事件（如语言更改）。
   * @param e 事件指针。
   */
  void changeEvent(QEvent *e) override;

 private:
  /**
   * @brief 将当前附着到鼠标光标的项从光标分离。
   * @param delete_nodes_too 如果为 true，则在分离的同时删除这些节点。默认为 true。
   */
  void DetachItemsFromCursor(bool delete_nodes_too = true);

  /**
   * @brief 设置节点图的流向（布局方向）。
   * @param dir NodeViewCommon::FlowDirection 枚举值，指定新的流向。
   */
  void SetFlowDirection(NodeViewCommon::FlowDirection dir);

  /**
   * @brief 将当前附着到光标的节点移动到指定的光标位置。
   * @param p 鼠标光标在视图坐标系中的位置。
   */
  void MoveAttachedNodesToCursor(const QPoint &p);
  /**
   * @brief 处理正在移动的附着节点的位置更新。
   * @param pos 鼠标当前位置。
   */
  void ProcessMovingAttachedNodes(const QPoint &pos);
  /**
   * @brief 处理放下（释放）附着到光标的节点。
   * @param command 用于记录此次操作的 MultiUndoCommand 指针（以便撤销）。
   * @param select_context 节点被放下时所在的上下文 Node 指针。
   * @param pos 放下时的鼠标位置。
   * @return 返回一个 QVector，包含被放下并添加到图中的 Node 指针。
   */
  QVector<Node *> ProcessDroppingAttachedNodes(MultiUndoCommand *command, Node *select_context, const QPoint &pos);
  /**
   * @brief 获取指定鼠标位置下的上下文节点（例如，如果鼠标在一个组节点上，则返回该组节点）。
   * @param p 鼠标在视图坐标系中的位置。
   * @return 如果鼠标位置下有上下文节点，则返回 Node 指针；否则返回 nullptr。
   */
  Node *GetContextAtMousePos(const QPoint &p);

  /**
   * @brief 连接场景选择改变信号到内部槽函数。
   */
  void ConnectSelectionChangedSignal();
  /**
   * @brief 断开场景选择改变信号的连接。
   */
  void DisconnectSelectionChangedSignal();

  /**
   * @brief 通过键盘（例如 +/- 键）进行缩放。
   * @param multiplier 缩放乘数。
   */
  void ZoomFromKeyboard(double multiplier);

  /**
   * @brief 如果有必要（例如，用户取消了连接操作），清除正在创建的边的输入端信息。
   */
  void ClearCreateEdgeInputIfNecessary();

  /**
   * @brief 估算一个节点项在指定上下文中的理想放置位置。
   * @param item 目标 NodeViewItem 指针。
   * @param context 目标上下文 Node 指针。
   * @return 返回估算的 QPointF 位置。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  QPointF GetEstimatedPositionForContext(NodeViewItem *item, Node *context) const;

  /**
   * @brief 获取与选定节点关联的假定 NodeViewItem。
   * @param node 选定的 Node 指针。
   * @return 如果找到，则返回 NodeViewItem 指针；否则返回 nullptr。
   */
  NodeViewItem *GetAssumedItemForSelectedNode(Node *node);
  /**
   * @brief 获取选定节点的假定位置。
   * @param node 选定的 Node 指针。
   * @param pos 用于接收位置的 Node::Position 指针。
   * @return 如果成功获取位置，则返回 true。
   */
  bool GetAssumedPositionForSelectedNode(Node *node, Node::Position *pos);

  /**
   * @brief 创建一个用于添加新节点的菜单。
   * @param parent 父 Menu 指针 (如果作为子菜单)。
   * @return 返回创建的 Menu 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  Menu *CreateAddMenu(Menu *parent) const;

  /**
   * @brief 在拖拽创建新连接线时，根据鼠标位置定位连接线的末端。
   * @param pos 鼠标当前位置。
   */
  void PositionNewEdge(const QPoint &pos);

  /**
   * @brief 将一个节点添加为视图的上下文。
   * @param n 要添加的 Node 指针。
   */
  void AddContext(Node *n);

  /**
   * @brief 从视图的上下文中移除一个节点。
   * @param n 要移除的 Node 指针。
   */
  void RemoveContext(Node *n);

  /**
   * @brief 检查指定的节点项当前是否附着到鼠标光标（例如，在拖拽创建或粘贴时）。
   * @param item 要检查的 NodeViewItem 指针。
   * @return 如果已附着，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  bool IsItemAttachedToCursor(NodeViewItem *item) const;

  /**
   * @brief 展开指定的节点项（如果它是一个可折叠的组）。
   * @param item 要展开的 NodeViewItem 指针。
   */
  static void ExpandItem(NodeViewItem *item);

  /**
   * @brief 折叠指定的节点项（如果它是一个可折叠的组）。
   * @param item 要折叠的 NodeViewItem 指针。
   */
  static void CollapseItem(NodeViewItem *item);

  /**
   * @brief 结束边的拖拽操作（创建新连接）。
   * @param cancel 如果为 true，则取消边的创建；否则尝试完成连接。默认为 false。
   */
  void EndEdgeDrag(bool cancel = false);

  /**
   * @brief 在粘贴操作完成后执行的后续处理。
   * @param new_nodes 包含新粘贴的 Node 指针的 QVector。
   * @param map 包含原始节点到新粘贴节点映射的 Node::PositionMap。
   */
  void PostPaste(const QVector<Node *> &new_nodes, const Node::PositionMap &map);

  /**
   * @brief 当主视图大小改变时，调整覆盖视图的大小。
   */
  void ResizeOverlay();

  NodeViewMiniMap *minimap_;  ///< 指向迷你地图控件的指针。

  /**
   * @brief 静态辅助函数，从 NodeViewItem 获取其所属的 NodeViewContext。
   * @param item NodeViewItem 指针。
   * @return 返回 NodeViewContext 指针，如果找不到则可能为 nullptr。
   */
  static NodeViewContext *GetContextItemFromNodeItem(NodeViewItem *item);

  /**
   * @brief AttachedItem 结构体用于存储附着到鼠标光标的节点项及其相关信息。
   */
  struct AttachedItem {
    NodeViewItem *item{};  ///< 指向附着的 NodeViewItem。
    Node *node{};          ///< 指向附着的实际 Node 对象。
    QPointF original_pos;  ///< 附着前节点的原始位置。
  };

  /**
   * @brief 设置当前附着到鼠标光标的项列表。
   * @param items 包含 AttachedItem 的 QVector。
   */
  void SetAttachedItems(const QVector<AttachedItem> &items);
  QVector<AttachedItem> attached_items_;  ///< 当前附着到鼠标光标的项列表。

  NodeViewEdge *drop_edge_;  ///< 在拖拽创建连接时，表示正在拖拽的边的图形项。
  NodeInput drop_input_;     ///< 在拖拽创建连接到输入端点时，存储目标输入端点的信息。

  NodeViewEdge *create_edge_;              ///< 用户当前正在拖拽以创建的连接线。
  NodeViewItem *create_edge_output_item_;  ///< 创建连接线时，其输出端所连接的节点项。
  NodeViewItem *create_edge_input_item_;   ///< 创建连接线时，其输入端所连接的节点项。
  NodeInput create_edge_input_;            ///< 创建连接线时，其目标输入端点。
  bool create_edge_already_exists_{};      ///< 标记正在尝试创建的连接是否已存在。
  bool create_edge_from_output_{};         ///< 标记创建连接的操作是否从输出端点开始。

  QVector<NodeViewItem *> create_edge_expanded_items_;  ///< 在创建连接过程中，临时展开的节点组列表。

  NodeViewScene scene_;  ///< 此视图关联的自定义 QGraphicsScene 实例。

  QVector<Node *> selected_nodes_;  ///< 缓存当前选中的节点列表。

  QVector<Node *> contexts_;               ///< 当前视图显示的主要上下文（节点图或节点组）列表。
  QVector<Node *> last_set_filter_nodes_;  ///< 上一次设置的用于过滤显示的节点列表。
  QMap<Node *, QPointF> context_offsets_;  ///< 存储每个上下文在视图中的偏移量。

  QMap<NodeViewItem *, QPointF> dragging_items_;  ///< 存储当前正在被用户拖动的节点项及其起始拖动位置的映射。

  NodeView *overlay_view_;  ///< 指向覆盖视图的指针，用于显示节点组内部等。

  double scale_;  ///< 当前视图的缩放级别。

  bool dont_emit_selection_signals_;  ///< 标记是否应临时禁止发出选择改变信号（例如，在内部批量操作时）。

  static const double kMinimumScale;  ///< 视图允许的最小缩放级别。

  static const int kMaximumContexts;  ///< 视图允许同时显示的最大上下文数量。

 private slots:
  /**
   * @brief 当场景中的选中项发生改变时调用的槽函数。
   *
   * 用于更新内部的 selected_nodes_ 缓存并发出相应的选择改变信号。
   */
  void UpdateSelectionCache();

  /**
   * @brief 当用户请求上下文菜单时（例如右键点击视图空白处或节点项）调用的槽函数。
   * @param pos 请求上下文菜单的鼠标位置（视图坐标系）。
   */
  void ShowContextMenu(const QPoint &pos);

  /**
   * @brief 当用户从“添加节点”菜单中选择一项以创建新节点时调用的槽函数。
   * @param action 被触发的 QAction 指针，其数据通常包含要创建的节点的类型信息。
   */
  void CreateNodeSlot(QAction *action);

  /**
   * @brief 当用户从上下文菜单中选择设置节点图流向时调用的槽函数。
   * @param action 被触发的 QAction 指针，其数据指示新的流向。
   */
  void ContextMenuSetDirection(QAction *action);

  /**
   * @brief 在查看器中打开选中的节点（如果适用）。
   */
  void OpenSelectedNodeInViewer();

  /**
   * @brief 更新场景的包围盒矩形。
   */
  void UpdateSceneBoundingRect();

  /**
   * @brief 根据主视图的当前视口重新定位迷你地图。
   */
  void RepositionMiniMap();

  /**
   * @brief 更新迷你地图上表示主视图当前可见区域的矩形。
   */
  void UpdateViewportOnMiniMap();

  /**
   * @brief 将视图的中心移动到指定的场景点。
   * @param pos 目标场景点的 QPointF 坐标。
   */
  void MoveToScenePoint(const QPointF &pos);

  /**
   * @brief 当节点从图中被移除（例如，通过 DeleteSelected 或其他方式）时调用的槽函数。
   */
  void NodeRemovedFromGraph();

  /**
   * @brief 将当前选中的节点组合成一个新的节点组。
   */
  void GroupNodes();

  /**
   * @brief 将当前选中的节点组解散，将其内部节点释放到当前上下文中。
   */
  void UngroupNodes();

  /**
   * @brief 显示选中节点的属性（可能会打开属性编辑器面板）。
   */
  void ShowNodeProperties();

  /**
   * @brief 当一个 NodeViewItem 即将被从场景中删除时调用的槽函数。
   * @param item 指向即将被删除的 NodeViewItem 的指针。
   */
  void ItemAboutToBeDeleted(NodeViewItem *item);

  /**
   * @brief 关闭当前的覆盖视图（例如，退出节点组编辑模式）。
   */
  void CloseOverlay();
};

}  // namespace olive

#endif  // NODEVIEW_H
