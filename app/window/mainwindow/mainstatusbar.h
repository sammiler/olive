#ifndef MAINSTATUSBAR_H
#define MAINSTATUSBAR_H

#include <QMouseEvent>   // 引入 QMouseEvent，用于处理鼠标事件
#include <QProgressBar>  // 引入 QProgressBar 类，用于在状态栏中显示进度
#include <QStatusBar>    // 引入 QStatusBar 基类，MainStatusBar 继承自它

#include "task/taskmanager.h"  // 引入 TaskManager 类的定义，状态栏会显示其信息

namespace olive {

// 前向声明 TaskManager 和 Task，如果它们在 "task/taskmanager.h" 中已完整定义，则此处非必需
// class TaskManager;
// class Task;

/**
 * @brief 显示来自 TaskManager 对象的摘要信息的主状态栏。
 *
 * MainStatusBar 继承自 QStatusBar，用于在应用程序主窗口的底部显示
 * 后台任务的简要状态，例如当前活动任务的名称和进度。
 */
class MainStatusBar : public QStatusBar {
 Q_OBJECT  // 声明此类使用 Qt 的元对象系统（信号和槽）
     public :
     /**
      * @brief 显式构造函数。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit MainStatusBar(QWidget* parent = nullptr);

  /**
   * @brief 连接状态栏到一个 TaskManager 实例。
   *
   * 连接后，状态栏将开始监听 TaskManager 的信号以更新其显示内容。
   * @param manager 指向要连接的 TaskManager 对象的指针。
   */
  void ConnectTaskManager(TaskManager* manager);

 signals:
  /**
   * @brief 当状态栏被双击时发出的信号。
   *
   * 通常用于触发打开更详细的任务视图的操作。
   */
  void DoubleClicked();

 protected:
  /**
   * @brief 重写鼠标双击事件处理函数。
   *
   * 当用户双击状态栏时，此方法被调用，并发出 DoubleClicked() 信号。
   * @param e 指向 QMouseEvent 事件对象的指针。
   */
  void mouseDoubleClickEvent(QMouseEvent* e) override;

 private slots:
  /**
   * @brief 更新状态栏显示的槽函数。
   *
   * 当 TaskManager 的状态发生变化时（例如，任务列表改变、任务失败），
   * 此槽函数会被调用以刷新状态栏上的文本和进度条。
   */
  void UpdateStatus();

  /**
   * @brief 设置进度条值的槽函数。
   *
   * 通常由当前活动任务的进度更新信号触发。
   * @param d 进度值，通常在 0.0 到 1.0 之间 (或 0 到 100，取决于进度条的配置)。
   */
  void SetProgressBarValue(double d);

  /**
   * @brief 当已连接的任务被删除时调用的槽函数。
   *
   * 如果状态栏当前正在显示某个特定任务的进度，而该任务被删除了，
   * 此槽函数负责清理相关状态并更新显示。
   */
  void ConnectedTaskDeleted();

 private:
  TaskManager* manager_;  ///< 指向已连接的 TaskManager 实例的指针。

  QProgressBar* bar_;  ///< 指向状态栏中用于显示任务进度的 QProgressBar 对象的指针。

  Task* connected_task_;  ///< 指向当前状态栏正在显示其进度的 Task 对象的指针，可能为 nullptr。
};

}  // namespace olive

#endif  // MAINSTATUSBAR_H
