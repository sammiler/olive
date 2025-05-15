

#include "loadbasetask.h"

namespace olive {

ProjectLoadBaseTask::ProjectLoadBaseTask(const QString &filename) : project_(nullptr), filename_(filename) {
  SetTitle(tr("Loading '%1'").arg(filename));
}

}  // namespace olive
