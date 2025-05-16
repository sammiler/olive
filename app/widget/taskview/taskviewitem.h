#ifndef TASKVIEWITEM_H
#define TASKVIEWITEM_H

#include <QFrame>          // 引入 QFrame 类，是带有边框的控件的基类
#include <QLabel>          // 引入 QLabel 类，用于显示文本或图像
#include <QProgressBar>    // 引入 QProgressBar 类，用于显示进度条
#include <QPushButton>     // 引入 QPushButton 类，用于创建按钮
#include <QStackedWidget>  // 引入 QStackedWidget 类，用于在同一位置显示多个控件，一次只显示一个
#include <QWidget>         // 引入 QWidget 类，是所有用户界面对象的基类

#include "elapsedcounterwidget.h"  // 引入 ElapsedCounterWidget 类，用于显示已用时间
#include "task/task.h"             // 引入 Task 类，代表一个可执行的任务

namespace olive {

/**
 * @brief TaskViewItem 类是一个以可视化方式表示任务 (Task) 状态的控件。
 *
 * TaskViewItem 控件显示任务的描述 (Task::text())、一个进度条 (通过 Task::ProgressChanged 更新)、
 * 任务的状态 (通过 Task::status() 或 Task::error() 生成的文本)，并提供一个取消按钮 (触发 Task::Cancel())
 * 用于在任务完成前取消任务。
 *
 * 主要的入口点是在创建 Task 和 TaskViewItem 对象后调用 SetTask() (虽然此类构造函数直接接收Task*，
 * 但通常良好实践中可能会有一个SetTask方法来更新或设置任务，此处注释遵循原始英文注释的意图)。
 */
class TaskViewItem : public QFrame {  // TaskViewItem 类继承自 QFrame，使其可以有边框
 Q_OBJECT                             // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

     public :
     /**
      * @brief 构造一个 TaskViewItem 对象。
      * @param task 指向关联的 Task 对象的指针。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit TaskViewItem(Task* task, QWidget* parent = nullptr);

  /**
   * @brief 将任务项标记为失败状态。
   *
   * 通常在关联的任务执行失败时调用此方法，
   * 以更新界面显示错误信息。
   */
  void Failed();

 signals:
  /**
   * @brief 当任务被取消时发出此信号。
   * @param t 指向被取消的 Task 对象的指针。
   */
  void TaskCancelled(Task* t);

 private:
  /**
   * @brief 用于显示任务名称的 QLabel 指针。
   */
  QLabel* task_name_lbl_;

  /**
   * @brief 用于显示任务进度的 QProgressBar 指针。
   */
  QProgressBar* progress_bar_;

  /**
   * @brief 用于取消任务的 QPushButton 指针。
   */
  QPushButton* cancel_btn_;

  /**
   * @brief QStackedWidget 指针，用于在不同状态（如计时器、错误信息）之间切换显示。
   */
  QStackedWidget* status_stack_;

  /**
   * @brief ElapsedCounterWidget 指针，用于显示任务已用时间。
   *
   * 它是 status_stack_ 中的一个页面。
   */
  ElapsedCounterWidget* elapsed_timer_lbl_;

  /**
   * @brief 用于显示任务错误信息的 QLabel 指针。
   *
   * 它是 status_stack_ 中的另一个页面。
   */
  QLabel* task_error_lbl_;

  /**
   * @brief 指向此任务项所代表的 Task 对象的指针。
   */
  Task* task_;

 private slots:
  /**
   * @brief 更新进度条的槽函数。
   *
   * 通常连接到 Task::ProgressChanged() 信号。
   * @param d 进度值，通常在 0.0 到 1.0 之间。
   */
  void UpdateProgress(double d);
};

}  // namespace olive

#endif  // TASKVIEWITEM_H
