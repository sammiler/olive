#ifndef ELAPSEDCOUNTERWIDGET_H
#define ELAPSEDCOUNTERWIDGET_H

#include <QLabel>
#include <QTimer>
#include <QWidget>

#include "common/define.h"

namespace olive {

/**
 * @brief ElapsedCounterWidget 类用于显示已用时间和剩余时间。
 *
 * 它继承自 QWidget，并提供更新计时器的功能。
 */
class ElapsedCounterWidget : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个 ElapsedCounterWidget 对象。
   * @param parent 父 QWidget 对象，默认为 nullptr。
   */
  explicit ElapsedCounterWidget(QWidget* parent = nullptr);

  /**
   * @brief 设置进度。
   *
   * 根据给定的进度值更新内部状态，但此方法目前未实现。
   * @param d 进度值 (double)。
   */
  void SetProgress(double d);

public slots:
 /**
  * @brief 开始计时器，并使用指定的开始时间。
  * @param start_time 计时器开始的时间戳 (qint64)。
  */
 void Start(qint64 start_time);
  /**
   * @brief 开始计时器，使用当前时间作为开始时间。
   */
  void Start();

public slots:
 /**
  * @brief 停止计时器。
  */
 void Stop();

private:
  /**
   * @brief 用于显示已用时间的 QLabel 指针。
   */
  QLabel* elapsed_lbl_;

  /**
   * @brief 用于显示剩余时间的 QLabel 指针。
   */
  QLabel* remaining_lbl_;

  /**
   * @brief 上一次记录的进度值。
   */
  double last_progress_;

  /**
   * @brief 用于定期更新已用时间的 QTimer 对象。
   */
  QTimer elapsed_timer_;

  /**
   * @brief 计时器开始的时间戳。
   */
  qint64 start_time_;

private slots:
 /**
  * @brief 更新计时器显示。
  *
  * 此槽函数连接到 elapsed_timer_ 的 timeout() 信号。
  */
 void UpdateTimers();
};

}  // namespace olive

#endif  // ELAPSEDCOUNTERWIDGET_H