#ifndef CACHEJOB_H
#define CACHEJOB_H

#include <QString>
#include <QVariant>

#include "node/value.h"
#include "render/job/acceleratedjob.h"

namespace olive {

class CacheJob : public AcceleratedJob {
 public:
  CacheJob() = default;
  explicit CacheJob(const QString &filename, const NodeValue &fallback = NodeValue()) { filename_ = filename; }

  [[nodiscard]] const QString &GetFilename() const { return filename_; }
  void SetFilename(const QString &s) { filename_ = s; }

  [[nodiscard]] const NodeValue &GetFallback() const { return fallback_; }
  void SetFallback(const NodeValue &val) { fallback_ = val; }

 private:
  QString filename_;

  NodeValue fallback_;
};

}  // namespace olive

#endif  // CACHEJOB_H
