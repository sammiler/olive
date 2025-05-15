#ifndef GENERATEJOB_H
#define GENERATEJOB_H

#include "acceleratedjob.h"
#include "codec/frame.h"

namespace olive {

class GenerateJob : public AcceleratedJob {
 public:
  GenerateJob() = default;
  explicit GenerateJob(const NodeValueRow &row) : GenerateJob() { Insert(row); }
};

}  // namespace olive

#endif  // GENERATEJOB_H
