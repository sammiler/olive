#ifndef NODEPANEL_H  // 防止头文件被重复包含的宏
#define NODEPANEL_H  // 定义 NODEPANEL_H 宏

#include "panel/panel.h"                 // 包含 PanelWidget 基类的定义
#include "widget/nodeview/nodewidget.h"  // 包含 NodeWidget 控件的定义 (NodeWidget 内部通常包含 NodeView)

namespace olive {  // olive 项目的命名空间

/**
 * @brief NodePanel 类是一个 PanelWidget 的包装器，用于封装和管理一个 NodeWidget (节点编辑器控件)。
 *
 * 这个面板提供了用户与节点图进行可视化交互的界面。用户可以在这个面板中创建、连接、
 * 修改和组织节点，以构建视频处理流程。
 * 它通常会包含缩放、平移、选择、复制、粘贴、删除节点等功能。
 */
class NodePanel : public PanelWidget {  // NodePanel 继承自 PanelWidget
 Q_OBJECT                               // 声明此类使用 Qt 的元对象系统

     public :
     // 构造函数
     NodePanel();

  /**
   * @brief 获取内部封装的 NodeWidget 控件的指针。
   * @return 返回 NodeWidget 指针。
   */
  [[nodiscard]] NodeWidget *GetNodeWidget() const { return node_widget_; }

  /**
   * @brief 获取当前节点编辑器视图中显示的上下文节点列表。
   * 上下文节点通常指用户当前正在编辑其内部图的节点 (例如一个 NodeGroup)。
   * @return 返回一个包含上下文 Node 指针的 QVector 的常量引用。
   */
  [[nodiscard]] const QVector<Node *> &GetContexts() const { return node_widget_->view()->GetContexts(); }

  /**
   * @brief 检查节点编辑器视图当前是否处于“组覆盖”模式。
   * 组覆盖模式可能意味着视图正在显示一个节点组的内部，并且可能有一些特殊的显示或行为。
   * @return 如果是组覆盖模式，则返回 true。
   */
  [[nodiscard]] bool IsGroupOverlay() const { return node_widget_->view()->IsGroupOverlay(); }

  /**
   * @brief 设置节点编辑器视图的上下文节点。
   * 这会使节点编辑器显示指定节点 (通常是 NodeGroup) 的内部图。
   * @param nodes 包含要设置为上下文的节点指针的 QVector。通常只包含一个节点。
   */
  void SetContexts(const QVector<Node *> &nodes) { node_widget_->SetContexts(nodes); }

  /**
   * @brief 关闭所有属于指定项目的上下文。
   * 例如，当关闭一个项目时，需要关闭该项目中所有已打开的节点组编辑器。
   * @param project 要关闭其上下文的项目指针。
   */
  void CloseContextsBelongingToProject(Project *project) {
    node_widget_->view()->CloseContextsBelongingToProject(project);
  }

  // --- 重写 PanelWidget 或通用编辑接口的方法 ---

  /**
   * @brief 全选节点编辑器视图中的所有可选对象 (通常是节点)。
   */
  void SelectAll() override { node_widget_->view()->SelectAll(); }

  /**
   * @brief 取消选择节点编辑器视图中的所有对象。
   */
  void DeselectAll() override { node_widget_->view()->DeselectAll(); }

  /**
   * @brief 删除节点编辑器视图中当前选中的对象。
   */
  void DeleteSelected() override { node_widget_->view()->DeleteSelected(); }

  /**
   * @brief 剪切节点编辑器视图中当前选中的对象。
   * 内部调用 NodeView::CopySelected(true) 来实现剪切 (复制并标记为剪切)。
   */
  void CutSelected() override { node_widget_->view()->CopySelected(true); }

  /**
   * @brief 复制节点编辑器视图中当前选中的对象。
   * 内部调用 NodeView::CopySelected(false) 来实现复制。
   */
  void CopySelected() override { node_widget_->view()->CopySelected(false); }

  /**
   * @brief 在节点编辑器视图中执行粘贴操作。
   */
  void Paste() override { node_widget_->view()->Paste(); }

  /**
   * @brief 复制并粘贴 (创建副本) 节点编辑器视图中当前选中的对象。
   */
  void Duplicate() override { node_widget_->view()->Duplicate(); }

  /**
   * @brief 为节点编辑器视图中当前选中的节点设置颜色标签。
   * @param index 颜色标签的索引。
   */
  void SetColorLabel(int index) override { node_widget_->view()->SetColorLabel(index); }

  /**
   * @brief 放大节点编辑器视图。
   */
  void ZoomIn() override { node_widget_->view()->ZoomIn(); }

  /**
   * @brief 缩小节点编辑器视图。
   */
  void ZoomOut() override { node_widget_->view()->ZoomOut(); }

  /**
   * @brief 重命名节点编辑器视图中当前选中的节点 (通常是修改其标签)。
   */
  void RenameSelected() override { node_widget_->view()->LabelSelectedNodes(); }

 public slots:  // Qt 公有槽函数
  /**
   * @brief 以编程方式选择指定的节点 (及其在上下文中的配对)。
   * @param p 包含要选择的 Node::ContextPair 的 QVector。
   *          ContextPair 包含了节点本身和它所在的上下文节点。
   */
  void Select(const QVector<Node::ContextPair> &p) { node_widget_->view()->Select(p, true); }

 signals:  // Qt 信号声明
  /**
   * @brief 当有节点被选中时发出的信号。
   * @param nodes 被选中的节点列表。
   */
  void NodesSelected(const QVector<Node *> &nodes);

  /**
   * @brief 当有节点被取消选中时发出的信号。
   * @param nodes 被取消选中的节点列表。
   */
  void NodesDeselected(const QVector<Node *> &nodes);

  /**
   * @brief 当节点选择状态发生改变时发出的信号。
   * @param nodes 当前所有被选中的节点列表。
   */
  void NodeSelectionChanged(const QVector<Node *> &nodes);
  /**
   * @brief 当节点选择状态发生改变时发出的信号 (包含上下文信息)。
   * @param nodes 当前所有被选中的 Node::ContextPair 列表。
   */
  void NodeSelectionChangedWithContexts(const QVector<Node::ContextPair> &nodes);

  /**
   * @brief 当一个节点组被打开 (其内部图在节点编辑器中显示) 时发出的信号。
   * @param group 被打开的 NodeGroup 指针。
   */
  void NodeGroupOpened(NodeGroup *group);

  /**
   * @brief 当一个节点组被关闭 (其内部图不再显示) 时发出的信号。
   */
  void NodeGroupClosed();

 private:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板的标题。
   */
  void Retranslate() override { SetTitle(tr("Node Editor")); }  // 设置面板标题为 "Node Editor" (可翻译)

  NodeWidget *node_widget_;  // 指向内部的 NodeWidget 控件的指针
                             // NodeWidget 负责实际的节点图显示和交互。
};

}  // namespace olive

#endif  // NODEPANEL_H