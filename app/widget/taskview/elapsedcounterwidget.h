

#ifndef ELAPSEDCOUNTERWIDGET_H
#define ELAPSEDCOUNTERWIDGET_H

#include <QLabel>
#include <QTimer>
#include <QWidget>

#include "common/define.h"

namespace olive {

class ElapsedCounterWidget : public QWidget {
  Q_OBJECT
 public:
  explicit ElapsedCounterWidget(QWidget* parent = nullptr);

  void SetProgress(double d);

 public slots:
  void Start(qint64 start_time);
  void Start();

 public slots:
  void Stop();

 private:
  QLabel* elapsed_lbl_;

  QLabel* remaining_lbl_;

  double last_progress_;

  QTimer elapsed_timer_;

  qint64 start_time_;

 private slots:
  void UpdateTimers();
};

}  // namespace olive

#endif  // ELAPSEDCOUNTERWIDGET_H
