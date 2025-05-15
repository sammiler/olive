

#ifndef SAMPLEJOB_H
#define SAMPLEJOB_H

#include "acceleratedjob.h"

namespace olive {

class SampleJob : public AcceleratedJob {
 public:
  SampleJob() = default;

  SampleJob(const TimeRange &time, const NodeValue &value) {
    samples_ = value.toSamples();
    time_ = time;
  }

  SampleJob(const TimeRange &time, const QString &from, const NodeValueRow &row) {
    samples_ = row[from].toSamples();
    time_ = time;
  }

  [[nodiscard]] const SampleBuffer &samples() const { return samples_; }

  [[nodiscard]] bool HasSamples() const { return samples_.is_allocated(); }

  [[nodiscard]] const TimeRange &time() const { return time_; }

 private:
  SampleBuffer samples_;

  TimeRange time_;
};

}  // namespace olive

Q_DECLARE_METATYPE(olive::SampleJob)

#endif  // SAMPLEJOB_H
