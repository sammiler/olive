#ifndef PROJECTEXPLORERLISTVIEWBASE_H
#define PROJECTEXPLORERLISTVIEWBASE_H

#include <QListView>     // Qt 列表视图控件基类
#include <QMouseEvent>   // Qt 鼠标事件类 (用于 mouseDoubleClickEvent)

#include "common/define.h" // 项目通用定义

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget; // QListView 的基类 QAbstractItemView 的基类 QAbstractScrollArea 的基类 QFrame 的基类 QWidget
// class QEvent; // 虽然 mouseDoubleClickEvent 接收 QMouseEvent，但 QEvent 是其基类

namespace olive {

/**
 * @brief ProjectExplorerListViewBase 类是一个 QListView 的派生类，
 * 包含了列表视图 (List view) 和图标视图 (Icon view) （两者都基于 QListView）所共用的功能。
 */
class ProjectExplorerListViewBase : public QListView {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针。
   */
  explicit ProjectExplorerListViewBase(QWidget* parent);

protected:
  /**
   * @brief 重写鼠标双击事件处理函数。
   *
   * 此函数会发出 DoubleClickedEmptyArea() 信号（如果双击发生在空白区域）。
   *
   * FIXME（待修复）: 此代码与 ProjectExplorerTreeView 中的代码相同。是否有办法通过子类化来合并这两部分？
   * @param event 鼠标事件指针。
   */
  void mouseDoubleClickEvent(QMouseEvent* event) override;

  signals:
   /**
    * @brief 无条件双击信号。
    *
    * 当视图被双击但未在任何特定项目上时发出此信号。
    */
   void DoubleClickedEmptyArea();
};

}  // namespace olive

#endif  // PROJECTEXPLORERLISTVIEWBASE_H
