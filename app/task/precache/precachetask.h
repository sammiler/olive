

#ifndef PRECACHETASK_H
#define PRECACHETASK_H

#include "node/project/footage/footage.h"
#include "node/project/sequence/sequence.h"
#include "task/render/render.h"

namespace olive {

class PreCacheTask : public RenderTask {
  Q_OBJECT
 public:
  PreCacheTask(Footage* footage, int index, Sequence* sequence);

  ~PreCacheTask() override;

 protected:
  bool Run() override;

  bool FrameDownloaded(FramePtr frame, const rational& times) override;

  bool AudioDownloaded(const TimeRange& range, const SampleBuffer& samples) override;

 private:
  Project* project_;

  Footage* footage_;
};

}  // namespace olive

#endif  // PRECACHETASK_H
