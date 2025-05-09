#ifndef JOBTIME_H
#define JOBTIME_H

#include <stdint.h>
#include <QDebug>

namespace olive {

class JobTime {
 public:
  JobTime();

  void Acquire();

  uint64_t value() const { return value_; }

  bool operator==(const JobTime &rhs) const { return value_ == rhs.value_; }

  bool operator!=(const JobTime &rhs) const { return value_ != rhs.value_; }

  bool operator<(const JobTime &rhs) const { return value_ < rhs.value_; }

  bool operator>(const JobTime &rhs) const { return value_ > rhs.value_; }

  bool operator<=(const JobTime &rhs) const { return value_ <= rhs.value_; }

  bool operator>=(const JobTime &rhs) const { return value_ >= rhs.value_; }

 private:
  uint64_t value_;
};

}  // namespace olive

QDebug operator<<(QDebug debug, const olive::JobTime &r);

Q_DECLARE_METATYPE(olive::JobTime)

#endif  // JOBTIME_H
