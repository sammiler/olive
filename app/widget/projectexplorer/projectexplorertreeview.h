#ifndef PROJECTEXPLORERTREEVIEW_H
#define PROJECTEXPLORERTREEVIEW_H

#include <QTreeView> // 包含 QTreeView 类的头文件，用于树形视图显示

#include "common/define.h" // 包含项目通用定义

namespace olive {

/**
 * @brief 当 ProjectExplorer (项目浏览器) 处于树形视图模式时使用的视图小部件。
 *
 * 这是一个 QTreeView 的相当简单的子类，它提供了一个双击信号，
 * 无论索引是否有效都会发出该信号（QAbstractItemView 有一个 doubleClicked() 信号，
 * 但它仅在索引有效时发出）。
 */
class ProjectExplorerTreeView : public QTreeView {
  Q_OBJECT // Q_OBJECT宏，用于支持 Qt 的元对象系统（信号、槽等）

 public:
  /**
   * @brief ProjectExplorerTreeView 的显式构造函数。
   * @param parent 父 QWidget 指针。
   */
  explicit ProjectExplorerTreeView(QWidget* parent);

protected:
  /**
   * @brief 重写鼠标双击事件处理函数。
   *
   * 此函数会发出 DoubleClickedEmptyArea() 信号。
   *
   * @FIXME: 此处的代码与 ProjectExplorerListViewBase 中的代码相同。
   * 是否有办法通过某种方式合并这两个类的这部分代码？
   * @param event 鼠标事件指针。
   */
  void mouseDoubleClickEvent(QMouseEvent* event) override;

  signals:
   /**
    * @brief 无条件双击信号。
    *
    * 当视图被双击但未在任何特定项目上时发出此信号。
    * 这与标准的 QAbstractItemView::doubleClicked(const QModelIndex &index) 信号不同，
    * 后者仅在双击有效项目时发出。
    */
   void DoubleClickedEmptyArea();
};

}  // namespace olive

#endif  // PROJECTEXPLORERTREEVIEW_H
