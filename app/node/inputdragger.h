#ifndef NODEINPUTDRAGGER_H
#define NODEINPUTDRAGGER_H

#include "node/keyframe.h"
#include "node/param.h"
#include "undo/undocommand.h"

namespace olive {

class NodeInputDragger {
 public:
  NodeInputDragger();

  [[nodiscard]] bool IsStarted() const;

  void Start(const NodeKeyframeTrackReference &input, const rational &time, bool create_key_on_all_tracks = true);

  void Drag(QVariant value);

  void End(MultiUndoCommand *command);

  static bool IsInputBeingDragged() { return input_being_dragged; }

  [[nodiscard]] const QVariant &GetStartValue() const { return start_value_; }

  [[nodiscard]] const NodeKeyframeTrackReference &GetInput() const { return input_; }

  [[nodiscard]] const rational &GetTime() const { return time_; }

 private:
  NodeKeyframeTrackReference input_;

  rational time_;

  QVariant start_value_;

  QVariant end_value_;

  NodeKeyframe *dragging_key_{};
  QVector<NodeKeyframe *> created_keys_;

  static int input_being_dragged;
};

}  // namespace olive

#endif  // NODEINPUTDRAGGER_H
