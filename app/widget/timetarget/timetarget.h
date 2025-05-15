

#ifndef TIMETARGETOBJECT_H
#define TIMETARGETOBJECT_H

#include "node/output/viewer/viewer.h"

namespace olive {

class TimeTargetObject {
 public:
  TimeTargetObject();

  [[nodiscard]] ViewerOutput* GetTimeTarget() const;
  void SetTimeTarget(ViewerOutput* target);

  void SetPathIndex(int index);

  rational GetAdjustedTime(Node* from, Node* to, const rational& r, Node::TransformTimeDirection dir) const;
  TimeRange GetAdjustedTime(Node* from, Node* to, const TimeRange& r, Node::TransformTimeDirection dir) const;

  // int GetNumberOfPathAdjustments(Node* from, NodeParam::Type direction) const;

 protected:
  virtual void TimeTargetDisconnectEvent(ViewerOutput*) {}
  virtual void TimeTargetChangedEvent(ViewerOutput*) {}
  virtual void TimeTargetConnectEvent(ViewerOutput*) {}

 private:
  ViewerOutput* time_target_;

  int path_index_;
};

}  // namespace olive

#endif  // TIMETARGETOBJECT_H
