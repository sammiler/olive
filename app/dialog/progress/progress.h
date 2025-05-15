#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>      // 引入 QDialog 类，对话框窗口的基类
#include <QProgressBar> // 引入 QProgressBar 类，用于显示进度条

#include "common/debug.h"                        // 引入调试相关的宏和函数
#include "widget/taskview/elapsedcounterwidget.h" // 引入显示已用时间的控件

namespace olive {

/**
 * @class ProgressDialog
 * @brief 一个通用的进度对话框类。
 *
 * 用于向用户显示耗时操作的进度，并提供一个取消操作的选项。
 * 它包含一个进度条和一个已用时间计时器。
 */
class ProgressDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief ProgressDialog 构造函数。
   *
   * @param message 对话框中显示给用户的消息文本。
   * @param title 对话框的窗口标题。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  ProgressDialog(const QString& message, const QString& title, QWidget* parent = nullptr);

 protected:
  /**
   * @brief 重写 QDialog 的 showEvent 事件处理函数。
   *
   * 在对话框首次显示时进行一些初始化设置。
   * @param e QShowEvent 事件对象指针。
   */
  void showEvent(QShowEvent* e) override;

  /**
   * @brief 重写 QDialog 的 closeEvent 事件处理函数。
   *
   * 当用户尝试关闭对话框时触发 Cancelled 信号。
   * @param event QCloseEvent 事件对象指针。
   */
  void closeEvent(QCloseEvent* event) override;

 public slots:
  /**
   * @brief 公有槽函数：设置进度条的当前值。
   *
   * @param value 进度值，通常在 0.0 到 1.0 之间 (或对应 QProgressBar 的 min/max)。
   */
  void SetProgress(double value);

 signals:
  /**
   * @brief 信号：当用户点击取消按钮或关闭对话框时发出。
   *
   * 用于通知相关操作应被取消。
   */
  void Cancelled();

 protected:
  /**
   * @brief 受保护方法：显示错误消息对话框。
   *
   * @param title 错误消息对话框的标题。
   * @param message 错误消息对话框的内容。
   */
  void ShowErrorMessage(const QString& title, const QString& message);

 private:
  /**
   * @brief 指向进度条控件的指针。
   */
  QProgressBar* bar_;

  /**
   * @brief 指向显示已用时间的标签控件的指针。
   */
  ElapsedCounterWidget* elapsed_timer_lbl_;

  /**
   * @brief 标记是否应显示进度。
   */
  bool show_progress_;

  /**
   * @brief 标记是否是第一次显示对话框。
   */
  bool first_show_;

 private slots:
  /**
   * @brief 私有槽函数：禁用发送者控件。
   *
   * 可能用于在操作开始后禁用触发操作的按钮等。
   */
  void DisableSenderWidget();

  /**
   * @brief 私有槽函数：禁用进度相关的控件。
   *
   * 可能在操作完成或取消后调用。
   */
  void DisableProgressWidgets();
};

}  // namespace olive

#endif  // PROGRESSDIALOG_H
