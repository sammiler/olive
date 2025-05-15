

#ifndef TIMELINEWIDGETSELECTIONS_H
#define TIMELINEWIDGETSELECTIONS_H

#include <QHash>

#include "node/output/track/track.h"

namespace olive {

class TimelineWidgetSelections : public QHash<Track::Reference, TimeRangeList> {
 public:
  TimelineWidgetSelections() = default;

  void ShiftTime(const rational& diff);

  void ShiftTracks(Track::Type type, int diff);

  void TrimIn(const rational& diff);

  void TrimOut(const rational& diff);

  void Subtract(const TimelineWidgetSelections& selections);

  [[nodiscard]] TimelineWidgetSelections Subtracted(const TimelineWidgetSelections& selections) const {
    TimelineWidgetSelections copy = *this;
    copy.Subtract(selections);
    return copy;
  }
};

}  // namespace olive

#endif  // TIMELINEWIDGETSELECTIONS_H
