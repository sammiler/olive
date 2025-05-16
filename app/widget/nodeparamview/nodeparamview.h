#ifndef NODEPARAMVIEW_H
#define NODEPARAMVIEW_H

#include <QVBoxLayout> // Qt 垂直布局类
#include <QWidget>     // Qt 控件基类

#include "node/group/group.h"                        // 节点组类
#include "node/node.h"                               // 节点基类
#include "node/project/serializer/serializer.h"      // 项目序列化相关，用于复制粘贴
#include "nodeparamviewcontext.h"                    // 节点参数视图上下文类
#include "nodeparamviewdockarea.h"                   // 节点参数视图停靠区域类
#include "nodeparamviewitem.h"                       // 节点参数视图中的单个参数项
#include "widget/keyframeview/keyframeview.h"        // 关键帧视图控件
#include "widget/timebased/timebasedwidget.h"      // 基于时间的控件基类

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QScrollBar;
// class QScrollArea;
// class QResizeEvent;
// class QHash;
// class QString;
// class QVector;

// 前向声明项目内部类 (根据用户要求，不添加)
// class Project;
// class ViewerOutput;
// class KeyframeViewInputConnection;
// class NodeKeyframe;
// class TimeTargetObject;
// class NodeInput;
// class rational;

namespace olive {

/**
 * @brief NodeParamView 类是用于显示和编辑节点参数的复杂控件。
 *
 * 它继承自 TimeBasedWidget，因此与时间轴同步。它可以显示多个节点（上下文）的参数，
 * 并可选地集成一个 KeyframeView 来编辑与这些参数关联的动画曲线。
 * 该视图支持节点选择、参数钉固、复制/粘贴关键帧和参数等功能。
 * 内部使用 NodeParamViewContext 来管理每个上下文（通常是一个节点）的参数显示，
 * 并使用 NodeParamViewDockArea 来组织这些参数项的布局。
 */
class NodeParamView : public TimeBasedWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param create_keyframe_view 布尔值，指示是否应创建并集成一个 KeyframeView。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeParamView(bool create_keyframe_view, QWidget *parent = nullptr);
  /**
   * @brief 构造函数重载，默认创建 KeyframeView。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeParamView(QWidget *parent = nullptr) : NodeParamView(true, parent) {}

  /**
   * @brief 析构函数。
   */
  ~NodeParamView() override;

  /**
   * @brief 关闭属于指定项目的所有上下文。
   *
   * 当项目关闭时，用于清理与该项目相关的参数显示。
   * @param p 指向 Project 对象的指针。
   */
  void CloseContextsBelongingToProject(Project *p);

  /**
   * @brief 删除当前选中的参数项或关键帧。
   */
  void DeleteSelected();

  /**
   * @brief 选中所有可见的关键帧（如果 KeyframeView 存在）。
   */
  void SelectAll() { keyframe_view_->SelectAll(); }

  /**
   * @brief 取消选中所有关键帧（如果 KeyframeView 存在）。
   */
  void DeselectAll() { keyframe_view_->DeselectAll(); }

  /**
   * @brief 设置当前选中的节点参数项。
   * @param nodes 包含 NodeParamViewItem 指针的 QVector，代表要选中的参数项。
   * @param handle_focused_node 布尔值，指示是否同时处理焦点节点的逻辑。默认为 true。
   * @param emit_signal 布尔值，指示是否在选择改变后发出 SelectedNodesChanged 信号。默认为 true。
   */
  void SetSelectedNodes(const QVector<NodeParamViewItem *> &nodes, bool handle_focused_node = true,
                        bool emit_signal = true);
  /**
   * @brief 根据节点上下文对设置选中的节点。
   * @param nodes 包含 Node::ContextPair 的 QVector，代表要选中的节点及其上下文。
   * @param emit_signal 布尔值，指示是否发出信号。默认为 true。
   */
  void SetSelectedNodes(const QVector<Node::ContextPair> &nodes, bool emit_signal = true);

  /**
   * @brief 根据节点 ID 获取节点指针。
   * @param id 要查找的节点的 ID 字符串。
   * @return 如果找到，则返回 Node 指针；否则返回 nullptr。
   */
  Node *GetNodeWithID(const QString &id);
  /**
   * @brief 根据节点 ID 获取节点指针，但会忽略指定列表中的节点。
   * @param id 要查找的节点的 ID 字符串。
   * @param ignore 包含要忽略的 Node 指针的 QVector。
   * @return 如果找到且不在忽略列表中，则返回 Node 指针；否则返回 nullptr。
   */
  Node *GetNodeWithIDAndIgnoreList(const QString &id, const QVector<Node *> &ignore);

  /**
   * @brief 获取当前在此视图中显示参数的所有上下文（通常是节点）。
   * @return 返回一个 const 引用，指向包含 Node 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<Node *> &GetContexts() const { return contexts_; }

  /**
   * @brief 复制选中的参数或关键帧（支持剪切）。
   * @param cut 如果为 true，则执行剪切操作。
   * @return 如果成功复制或剪切，则返回 true。
   */
  bool CopySelected(bool cut) override;

  /**
   * @brief 粘贴之前复制或剪切的参数或关键帧。
   * @return 如果成功粘贴，则返回 true。
   */
  bool Paste() override;
  /**
   * @brief 静态粘贴函数，允许更灵活地处理粘贴目标和映射。
   * @param parent 粘贴操作的父控件，通常用于显示任何相关的对话框。
   * @param get_existing_map_function 一个函数对象，用于根据序列化结果生成现有节点到新粘贴节点的映射。
   * @return 如果成功粘贴，则返回 true。
   */
  static bool Paste(
      QWidget *parent,
      const std::function<QHash<Node *, Node *>(const ProjectSerializer::Result &)> &get_existing_map_function);

 public slots:
  /**
   * @brief 设置要在参数视图中显示的上下文（节点）列表。
   * @param contexts 包含 Node 指针的 QVector。
   */
  void SetContexts(const QVector<Node *> &contexts);

  /**
   * @brief 更新参数元素在垂直方向上的位置（Y坐标）。
   *
   * 当参数列表布局发生变化时（例如，折叠/展开组）可能需要调用此函数。
   */
  void UpdateElementY();

 signals:
  /**
   * @brief 当焦点所在的节点参数项发生改变时发出此信号。
   * @param n 指向新获得焦点的 Node 的指针。
   */
  void FocusedNodeChanged(Node *n);

  /**
   * @brief 当选中的节点列表发生改变时发出此信号。
   * @param nodes 包含新选中的 Node::ContextPair 的 QVector。
   */
  void SelectedNodesChanged(const QVector<Node::ContextPair> &nodes);

  /**
   * @brief 请求关联的查看器开始编辑文本（例如，当用户激活文本参数的编辑时）。
   */
  void RequestViewerToStartEditingText();

 protected:
  /**
   * @brief 控件大小调整事件处理函数。
   * @param event 尺寸调整事件指针。
   */
  void resizeEvent(QResizeEvent *event) override;

  /**
   * @brief 当时间轴的缩放比例改变时调用的事件处理函数。
   * @param scale 新的水平缩放比例。
   */
  void ScaleChangedEvent(const double &) override;
  /**
   * @brief 当时间轴的时间基准（如帧率）改变时调用的事件处理函数。
   * @param timebase 新的时间基准。
   */
  void TimebaseChangedEvent(const rational &) override;

  /**
   * @brief 当连接的节点（通常是时间目标所关联的节点）发生改变时的事件处理函数。
   * @param n 新的关联节点 ViewerOutput 指针。
   */
  void ConnectedNodeChangeEvent(ViewerOutput *n) override;

  /**
   * @brief 获取用于对齐操作的关键帧轨道列表。
   * @return 如果 KeyframeView 存在，则返回其轨道列表；否则返回 nullptr。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<KeyframeViewInputConnection *> *GetSnapKeyframes() const override {
    return keyframe_view_ ? &keyframe_view_->GetKeyframeTracks() : nullptr;
  }

  /**
   * @brief 获取在对齐操作中应忽略的关键帧列表（通常是当前选中的关键帧）。
   * @return 如果 KeyframeView 存在，则返回其选中的关键帧列表；否则返回 nullptr。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const std::vector<NodeKeyframe *> *GetSnapIgnoreKeyframes() const override {
    return keyframe_view_ ? &keyframe_view_->GetSelectedKeyframes() : nullptr;
  }

  /**
   * @brief 获取关键帧的时间目标对象。
   * @return 返回内部 KeyframeView 实例作为 TimeTargetObject。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const TimeTargetObject *GetKeyframeTimeTarget() const override { return keyframe_view_; }

 private:
  /**
   * @brief 将关键帧位置更新请求加入队列，通常用于延迟更新以避免频繁重绘。
   */
  void QueueKeyframePositionUpdate();

  /**
   * @brief 添加一个新的上下文（通常是一个节点）到参数视图中。
   * @param ctx 要添加的 Node 指针。
   */
  void AddContext(Node *ctx);

  /**
   * @brief 从参数视图中移除一个上下文。
   * @param ctx 要移除的 Node 指针。
   */
  void RemoveContext(Node *ctx);

  /**
   * @brief 将指定节点的参数添加到指定的上下文中。
   * @param n 要添加其参数的 Node 指针。
   * @param ctx 此节点所属的上下文 Node 指针。
   * @param context 指向 NodeParamViewContext 对象的指针，表示该上下文在UI中的容器。
   */
  void AddNode(Node *n, Node *ctx, NodeParamViewContext *context);

  /**
   * @brief 对指定上下文中的参数项进行排序。
   * @param context 要排序其参数项的 NodeParamViewContext 指针。
   */
  static void SortItemsInContext(NodeParamViewContext *context);

  /**
   * @brief 根据上下文节点获取其对应的 NodeParamViewContext UI 项。
   * @param ctx 上下文 Node 指针。
   * @return 如果找到，则返回 NodeParamViewContext 指针；否则返回 nullptr。
   */
  NodeParamViewContext *GetContextItemFromContext(Node *ctx);

  /**
   * @brief 检查当前是否处于“组模式”。
   *
   * 组模式可能意味着当前只显示一个 NodeGroup 的内部节点参数。
   * @return 如果是组模式，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsGroupMode() const {
    // 如果只有一个上下文，并且该上下文是一个 NodeGroup，则认为是组模式
    return contexts_.size() == 1 && dynamic_cast<NodeGroup *>(contexts_.first());
  }

  /**
   * @brief 切换指定参数项的选中状态。
   * @param item 要切换选中状态的 NodeParamViewItem 指针。
   */
  void ToggleSelect(NodeParamViewItem *item);

  /**
   * @brief 在粘贴操作中，根据序列化结果生成现有节点到新创建（或已存在）节点的映射。
   * @param r ProjectSerializer::Result 对象，包含粘贴的数据。
   * @return 返回一个 QHash，键是原始（被复制）节点，值是目标（新粘贴或已存在）节点。
   */
  QHash<Node *, Node *> GenerateExistingPasteMap(const ProjectSerializer::Result &r);

  KeyframeView *keyframe_view_; ///< 指向内部集成的 KeyframeView 控件的指针，用于编辑动画曲线。

  QVector<NodeParamViewContext *> context_items_; ///< 存储所有活动的 NodeParamViewContext 对象的列表，每个对象代表一个显示的节点上下文。

  QScrollBar *vertical_scrollbar_; ///< 用于参数区域垂直滚动的滚动条。

  int last_scroll_val_; ///< 上一次滚动条的值，用于检测滚动变化。

  QScrollArea *param_scroll_area_; ///< 用于容纳可滚动参数控件区域的 QScrollArea。

  QWidget *param_widget_container_; ///< 作为 param_scroll_area_ 内容的实际容器控件。

  NodeParamViewDockArea *param_widget_area_; ///< 用于组织和停靠各个参数项 (NodeParamViewItem) 的区域。

  QVector<Node *> pinned_nodes_; ///< 被用户“钉住”的节点列表，这些节点的参数会持续显示。
  QVector<Node *> active_nodes_; ///< 当前活动的（例如，被选中的）节点列表。

  NodeParamViewItem *focused_node_;         ///< 指向当前获得焦点的节点参数项。
  QVector<NodeParamViewItem *> selected_nodes_; ///< 当前选中的节点参数项列表。

  QVector<Node *> contexts_;         ///< 当前配置要显示其参数的上下文（节点）列表。
  QVector<Node *> current_contexts_; ///< 实际当前正在显示的上下文列表（可能与 contexts_ 不同，例如在加载过程中）。

  bool show_all_nodes_; ///< 标记是否应显示所有节点的参数（可能用于全局模式）。

 private slots:
  /**
   * @brief 更新全局滚动条（可能是指 param_scroll_area_ 的滚动条）的状态。
   */
  void UpdateGlobalScrollBar();

  /**
   * @brief 钉住或取消钉住一个节点的参数显示。
   * @param pin 如果为 true，则钉住；否则取消钉住。
   */
  void PinNode(bool pin);

  // void FocusChanged(QWidget *old, QWidget *now); // 焦点改变的槽函数 (被注释掉了)

  /**
   * @brief 当节点被添加到某个上下文中时调用的槽函数。
   * @param n 被添加的 Node 指针。
   */
  void NodeAddedToContext(Node *n);

  /**
   * @brief 当节点从某个上下文中移除时调用的槽函数。
   * @param n 被移除的 Node 指针。
   */
  void NodeRemovedFromContext(Node *n);

  /**
   * @brief 当某个输入参数的复选框（通常用于启用/禁用或切换布尔值）状态改变时调用的槽函数。
   * @param input 发生改变的 NodeInput 的引用。
   * @param e 复选框的新状态。
   */
  void InputCheckBoxChanged(const NodeInput &input, bool e);

  /**
   * @brief 当 NodeGroup 的输入直通被添加时调用的槽函数。
   * @param group 相关的 NodeGroup 指针。
   * @param input 被添加为直通的 NodeInput。
   */
  void GroupInputPassthroughAdded(olive::NodeGroup *group, const olive::NodeInput &input);

  /**
   * @brief 当 NodeGroup 的输入直通被移除时调用的槽函数。
   * @param group 相关的 NodeGroup 指针。
   * @param input 被移除直通的 NodeInput。
   */
  void GroupInputPassthroughRemoved(olive::NodeGroup *group, const olive::NodeInput &input);

  /**
   * @brief 更新当前显示的上下文列表。
   */
  void UpdateContexts();

  /**
   * @brief 当一个 NodeParamViewItem 即将被移除时调用的槽函数。
   * @param item 即将被移除的 NodeParamViewItem 指针。
   */
  void ItemAboutToBeRemoved(NodeParamViewItem *item);

  /**
   * @brief 当一个 NodeParamViewItem 被点击时调用的槽函数。
   *
   * 通常用于处理参数项的选择和焦点。
   */
  void ItemClicked();

  /**
   * @brief 当用户点击参数项上的连接指示器（例如，链接到另一个节点的输入）时，选择并可能聚焦到被连接的节点。
   * @param node 指向被连接的 Node 的指针。
   */
  void SelectNodeFromConnectedLink(Node *node);

  /**
   * @brief 请求在查看器中编辑文本（例如，当文本参数被激活编辑时）。
   */
  void RequestEditTextInViewer();

  /**
   * @brief 当数组类型的输入参数的大小发生改变时调用的槽函数。
   * @param input 发生改变的输入参数的名称。
   * @param old_size 数组的旧大小。
   * @param new_size 数组的新大小。
   */
  void InputArraySizeChanged(const QString &input, int old_size, int new_size);
};

}  // namespace olive

#endif  // NODEPARAMVIEW_H
