#ifndef KEYFRAMEVIEWUNDO_H
#define KEYFRAMEVIEWUNDO_H

#include "node/keyframe.h"
#include "undo/undocommand.h"

namespace olive {

class KeyframeSetTypeCommand : public UndoCommand {
 public:
  KeyframeSetTypeCommand(NodeKeyframe* key, NodeKeyframe::Type type);

  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  void redo() override;
  void undo() override;

 private:
  NodeKeyframe* key_;

  NodeKeyframe::Type old_type_;

  NodeKeyframe::Type new_type_;
};

class KeyframeSetBezierControlPoint : public UndoCommand {
 public:
  KeyframeSetBezierControlPoint(NodeKeyframe* key, NodeKeyframe::BezierType mode, const QPointF& point);
  KeyframeSetBezierControlPoint(NodeKeyframe* key, NodeKeyframe::BezierType mode, const QPointF& new_point,
                                const QPointF& old_point);

  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  void redo() override;
  void undo() override;

 private:
  NodeKeyframe* key_;

  NodeKeyframe::BezierType mode_;

  QPointF old_point_;

  QPointF new_point_;
};

}  // namespace olive

#endif  // KEYFRAMEVIEWUNDO_H
