

#ifndef CUSTOMCACHETASK_H
#define CUSTOMCACHETASK_H

#include <QMutex>
#include <QWaitCondition>

#include "task/task.h"

namespace olive {

class CustomCacheTask : public Task {
  Q_OBJECT
 public:
  explicit CustomCacheTask(const QString &sequence_name);

  void Finish();

 signals:
  void Cancelled();

 protected:
  bool Run() override;

  void CancelEvent() override;

 private:
  QMutex mutex_;

  QWaitCondition wait_cond_;

  bool cancelled_through_finish_;
};

}  // namespace olive

#endif  // CUSTOMCACHETASK_H
