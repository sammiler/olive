#ifndef CLITASKDIALOG_H
#define CLITASKDIALOG_H

#include "cli/cliprogress/cliprogressdialog.h" // 基类
#include "task/task.h"                         // 关联的任务类

namespace olive {

/**
 * @brief 专门用于在命令行界面显示特定任务 (Task) 进度的对话框类。
 *
 * 此类继承自 CLIProgressDialog，并与其关联一个具体的 Task 对象。
 * 它负责启动任务，并根据任务的进度更新命令行的显示。
 */
class CLITaskDialog : public CLIProgressDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 CLITaskDialog 对象。
   * @param task 指向要监视和运行的 Task 对象的指针。对话框的标题通常会从任务中获取。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  explicit CLITaskDialog(Task* task, QObject* parent = nullptr);

  /**
   * @brief 运行与此对话框关联的任务。
   * 此方法通常会启动任务，并处理其执行直到完成或被取消。
   * 同时，它会通过基类 CLIProgressDialog 的功能更新命令行进度。
   * @return bool 如果任务成功运行完成则返回 true，如果任务失败、被取消或未运行则返回 false。
   */
  bool Run();

private:
  /**
   * @brief 指向与此命令行进度对话框关联的 Task 对象的指针。
   */
  Task* task_;
};

}  // namespace olive

#endif  // CLITASKDIALOG_H