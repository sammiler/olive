#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include "dialog/progress/progress.h" // 引入基础进度对话框类
#include "task/task.h"                // 引入任务基类定义

namespace olive {

/**
 * @class TaskDialog
 * @brief 任务对话框类，继承自 ProgressDialog。
 *
 * 此对话框用于显示一个特定任务 (Task) 的进度，并管理该任务的生命周期。
 * 它通常在后台任务执行时向用户提供反馈。
 */
class TaskDialog : public ProgressDialog {
  Q_OBJECT
 public:
  /**
   * @brief TaskDialog 构造函数。
   *
   * 创建一个 TaskDialog。TaskDialog 会获得 Task 对象的所有权，并在关闭时销毁它。
   * 如果需要在任务销毁前从中检索信息，请连接到 Task::Succeeded() 信号。
   * @param task 指向要执行和显示的 Task 对象的指针。
   * @param title 对话框的窗口标题。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  TaskDialog(Task* task, const QString& title, QWidget* parent = nullptr);

  /**
   * @brief 设置 TaskDialog 在关闭时是否应销毁自身（以及关联的任务）。
   *
   * 默认值为 TRUE。
   * @param e 如果为 true，则在关闭时销毁；否则为 false。
   */
  void SetDestroyOnClose(bool e) { destroy_on_close_ = e; }

  /**
   * @brief 返回此对话框关联的任务对象。
   * @return [[nodiscard]] 指向 Task 对象的指针。
   * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
   */
  [[nodiscard]] Task* GetTask() const { return task_; }

 protected:
  /**
   * @brief 重写 QDialog 的 showEvent 事件处理函数。
   *
   * 在对话框首次显示时启动关联的任务（如果尚未启动）。
   * @param e QShowEvent 事件对象指针。
   */
  void showEvent(QShowEvent* e) override;

  /**
   * @brief 重写 QDialog 的 closeEvent 事件处理函数。
   *
   * 当用户尝试关闭对话框时，会尝试取消关联的任务。
   * @param e QCloseEvent 事件对象指针。
   */
  void closeEvent(QCloseEvent* e) override;

 signals:
  /**
   * @brief 信号：当关联的任务成功完成时发出。
   * @param task 指向已成功完成的 Task 对象的指针。
   */
  void TaskSucceeded(Task* task);

  /**
   * @brief 信号：当关联的任务执行失败时发出。
   * @param task 指向执行失败的 Task 对象的指针。
   */
  void TaskFailed(Task* task);

 private:
  /**
   * @brief 指向此对话框管理的 Task 对象的指针。
   */
  Task* task_;

  /**
   * @brief 标记对话框关闭时是否销毁自身和任务的布尔值。
   */
  bool destroy_on_close_;

  /**
   * @brief 标记对话框是否已经显示过的布尔值。
   * 用于确保任务只在第一次显示时启动。
   */
  bool already_shown_;

 private slots:
  /**
   * @brief 私有槽函数：当关联的任务完成（无论成功或失败）时调用。
   *
   * 此槽函数会根据任务的最终状态发出 TaskSucceeded 或 TaskFailed 信号，
   * 并根据 destroy_on_close_ 的设置决定是否关闭并销毁对话框。
   */
  void TaskFinished();
};

}  // namespace olive

#endif  // TASKDIALOG_H
