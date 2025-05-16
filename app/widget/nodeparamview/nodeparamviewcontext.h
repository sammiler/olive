#ifndef NODEPARAMVIEWCONTEXT_H
#define NODEPARAMVIEWCONTEXT_H

#include <QAction>  // Qt 动作类 (用于槽函数参数)
#include <QHash>    // Qt 哈希表容器 (虽然本文件未直接使用，但其包含的头文件可能使用)
#include <QVector>  // Qt 动态数组容器
#include <QWidget>  // Qt 控件基类 (作为 NodeParamViewContext 的基类 NodeParamViewItemBase 的父类)

#include "node/node.h"                // 节点基类 (用于 contexts_ 成员和函数参数)
#include "node/output/track/track.h"  // 轨道类型定义 (用于 Track::Type)
#include "node/param.h"               // 节点参数相关定义 (用于 SetInputChecked 等)
#include "nodeparamviewdockarea.h"    // 节点参数视图停靠区域类
#include "nodeparamviewitem.h"        // 节点参数视图中的单个参数项
#include "nodeparamviewitembase.h"    // 节点参数视图项的基类 (NodeParamViewContext 的父类)

// 前向声明项目内部类 (根据用户要求，不添加)
// class rational;
// class ViewerOutput;
// class NodeInput;

namespace olive {

/**
 * @brief NodeParamViewContext 类代表节点参数视图中的一个“上下文”。
 *
 * 一个上下文通常对应一个或多个节点，并负责在 UI 中组织和显示这些节点的参数项 (NodeParamViewItem)。
 * 它继承自 NodeParamViewItemBase，表明它本身也可以被视为参数视图中的一个可管理项（例如，一个可折叠的组）。
 * 内部包含一个 NodeParamViewDockArea 来实际布局各个参数控件。
 */
class NodeParamViewContext : public NodeParamViewItemBase {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit NodeParamViewContext(QWidget *parent = nullptr);

  /**
   * @brief 获取此上下文内部用于停靠和排列参数项的区域。
   * @return 指向 NodeParamViewDockArea 对象的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] NodeParamViewDockArea *GetDockArea() const { return dock_area_; }

  /**
   * @brief 获取此上下文关联的所有节点（上下文本身）。
   * @return 返回一个 const 引用，指向包含 Node 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<Node *> &GetContexts() const { return contexts_; }

  /**
   * @brief 获取此上下文中显示的所有参数项。
   * @return 返回一个 const 引用，指向包含 NodeParamViewItem 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<NodeParamViewItem *> &GetItems() const { return items_; }

  /**
   * @brief 根据节点及其直接上下文获取对应的参数项。
   * @param node 要查找其参数项的节点。
   * @param ctx 该节点所属的直接上下文（如果节点是嵌套的，例如在组内）。
   * @return 如果找到，则返回 NodeParamViewItem 指针；否则返回 nullptr。
   */
  NodeParamViewItem *GetItem(Node *node, Node *ctx);

  /**
   * @brief 向此上下文中添加一个节点参数项。
   * @param item 要添加的 NodeParamViewItem 指针。
   */
  void AddNode(NodeParamViewItem *item);

  /**
   * @brief 从此上下文中移除指定节点（及其参数项）。
   * @param node 要移除的节点。
   * @param ctx 该节点所属的直接上下文。
   */
  void RemoveNode(Node *node, Node *ctx);

  /**
   * @brief 移除所有与指定上下文节点相关的参数项。
   * @param ctx 要移除其所有参数项的上下文 Node 指针。
   */
  void RemoveNodesWithContext(Node *ctx);

  /**
   * @brief 设置指定输入参数的复选框（如果存在）的勾选状态。
   * @param input 目标 NodeInput 的引用。
   * @param e 布尔值，true 表示勾选，false 表示取消勾选。
   */
  void SetInputChecked(const NodeInput &input, bool e);

  /**
   * @brief 设置此上下文关联的时间基准（例如帧率）。
   * @param timebase 新的时间基准 (rational 类型)。
   */
  void SetTimebase(const rational &timebase);

  /**
   * @brief 设置此上下文关联的时间目标（通常是查看器输出）。
   * @param n 指向 ViewerOutput 对象的指针。
   */
  void SetTimeTarget(ViewerOutput *n);

  /**
   * @brief 设置效果类型（可能影响此上下文中可添加的效果列表）。
   * @param type Track::Type 枚举值。
   */
  void SetEffectType(Track::Type type);

 signals:
  /**
   * @brief 当一个参数项即将从此上下文中删除时发出此信号。
   * @param item 指向即将被删除的 NodeParamViewItem 的指针。
   */
  void AboutToDeleteItem(NodeParamViewItem *item);

 public slots:
  /**
   * @brief 向此上下文添加一个关联的节点。
   * @param node 要添加的 Node 指针。
   */
  void AddContext(Node *node) { contexts_.append(node); }

  /**
   * @brief从此上下文移除一个关联的节点。
   * @param node 要移除的 Node 指针。
   */
  void RemoveContext(Node *node) { contexts_.removeOne(node); }

 protected slots:
  /**
   * @brief 重写基类的 Retranslate 方法，用于更新此上下文相关的 UI 文本。
   */
  void Retranslate() override;

 private:
  NodeParamViewDockArea *dock_area_;  ///< 用于停靠和显示此上下文中所有 NodeParamViewItem 的区域。

  QVector<Node *> contexts_;  ///< 存储此 UI 上下文所代表的实际节点（或节点组）的列表。
                              ///< 例如，如果多个节点被同时编辑，它们可能共享一个上下文显示。

  QVector<NodeParamViewItem *> items_;  ///< 存储此上下文中所有直接显示的 NodeParamViewItem 的列表。

  Track::Type type_;  ///< 可能用于指示此上下文关联的轨道类型（视频、音频），影响可添加的效果等。

 private slots:
  /**
   * @brief “添加效果”按钮被点击时的槽函数。
   *
   * 通常会弹出一个菜单，列出可添加到当前上下文（节点）的效果。
   */
  void AddEffectButtonClicked();

  /**
   * @brief “添加效果”菜单中的某个菜单项被触发时的槽函数。
   * @param a 被触发的 QAction 指针，其数据通常包含要添加的效果的标识。
   */
  void AddEffectMenuItemTriggered(QAction *a);
};

}  // namespace olive

#endif  // NODEPARAMVIEWCONTEXT_H
