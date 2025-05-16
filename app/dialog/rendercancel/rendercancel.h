#ifndef RENDERCANCELDIALOG_H
#define RENDERCANCELDIALOG_H

#include "dialog/progress/progress.h"  // 引入基础进度对话框类

namespace olive {

/**
 * @class RenderCancelDialog
 * @brief 渲染取消对话框类，继承自 ProgressDialog。
 *
 * 此对话框专门用于显示渲染操作的进度，并允许用户取消渲染。
 * 它扩展了 ProgressDialog 的功能，以处理多个工作线程的进度跟踪。
 */
class RenderCancelDialog : public ProgressDialog {
  Q_OBJECT
 public:
  /**
   * @brief RenderCancelDialog 构造函数。
   *
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit RenderCancelDialog(QWidget* parent = nullptr);

  /**
   * @brief 如果有工作线程正在忙碌，则运行（显示）此对话框。
   */
  void RunIfWorkersAreBusy();

  /**
   * @brief 设置总的工作线程数量。
   *
   * @param count 工作线程的总数。
   */
  void SetWorkerCount(int count);

  /**
   * @brief 当一个工作线程开始时调用此方法。
   *
   * 用于更新内部计数器和进度显示。
   */
  void WorkerStarted();

 public slots:
  /**
   * @brief 公有槽函数：当一个工作线程完成时调用。
   *
   * 用于更新内部计数器和进度显示。如果所有工作线程都已完成，则关闭对话框。
   */
  void WorkerDone();

 protected:
  /**
   * @brief 重写 QDialog 的 showEvent 事件处理函数。
   *
   * @param event QShowEvent 事件对象指针。
   */
  void showEvent(QShowEvent* event) override;

 private:
  /**
   * @brief 私有方法：根据当前工作线程的状态更新进度条。
   */
  void UpdateProgress();

  /**
   * @brief 当前正在忙碌的工作线程数量。
   */
  int busy_workers_;

  /**
   * @brief 工作线程的总数量。
   */
  int total_workers_;

  /**
   * @brief 等待开始的工作线程数量。
   */
  int waiting_workers_{};  // C++11风格的成员初始化
};

}  // namespace olive

#endif  // RENDERCANCELDIALOG_H
