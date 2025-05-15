#include "timelinecoordinate.h"

namespace olive {

TimelineCoordinate::TimelineCoordinate() : track_(Track::kNone, 0) {}

TimelineCoordinate::TimelineCoordinate(const rational &frame, const Track::Reference &track)
    : frame_(frame), track_(track) {}

TimelineCoordinate::TimelineCoordinate(const rational &frame, const Track::Type &track_type, const int &track_index)
    : frame_(frame), track_(track_type, track_index) {}

const rational &TimelineCoordinate::GetFrame() const { return frame_; }

const Track::Reference &TimelineCoordinate::GetTrack() const { return track_; }

void TimelineCoordinate::SetFrame(const rational &frame) { frame_ = frame; }

void TimelineCoordinate::SetTrack(const Track::Reference &track) { track_ = track; }

}  // namespace olive
