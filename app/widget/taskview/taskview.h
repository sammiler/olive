#ifndef TASKVIEW_H
#define TASKVIEW_H

#include <QScrollArea> // 引入 QScrollArea 类，用于提供带滚动条的视图区域
#include <QVBoxLayout> // 引入 QVBoxLayout 类，用于垂直布局

#include "widget/taskview/taskviewitem.h" // 引入 TaskViewItem 类，用于显示单个任务项

namespace olive {

/**
 * @brief TaskView 类是一个显示任务列表的控件。
 *
 * TaskView 是一个相对简单的控件，用于显示 TaskViewItem 控件，每个 TaskViewItem 代表一个 Task 对象。
 * 主要的入口点是 AddTask() 槽函数，该槽函数应连接到 TaskManager 的 TaskAdded() 信号。
 * 不需要进行更多的连接，因为当任务完成时，TaskViewItem 会自动删除自身（从而将其从 TaskView 中移除）。
 */
class TaskView : public QScrollArea { // TaskView 类继承自 QScrollArea
  Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽
 public:
  /**
   * @brief 构造一个 TaskView 对象。
   * @param parent 父 QWidget 对象。
   */
  explicit TaskView(QWidget* parent);

  signals:
   /**
    * @brief 当任务被取消时发出此信号。
    * @param t 指向被取消的 Task 对象的指针。
    */
   void TaskCancelled(Task* t);

public slots:
 /**
  * @brief 创建一个 TaskViewItem，将其连接到一个 Task，并将其添加到此控件中。
  *
  * 将此槽函数连接到 TaskManager::TaskAdded() 信号。
  * @param t 指向要添加的 Task 对象的指针。
  */
 void AddTask(Task* t);

  /**
   * @brief 当任务失败时调用的槽函数。
   *
   * 此槽函数用于处理任务失败的情况。
   * @param t 指向失败的 Task 对象的指针。
   */
  void TaskFailed(Task* t);

  /**
   * @brief 移除一个任务。
   *
   * 此槽函数用于从视图中移除与指定任务关联的 TaskViewItem。
   * @param t 指向要移除的 Task 对象的指针。
   */
  void RemoveTask(Task* t);

private:
  /**
   * @brief 作为滚动区域中心控件的 QWidget 指针。
   *
   * TaskViewItem 对象会被添加到这个中心控件的布局中。
   */
  QWidget* central_widget_;

  /**
   * @brief 用于排列 TaskViewItem 的垂直布局 QVBoxLayout 指针。
   */
  QVBoxLayout* layout_;

  /**
   * @brief 存储 Task 指针与对应的 TaskViewItem 指针的哈希表。
   *
   * 用于快速查找和管理任务项。
   */
  QHash<Task*, TaskViewItem*> items_;
};

}  // namespace olive

#endif  // TASKVIEW_H
