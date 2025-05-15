

#ifndef RENDERJOBTRACKER_H
#define RENDERJOBTRACKER_H

#include <olive/core/core.h>

#include "common/jobtime.h"

namespace olive {

using namespace core;

class RenderJobTracker {
 public:
  RenderJobTracker() = default;

  void insert(const TimeRange &range, JobTime job_time);
  void insert(const TimeRangeList &ranges, JobTime job_time);

  void clear();

  [[nodiscard]] bool isCurrent(const rational &time, JobTime job_time) const;

  [[nodiscard]] TimeRangeList getCurrentSubRanges(const TimeRange &range, const JobTime &job_time) const;

 private:
  class TimeRangeWithJob : public TimeRange {
   public:
    TimeRangeWithJob() = default;
    TimeRangeWithJob(const TimeRange &range, const JobTime &job_time) {
      set_range(range.in(), range.out());
      job_time_ = job_time;
    }

    [[nodiscard]] JobTime GetJobTime() const { return job_time_; }
    void SetJobTime(JobTime jt) { job_time_ = jt; }

   private:
    JobTime job_time_;
  };

  std::vector<TimeRangeWithJob> jobs_;
};

}  // namespace olive

#endif  // RENDERJOBTRACKER_H
