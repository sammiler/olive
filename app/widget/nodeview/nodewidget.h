#ifndef NODEWIDGET_H
#define NODEWIDGET_H

#include <QVector>  // Qt 动态数组容器 (用于 SetContexts 参数)
#include <QWidget>  // Qt 控件基类

#include "nodeview.h"         // 节点视图控件，用于显示和编辑节点图
#include "nodeviewtoolbar.h"  // 节点视图的工具栏控件

// 前向声明项目内部类 (根据用户要求，不添加)
// class Node; // 已在 nodeview.h 中包含

namespace olive {

/**
 * @brief NodeWidget 类是一个组合控件，用于将节点视图 (NodeView) 和其工具栏 (NodeViewToolBar) 集成在一起。
 *
 * 它提供了一个统一的界面来展示和操作节点图。
 * 当设置上下文（要显示的节点）时，它会同时更新节点视图和工具栏的状态。
 */
class NodeWidget : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit NodeWidget(QWidget *parent = nullptr);

  /**
   * @brief 获取内部的 NodeView 实例。
   * @return 返回指向 NodeView 控件的 const 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] NodeView *view() const { return node_view_; }

  /**
   * @brief 设置要在节点视图中显示的上下文（节点列表）。
   *
   * 同时会根据节点列表是否为空来启用或禁用工具栏。
   * @param nodes 包含要显示的 Node 指针的 QVector。
   */
  void SetContexts(const QVector<Node *> &nodes) {
    node_view_->SetContexts(nodes);          // 将上下文设置到节点视图
    toolbar_->setEnabled(!nodes.isEmpty());  // 如果节点列表不为空，则启用工具栏，否则禁用
  }

 private:
  NodeView *node_view_;       ///< 指向内部的 NodeView 实例，用于显示和编辑节点图。
  NodeViewToolBar *toolbar_;  ///< 指向内部的 NodeViewToolBar 实例，提供节点视图相关的操作按钮。
};

}  // namespace olive

#endif  // NODEWIDGET_H
