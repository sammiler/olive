#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QTreeView>  // Qt 树视图控件基类

#include "undo/undostack.h"  // 项目中的撤销栈类

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QModelIndex;

namespace olive {

/**
 * @brief HistoryWidget 类是一个用于显示和导航操作历史（撤销/重做栈）的控件。
 *
 * 它继承自 QTreeView，并与一个 UndoStack 实例关联，
 * 以列表形式展示用户执行过的操作。用户可以点击列表中的某一项
 * 来撤销或重做至该操作的状态。
 */
class HistoryWidget : public QTreeView {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit HistoryWidget(QWidget *parent = nullptr);

 private:
  UndoStack *stack_;  ///< 指向关联的撤销栈 (UndoStack) 对象的指针。

  size_t current_row_{};  ///< 记录当前在历史记录视图中表示撤销栈当前位置的行号。

 private slots:
  /**
   * @brief 当撤销栈的当前索引发生改变时调用的槽函数。
   *
   * 此函数会更新视图中的选中项，以反映撤销栈中新的当前操作位置。
   * @param i 撤销栈中新的当前操作的索引。
   */
  void indexChanged(int i);

  /**
   * @brief 当视图中的当前选中行发生改变时（通常由用户点击触发）调用的槽函数。
   *
   * 此函数会将撤销栈的当前索引设置为用户在视图中新选中的行对应的操作。
   * @param current 新的当前项的 QModelIndex。
   * @param previous 前一个当前项的 QModelIndex。
   */
  void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
};

}  // namespace olive

#endif  // HISTORYWIDGET_H
