#ifndef PROJECTEXPLORERICONVIEW_H
#define PROJECTEXPLORERICONVIEW_H

#include "projectexplorericonviewitemdelegate.h"  // 项目浏览器图标视图的项委托类
#include "projectexplorerlistviewbase.h"          // 项目浏览器列表视图的基类 (可能包含通用选择和模型处理逻辑)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget; // ProjectExplorerListViewBase 的基类 QAbstractItemView 的基类 QAbstractScrollArea 的基类 QFrame
// 的基类 QWidget

namespace olive {

/**
 * @brief ProjectExplorerIconView 类是当 ProjectExplorer 处于图标视图模式时使用的视图控件。
 *
 * 它继承自 ProjectExplorerListViewBase (可能共享一些基础的列表/视图行为)，
 * 并使用 ProjectExplorerIconViewItemDelegate 来定制项目项如何以图标形式显示。
 * 这个视图允许用户以可视化的图标网格形式浏览项目内容。
 */
class ProjectExplorerIconView : public ProjectExplorerListViewBase {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针。
      */
     explicit ProjectExplorerIconView(QWidget* parent);

 private:
  ProjectExplorerIconViewItemDelegate delegate_;  ///< 用于自定义此视图中项目项（图标）绘制方式的委托。
  ///< 委托负责渲染每个图标，包括其文本标签和可能的缩略图。
};

}  // namespace olive

#endif  // PROJECTEXPLORERICONVIEW_H
