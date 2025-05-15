

#ifndef CLIPROGRESSDIALOG_H
#define CLIPROGRESSDIALOG_H

#include <QObject>
#include <QTimer> // QTimer 可能用于节流更新，但在此头文件中未直接实例化

#include "common/define.h"

namespace olive {

/**
 * @brief 在命令行界面 (CLI) 中显示进度的对话框类。
 *
 * 此类提供了一种在没有图形用户界面的情况下，
 * 向用户展示某个操作（如导出、渲染）进度的文本方式。
 * 它通常会在控制台输出一个更新的进度条或百分比。
 */
class CLIProgressDialog : public QObject {
  Q_OBJECT // Qt元对象系统宏
 public:
  /**
   * @brief 构造一个新的 CLIProgressDialog 对象。
   * @param title 进度对话框的标题，将显示在进度信息之前。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  explicit CLIProgressDialog(QString title, QObject* parent = nullptr);

public slots:
 /**
  * @brief 设置当前的进度值。
  * @param p 进度值，通常范围在 0.0 (0%) 到 1.0 (100%) 之间。
  */
 void SetProgress(double p);

private:
  /**
   * @brief 更新命令行中的进度显示。
   * 此方法负责在控制台绘制或更新进度条/百分比。
   */
  void Update();

  /**
   * @brief 进度对话框的标题。
   */
  QString title_;

  /**
   * @brief 当前的进度值，范围通常为 0.0 到 1.0。
   */
  double progress_;

  /**
   * @brief 标记进度条是否已经被绘制过至少一次。
   * 用于控制首次绘制和后续更新的行为。
   */
  bool drawn_;
};

}  // namespace olive

#endif  // CLIPROGRESSDIALOG_H