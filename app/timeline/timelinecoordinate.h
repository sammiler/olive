#ifndef TIMELINECOORDINATE_H
#define TIMELINECOORDINATE_H

#include "node/output/track/track.h"

namespace olive {

class TimelineCoordinate {
 public:
  TimelineCoordinate();
  TimelineCoordinate(const rational& frame, const Track::Reference& track);
  TimelineCoordinate(const rational& frame, const Track::Type& track_type, const int& track_index);

  [[nodiscard]] const rational& GetFrame() const;
  [[nodiscard]] const Track::Reference& GetTrack() const;

  void SetFrame(const rational& frame);
  void SetTrack(const Track::Reference& track);

 private:
  rational frame_;

  Track::Reference track_;
};

}  // namespace olive

#endif  // TIMELINECOORDINATE_H
