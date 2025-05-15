

#ifndef OTIODECODER_H
#define OTIODECODER_H

#include "common/otioutils.h"
#include "node/project.h"
#include "task/project/load/loadbasetask.h"

namespace olive {

class LoadOTIOTask : public ProjectLoadBaseTask {
  Q_OBJECT
 public:
  explicit LoadOTIOTask(const QString& s);

 protected:
  bool Run() override;
};

}  // namespace olive

#endif  // OTIODECODER_H
