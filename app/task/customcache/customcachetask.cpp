#include "customcachetask.h"

namespace olive {

CustomCacheTask::CustomCacheTask(const QString &sequence_name) : cancelled_through_finish_(false) {
  SetTitle(tr("Caching custom range for \"%1\"").arg(sequence_name));
}

void CustomCacheTask::Finish() {
  mutex_.lock();

  cancelled_through_finish_ = true;
  Cancel();

  mutex_.unlock();
}

bool CustomCacheTask::Run() {
  mutex_.lock();

  while (!IsCancelled()) {
    wait_cond_.wait(&mutex_);
  }

  mutex_.unlock();

  return true;
}

void CustomCacheTask::CancelEvent() {
  if (!cancelled_through_finish_) {
    emit Cancelled();
  }
  wait_cond_.wakeOne();
}

}  // namespace olive
