

#include "timetarget.h"

namespace olive {

TimeTargetObject::TimeTargetObject() : time_target_(nullptr), path_index_(0) {}

ViewerOutput* TimeTargetObject::GetTimeTarget() const { return time_target_; }

void TimeTargetObject::SetTimeTarget(ViewerOutput* target) {
  if (time_target_) {
    TimeTargetDisconnectEvent(time_target_);
  }

  time_target_ = target;
  TimeTargetChangedEvent(time_target_);

  if (time_target_) {
    TimeTargetConnectEvent(time_target_);
  }
}

void TimeTargetObject::SetPathIndex(int index) { path_index_ = index; }

rational TimeTargetObject::GetAdjustedTime(Node* from, Node* to, const rational& r,
                                           Node::TransformTimeDirection dir) const {
  if (!from || !to) {
    return r;
  }

  return GetAdjustedTime(from, to, TimeRange(r, r), dir).in();
}

TimeRange TimeTargetObject::GetAdjustedTime(Node* from, Node* to, const TimeRange& r,
                                            Node::TransformTimeDirection dir) const {
  if (!from || !to) {
    return r;
  }

  return from->TransformTimeTo(r, to, dir, path_index_);
}

/*int TimeTargetObject::GetNumberOfPathAdjustments(Node* from, NodeParam::Type direction) const
{
  if (!time_target_) {
    return 0;
  }

  QList<TimeRange> adjusted = from->TransformTimeTo(TimeRange(), time_target_, direction);

  return adjusted.size();
}*/

}  // namespace olive
