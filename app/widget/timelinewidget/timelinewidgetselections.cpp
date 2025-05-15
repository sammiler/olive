#include "timelinewidgetselections.h"

namespace olive {

void TimelineWidgetSelections::ShiftTime(const rational &diff) {
  for (auto it = this->begin(); it != this->end(); it++) {
    it.value().shift(diff);
  }
}

void TimelineWidgetSelections::ShiftTracks(Track::Type type, int diff) {
  TimelineWidgetSelections cached_selections;

  {
    // Take all selections of this track type
    auto it = this->begin();
    while (it != this->end()) {
      if (it.key().type() == type) {
        cached_selections.insert(it.key(), it.value());
        it = this->erase(it);
      } else {
        it++;
      }
    }
  }

  // Then re-insert them with the diff applied
  for (auto it = cached_selections.cbegin(); it != cached_selections.cend(); it++) {
    Track::Reference ref(it.key().type(), it.key().index() + diff);

    this->insert(ref, it.value());
  }
}

void TimelineWidgetSelections::TrimIn(const rational &diff) {
  for (auto it = this->begin(); it != this->end(); it++) {
    it.value().trim_in(diff);
  }
}

void TimelineWidgetSelections::TrimOut(const rational &diff) {
  for (auto it = this->begin(); it != this->end(); it++) {
    it.value().trim_out(diff);
  }
}

void TimelineWidgetSelections::Subtract(const TimelineWidgetSelections &selections) {
  for (auto it = selections.cbegin(); it != selections.cend(); it++) {
    const Track::Reference &track = it.key();
    const TimeRangeList &their_list = it.value();

    if (this->contains(track)) {
      TimeRangeList &our_list = (*this)[it.key()];
      our_list.remove(their_list);
    }
  }
}

}  // namespace olive
