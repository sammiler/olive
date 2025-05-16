#ifndef PROJECTEXPLORERLISTVIEW_H
#define PROJECTEXPLORERLISTVIEW_H

#include "projectexplorerlistviewbase.h"         // 项目浏览器列表视图的基类
#include "projectexplorerlistviewitemdelegate.h" // 项目浏览器列表视图的项委托类

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget; // ProjectExplorerListViewBase 的基类 QAbstractItemView 的基类 QAbstractScrollArea 的基类 QFrame 的基类 QWidget

namespace olive {

/**
 * @brief ProjectExplorerListView 类是当 ProjectExplorer 处于列表视图模式时使用的视图控件。
 *
 * 它继承自 ProjectExplorerListViewBase (可能共享一些基础的列表/视图行为，如模型交互和选择逻辑)，
 * 并使用 ProjectExplorerListViewItemDelegate 来定制项目项在列表中的显示方式（例如，每行显示哪些信息，如何绘制）。
 */
class ProjectExplorerListView : public ProjectExplorerListViewBase {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针。
   */
  explicit ProjectExplorerListView(QWidget* parent);

private:
  ProjectExplorerListViewItemDelegate delegate_; ///< 用于自定义此视图中项目项（列表中的每一行）绘制方式的委托。
  ///< 委托负责渲染每个列表项，包括其文本、图标以及可能的列数据。
};

}  // namespace olive

#endif  // PROJECTEXPLORERLISTVIEW_H
