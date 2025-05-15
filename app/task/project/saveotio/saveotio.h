#ifndef PROJECTSAVEASOTIOTASK_H
#define PROJECTSAVEASOTIOTASK_H

#include <opentimelineio/timeline.h>
#include <opentimelineio/track.h>

#include "common/otioutils.h"
#include "node/project.h"
#include "task/task.h"

namespace olive {

class SaveOTIOTask : public Task {
  Q_OBJECT
 public:
  explicit SaveOTIOTask(Project* project);

 protected:
  bool Run() override;

 private:
  OTIO::Timeline* SerializeTimeline(Sequence* sequence);

  static OTIO::Track* SerializeTrack(Track* track, double sequence_rate, rational max_track_length);

  static bool SerializeTrackList(TrackList* list, OTIO::Timeline* otio_timeline, double sequence_rate);

  Project* project_;
};

}  // namespace olive

#endif
