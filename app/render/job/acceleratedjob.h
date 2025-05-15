#ifndef ACCELERATEDJOB_H
#define ACCELERATEDJOB_H

#include "node/param.h"
#include "node/valuedatabase.h"

namespace olive {

class AcceleratedJob {
 public:
  AcceleratedJob() = default;

  virtual ~AcceleratedJob() = default;

  [[nodiscard]] NodeValue Get(const QString &input) const { return value_map_.value(input); }

  void Insert(const QString &input, const NodeValueRow &row) { value_map_.insert(input, row.value(input)); }

  void Insert(const QString &input, const NodeValue &value) { value_map_.insert(input, value); }

  void Insert(const NodeValueRow &row) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    value_map_.insert(row);
#else
    for (auto it = row.cbegin(); it != row.cend(); it++) {
      value_map_.insert(it.key(), it.value());
    }
#endif
  }

  [[nodiscard]] const NodeValueRow &GetValues() const { return value_map_; }
  NodeValueRow &GetValues() { return value_map_; }

 private:
  NodeValueRow value_map_;
};

}  // namespace olive

#endif  // ACCELERATEDJOB_H
