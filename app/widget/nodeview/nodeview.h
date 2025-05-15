

#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QGraphicsView>
#include <QTimer>

#include "core.h"
#include "node/group/group.h"
#include "nodeviewcontext.h"
#include "nodeviewedge.h"
#include "nodeviewminimap.h"
#include "nodeviewscene.h"
#include "widget/handmovableview/handmovableview.h"
#include "widget/menu/menu.h"

namespace olive {

/**
 * @brief A widget for viewing and editing node graphs
 *
 * This widget takes a NodeGraph object and constructs a QGraphicsScene representing its data, viewing and allowing
 * the user to make modifications to it.
 */
class NodeView : public HandMovableView {
  Q_OBJECT
 public:
  explicit NodeView(QWidget *parent = nullptr);

  ~NodeView() override;

  void SetContexts(const QVector<Node *> &nodes);

  [[nodiscard]] const QVector<Node *> &GetContexts() const {
    if (overlay_view_) {
      return overlay_view_->GetContexts();
    } else {
      return contexts_;
    }
  }

  [[nodiscard]] bool IsGroupOverlay() const { return overlay_view_; }

  void CloseContextsBelongingToProject(Project *project);

  void ClearGraph();

  /**
   * @brief Delete selected nodes from graph (user-friendly/undoable)
   */
  void DeleteSelected();

  void SelectAll();
  void DeselectAll();

  void Select(const QVector<Node::ContextPair> &nodes, bool center_view_on_item);

  void CopySelected(bool cut);
  void Paste();

  void Duplicate();

  void SetColorLabel(int index);

  void ZoomIn();

  void ZoomOut();

  [[nodiscard]] const QVector<Node *> &GetCurrentContexts() const { return contexts_; }

 public slots:
  void SetMiniMapEnabled(bool e) { minimap_->setVisible(e); }

  void ShowAddMenu() {
    Menu *m = CreateAddMenu(nullptr);
    m->exec(QCursor::pos());
    delete m;
  }

  void CenterOnItemsBoundingRect();

  void CenterOnNode(olive::Node *n);

  void LabelSelectedNodes();

 signals:
  void NodesSelected(const QVector<Node *> &nodes);

  void NodesDeselected(const QVector<Node *> &nodes);

  void NodeSelectionChanged(const QVector<Node *> &nodes);
  void NodeSelectionChangedWithContexts(const QVector<Node::ContextPair> &nodes);

  void NodeGroupOpened(NodeGroup *group);
  void NodeGroupClosed();

  void EscPressed();

 protected:
  void keyPressEvent(QKeyEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;

  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;

  void resizeEvent(QResizeEvent *event) override;

  void ZoomIntoCursorPosition(QWheelEvent *event, double multiplier, const QPointF &cursor_pos) override;

  bool event(QEvent *event) override;

  bool eventFilter(QObject *object, QEvent *event) override;

  void changeEvent(QEvent *e) override;

 private:
  void DetachItemsFromCursor(bool delete_nodes_too = true);

  void SetFlowDirection(NodeViewCommon::FlowDirection dir);

  void MoveAttachedNodesToCursor(const QPoint &p);
  void ProcessMovingAttachedNodes(const QPoint &pos);
  QVector<Node *> ProcessDroppingAttachedNodes(MultiUndoCommand *command, Node *select_context, const QPoint &pos);
  Node *GetContextAtMousePos(const QPoint &p);

  void ConnectSelectionChangedSignal();
  void DisconnectSelectionChangedSignal();

  void ZoomFromKeyboard(double multiplier);

  void ClearCreateEdgeInputIfNecessary();

  QPointF GetEstimatedPositionForContext(NodeViewItem *item, Node *context) const;

  NodeViewItem *GetAssumedItemForSelectedNode(Node *node);
  bool GetAssumedPositionForSelectedNode(Node *node, Node::Position *pos);

  Menu *CreateAddMenu(Menu *parent) const;

  void PositionNewEdge(const QPoint &pos);

  void AddContext(Node *n);

  void RemoveContext(Node *n);

  bool IsItemAttachedToCursor(NodeViewItem *item) const;

  static void ExpandItem(NodeViewItem *item);

  static void CollapseItem(NodeViewItem *item);

  void EndEdgeDrag(bool cancel = false);

  void PostPaste(const QVector<Node *> &new_nodes, const Node::PositionMap &map);

  void ResizeOverlay();

  NodeViewMiniMap *minimap_;

  static NodeViewContext *GetContextItemFromNodeItem(NodeViewItem *item);

  struct AttachedItem {
    NodeViewItem *item{};
    Node *node{};
    QPointF original_pos;
  };

  void SetAttachedItems(const QVector<AttachedItem> &items);
  QVector<AttachedItem> attached_items_;

  NodeViewEdge *drop_edge_;
  NodeInput drop_input_;

  NodeViewEdge *create_edge_;
  NodeViewItem *create_edge_output_item_;
  NodeViewItem *create_edge_input_item_;
  NodeInput create_edge_input_;
  bool create_edge_already_exists_{};
  bool create_edge_from_output_{};

  QVector<NodeViewItem *> create_edge_expanded_items_;

  NodeViewScene scene_;

  QVector<Node *> selected_nodes_;

  QVector<Node *> contexts_;
  QVector<Node *> last_set_filter_nodes_;
  QMap<Node *, QPointF> context_offsets_;

  QMap<NodeViewItem *, QPointF> dragging_items_;

  NodeView *overlay_view_;

  double scale_;

  bool dont_emit_selection_signals_;

  static const double kMinimumScale;

  static const int kMaximumContexts;

 private slots:
  /**
   * @brief Receiver for when the scene's selected items change
   */
  void UpdateSelectionCache();

  /**
   * @brief Receiver for when the user right clicks (or otherwise requests a context menu)
   */
  void ShowContextMenu(const QPoint &pos);

  /**
   * @brief Receiver for when the user requests a new node from the add menu
   */
  void CreateNodeSlot(QAction *action);

  /**
   * @brief Receiver for setting the direction from the context menu
   */
  void ContextMenuSetDirection(QAction *action);

  /**
   * @brief Opens the selected node in a Viewer
   */
  void OpenSelectedNodeInViewer();

  void UpdateSceneBoundingRect();

  void RepositionMiniMap();

  void UpdateViewportOnMiniMap();

  void MoveToScenePoint(const QPointF &pos);

  void NodeRemovedFromGraph();

  void GroupNodes();

  void UngroupNodes();

  void ShowNodeProperties();

  void ItemAboutToBeDeleted(NodeViewItem *item);

  void CloseOverlay();
};

}  // namespace olive

#endif  // NODEVIEW_H
