

#ifndef TIMELINECOMMON_H
#define TIMELINECOMMON_H

#include <olive/core/core.h>

#include "common/define.h"

using namespace olive::core;

namespace olive {

class Block;
class Track;

class Timeline {
 public:
  enum MovementMode { kNone, kMove, kTrimIn, kTrimOut };

  enum ThumbnailMode { kThumbnailOff, kThumbnailInOut, kThumbnailOn };

  enum WaveformMode { kWaveformsDisabled, kWaveformsEnabled };

  static bool IsATrimMode(MovementMode mode) { return mode == kTrimIn || mode == kTrimOut; }

  struct EditToInfo {
    Track* track{};
    rational nearest_time;
    Block* nearest_block{};
  };
};

#define PLAYHEAD_COLOR palette().highlight().color()

}  // namespace olive

#endif  // TIMELINECOMMON_H
