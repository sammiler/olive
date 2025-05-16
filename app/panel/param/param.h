#ifndef PARAM_H  // 防止头文件被重复包含的宏 (文件名似乎应为 PARAMPANEL_H 以匹配类名)
#define PARAM_H  // 定义 PARAM_H 宏

#include "panel/curve/curve.h"                   // 包含曲线面板 CurvePanel 的定义 (可能用于联动或接口)
#include "panel/timebased/timebased.h"           // 包含 TimeBasedPanel 基类的定义
#include "widget/nodeparamview/nodeparamview.h"  // 包含 NodeParamView 控件的定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief ParamPanel 类代表一个用于显示和编辑节点参数的面板。
 *
 * 它继承自 TimeBasedPanel，表明它可能与时间相关 (例如，参数值可能随时间变化，通过关键帧)。
 * 这个面板通常会包含一个 NodeParamView 控件，该控件负责列出所选节点的参数，
 * 并提供用户界面来修改这些参数的值，以及管理它们是否启用关键帧、是否连接等状态。
 */
class ParamPanel : public TimeBasedPanel {  // ParamPanel 继承自 TimeBasedPanel
 Q_OBJECT                                   // 声明此类使用 Qt 的元对象系统

     public :
     // 构造函数
     ParamPanel();

  /**
   * @brief 获取内部封装的 NodeParamView 控件的指针。
   * 通过动态类型转换从基类 (TimeBasedPanel) 提供的 GetTimeBasedWidget() 方法获取。
   * @return 返回 NodeParamView 指针，如果转换失败或内部控件不是 NodeParamView 类型，则返回 nullptr。
   */
  [[nodiscard]] NodeParamView *GetParamView() const {
    // 将基类提供的 TimeBasedWidget 动态转换为 NodeParamView
    return dynamic_cast<NodeParamView *>(GetTimeBasedWidget());
  }

  /**
   * @brief 获取当前参数视图中关注的上下文节点列表。
   * 上下文节点通常指其参数正在被显示的节点。
   * @return 返回一个包含上下文 Node 指针的 QVector 的常量引用。
   */
  [[nodiscard]] const QVector<Node *> &GetContexts() const { return GetParamView()->GetContexts(); }

  /**
   * @brief 关闭所有属于指定项目的参数视图上下文。
   * 当关闭项目时，需要确保与该项目相关的节点不再在参数面板中显示。
   * @param p 要关闭其上下文的项目指针。
   */
  void CloseContextsBelongingToProject(Project *p) const { GetParamView()->CloseContextsBelongingToProject(p); }

 public slots:  // Qt 公有槽函数
  /**
   * @brief 设置参数视图当前要显示的选定节点。
   * 参数视图将列出这些选定节点的参数。
   * @param nodes 包含 Node::ContextPair 的 QVector，指定了节点及其上下文。
   */
  void SetSelectedNodes(const QVector<Node::ContextPair> &nodes) const {
    // 调用 NodeParamView 的方法来设置选定节点，通常不立即应用更改 (false参数)
    GetParamView()->SetSelectedNodes(nodes, false);
  }

  /**
   * @brief 重写基类或接口方法，用于删除参数视图中当前选中的内容 (例如，选中的参数或关键帧)。
   */
  void DeleteSelected() override;

  /**
   * @brief 重写基类或接口方法，用于全选参数视图中的所有可选元素。
   */
  void SelectAll() override;

  /**
   * @brief 重写基类或接口方法，用于取消选择参数视图中的所有元素。
   */
  void DeselectAll() override;

  /**
   * @brief 设置参数视图的上下文节点。
   * 这会使参数视图显示指定节点 (通常是 NodeGroup) 的参数，或者在特定上下文中查看其他节点的参数。
   * @param contexts 包含要设置为上下文的节点指针的 QVector。
   */
  void SetContexts(const QVector<Node *> &contexts);

 signals:  // Qt 信号声明
  /**
   * @brief 当参数视图中当前聚焦的节点发生改变时发出的信号。
   * 聚焦的节点可能是用户正在编辑其参数的节点。
   * @param n 新聚焦的节点指针。
   */
  void FocusedNodeChanged(Node *n);

  /**
   * @brief 当参数视图中选定的节点列表发生改变时发出的信号。
   * @param nodes 新的选定节点列表 (包含上下文信息)。
   */
  void SelectedNodesChanged(const QVector<Node::ContextPair> &nodes);

  /**
   * @brief 请求查看器 (Viewer) 开始编辑文本的信号。
   * 这通常在参数面板中编辑文本参数时，希望在主查看器中进行更丰富的文本编辑时发出。
   */
  void RequestViewerToStartEditingText();

 protected:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   * 例如，可能会更新面板标题或内部控件的标签。
   */
  void Retranslate() override;

  // 注意：实际的 NodeParamView 成员变量通常在构造函数中创建，并通过 TimeBasedPanel
  // (或其父类 PanelWidget) 的机制 (如 setCentralWidget 或类似的) 设置为面板的中心内容。
};

}  // namespace olive

#endif  // PARAM_H (或应为 PARAMPANEL_H)