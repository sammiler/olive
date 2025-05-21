#include <cstring>

#include "util/tests.h"
#include "util/timerange.h"

using namespace olive::core;

bool timerangelist_remove_test() {
  TimeRangeList t;

  t.insert(TimeRange(rational(0), rational(30)));
  t.remove(TimeRange(rational(2), rational(5)));

  return true;
}

bool timerangelist_mergeadjacent_test() {
  TimeRangeList t;

  // TimeRangeList should merge 1 and 3 together since they're adjacent
  t.insert(TimeRange(rational(0), rational(6)));
  t.insert(TimeRange(rational(20), rational(30)));
  t.insert(TimeRange(rational(6), rational(10)));

  if (!(t.size() == 2)) {
    return false;
  }
  if (!(t.first() == TimeRange(rational(20), rational(30)))) {
    return false;
  }
  if (!(t.at(1) == TimeRange(rational(0), rational(10)))) {
    return false;
  }

  // TimeRangeList should ignore these because it's already contained
  TimeRangeList noop_test = t;

  noop_test.insert(TimeRange(rational(4), rational(7)));
  if (!(noop_test == t)) {
    return false;
  }

  noop_test.insert(TimeRange(rational(0), rational(3)));
  if (!(noop_test == t)) {
    return false;
  }

  noop_test.insert(TimeRange(rational(25), rational(30)));
  if (!(noop_test == t)) {
    return false;
  }

  // TimeRangeList should combine all these together
  TimeRangeList combine_test_no_overlap = t;
  combine_test_no_overlap.insert(TimeRange(rational(10), rational(20)));
  if (!(combine_test_no_overlap.size() == 1)) {
    return false;
  }
  if (!(combine_test_no_overlap.first() == TimeRange(rational(0), rational(30)))) {
    return false;
  }

  TimeRangeList combine_test_in_overlap = t;
  combine_test_in_overlap.insert(TimeRange(rational(9), rational(20)));
  if (!(combine_test_in_overlap.size() == 1)) {
    return false;
  }
  if (!(combine_test_in_overlap.first() == TimeRange(rational(0), rational(30)))) {
    return false;
  }

  TimeRangeList combine_test_out_overlap = t;
  combine_test_out_overlap.insert(TimeRange(rational(10), rational(21)));
  if (!(combine_test_out_overlap.size() == 1)) {
    return false;
  }
  if (!(combine_test_out_overlap.first() == TimeRange(rational(0), rational(30)))) {
    return false;
  }

  TimeRangeList combine_test_both_overlap = t;
  combine_test_both_overlap.insert(TimeRange(rational(9), rational(21)));
  if (!(combine_test_both_overlap.size() == 1)) {
    return false;
  }
  if (!(combine_test_both_overlap.first() == TimeRange(rational(0), rational(30)))) {
    return false;
  }

  return true;
}

int main() {
  Tester t;

  t.add("TimeRangeList::remove", timerangelist_remove_test);
  t.add("TimeRangeList::merge_adjacent", timerangelist_mergeadjacent_test);

  return t.exec();
}
