/***

  Olive - Non-Linear Video Editor
  Copyright (C) 2023 Olive Studios LLC

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

***/

#ifndef LIBOLIVECORE_TIMERANGE_H
#define LIBOLIVECORE_TIMERANGE_H

#include <list>
#include <vector>

#include "rational.h"

namespace olive::core {

class TimeRange {
 public:
  TimeRange() = default;
  TimeRange(const rational& in, const rational& out);
  TimeRange(const TimeRange& r) : TimeRange(r.in(), r.out()) {}

  TimeRange& operator=(const TimeRange& r) {
    set_range(r.in(), r.out());
    return *this;
  }

  [[nodiscard]] const rational& in() const;
  [[nodiscard]] const rational& out() const;
  [[nodiscard]] const rational& length() const;

  void set_in(const rational& in);
  void set_out(const rational& out);
  void set_range(const rational& in, const rational& out);

  bool operator==(const TimeRange& r) const;
  bool operator!=(const TimeRange& r) const;

  [[nodiscard]] bool OverlapsWith(const TimeRange& a, bool in_inclusive = true, bool out_inclusive = true) const;
  [[nodiscard]] bool Contains(const TimeRange& compare, bool in_inclusive = true, bool out_inclusive = true) const;
  [[nodiscard]] bool Contains(const rational& r) const;

  [[nodiscard]] TimeRange Combined(const TimeRange& a) const;
  static TimeRange Combine(const TimeRange& a, const TimeRange& b);
  [[nodiscard]] TimeRange Intersected(const TimeRange& a) const;
  static TimeRange Intersect(const TimeRange& a, const TimeRange& b);

  TimeRange operator+(const rational& rhs) const;
  TimeRange operator-(const rational& rhs) const;

  const TimeRange& operator+=(const rational& rhs);
  const TimeRange& operator-=(const rational& rhs);

  [[nodiscard]] std::list<TimeRange> Split(const int& chunk_size) const;

 private:
  void normalize();

  rational in_;
  rational out_;
  rational length_;
};

class TimeRangeList {
 public:
  TimeRangeList() = default;

  TimeRangeList(std::initializer_list<TimeRange> r) : array_(r) {}

  void insert(const TimeRangeList& list_to_add);
  void insert(TimeRange range_to_add);

  void remove(const TimeRange& remove);
  void remove(const TimeRangeList& list);

  template <typename T>
  static void util_remove(std::vector<T>* list, const TimeRange& remove) {
    std::vector<T> additions;

    for (auto it = list->begin(); it != list->end();) {
      T& compare = *it;

      if (remove.Contains(compare)) {
        // This element is entirely encompassed in this range, remove it
        it = list->erase(it);
      } else {
        if (compare.Contains(remove, false, false)) {
          // The remove range is within this element, only choice is to split the element into two
          T new_range = compare;
          new_range.set_in(remove.out());
          compare.set_out(remove.in());

          additions.push_back(new_range);
          break;
        } else {
          if (compare.in() < remove.in() && compare.out() > remove.in()) {
            // This element's out point overlaps the range's in, we'll trim it
            compare.set_out(remove.in());
          } else if (compare.in() < remove.out() && compare.out() > remove.out()) {
            // This element's in point overlaps the range's out, we'll trim it
            compare.set_in(remove.out());
          }

          it++;
        }
      }
    }

    list->insert(list->end(), additions.begin(), additions.end());
  }

  [[nodiscard]] bool contains(const TimeRange& range, bool in_inclusive = true, bool out_inclusive = true) const;

  [[nodiscard]] bool contains(const rational& r) const {
    for (const TimeRange& range : array_) {
      if (range.Contains(r)) {
        return true;
      }
    }

    return false;
  }

  [[nodiscard]] bool OverlapsWith(const TimeRange& r, bool in_inclusive = true, bool out_inclusive = true) const {
    for (const TimeRange& range : array_) {
      if (range.OverlapsWith(r, in_inclusive, out_inclusive)) {
        return true;
      }
    }

    return false;
  }

  [[nodiscard]] bool isEmpty() const { return array_.empty(); }

  void clear() { array_.clear(); }

  [[nodiscard]] int size() const { return array_.size(); }

  void shift(const rational& diff);

  void trim_in(const rational& diff);

  void trim_out(const rational& diff);

  [[nodiscard]] TimeRangeList Intersects(const TimeRange& range) const;

  using const_iterator = std::vector<TimeRange>::const_iterator;

  [[nodiscard]] const_iterator begin() const { return array_.cbegin(); }

  [[nodiscard]] const_iterator end() const { return array_.cend(); }

  [[nodiscard]] const_iterator cbegin() const { return begin(); }

  [[nodiscard]] const_iterator cend() const { return end(); }

  [[nodiscard]] const TimeRange& first() const { return array_.front(); }

  [[nodiscard]] const TimeRange& last() const { return array_.back(); }

  [[nodiscard]] const TimeRange& at(int index) const { return array_.at(index); }

  [[nodiscard]] const std::vector<TimeRange>& internal_array() const { return array_; }

  bool operator==(const TimeRangeList& rhs) const { return array_ == rhs.array_; }

 private:
  std::vector<TimeRange> array_;
};

class TimeRangeListFrameIterator {
 public:
  TimeRangeListFrameIterator();
  TimeRangeListFrameIterator(TimeRangeList list, const rational& timebase);

  [[nodiscard]] rational Snap(const rational& r) const;

  bool GetNext(rational* out);

  [[nodiscard]] bool HasNext() const;

  [[nodiscard]] std::vector<rational> ToVector() const {
    TimeRangeListFrameIterator copy(list_, timebase_);
    std::vector<rational> times;
    rational r;
    while (copy.GetNext(&r)) {
      times.push_back(r);
    }
    return times;
  }

  int size();

  void reset() { *this = TimeRangeListFrameIterator(); }

  void insert(const TimeRange& range) { list_.insert(range); }

  void insert(const TimeRangeList& list) { list_.insert(list); }

  [[nodiscard]] bool IsCustomRange() const { return custom_range_; }

  void SetCustomRange(bool e) { custom_range_ = e; }

  [[nodiscard]] int frame_index() const { return frame_index_; }

 private:
  void UpdateIndexIfNecessary();

  TimeRangeList list_;

  rational timebase_;

  rational current_;

  int range_index_;

  int size_;

  int frame_index_;

  bool custom_range_;
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_TIMERANGE_H
