

#include "keyframeviewundo.h"

#include "node/node.h"
#include "node/project/sequence/sequence.h"

namespace olive {

KeyframeSetTypeCommand::KeyframeSetTypeCommand(NodeKeyframe* key, NodeKeyframe::Type type)
    : key_(key), old_type_(key->type()), new_type_(type) {}

Project* KeyframeSetTypeCommand::GetRelevantProject() const { return key_->parent()->project(); }

void KeyframeSetTypeCommand::redo() { key_->set_type(new_type_); }

void KeyframeSetTypeCommand::undo() { key_->set_type(old_type_); }

KeyframeSetBezierControlPoint::KeyframeSetBezierControlPoint(NodeKeyframe* key, NodeKeyframe::BezierType mode,
                                                             const QPointF& point)
    : key_(key), mode_(mode), old_point_(key->bezier_control(mode_)), new_point_(point) {}

KeyframeSetBezierControlPoint::KeyframeSetBezierControlPoint(NodeKeyframe* key, NodeKeyframe::BezierType mode,
                                                             const QPointF& new_point, const QPointF& old_point)
    : key_(key), mode_(mode), old_point_(old_point), new_point_(new_point) {}

Project* KeyframeSetBezierControlPoint::GetRelevantProject() const { return key_->parent()->project(); }

void KeyframeSetBezierControlPoint::redo() { key_->set_bezier_control(mode_, new_point_); }

void KeyframeSetBezierControlPoint::undo() { key_->set_bezier_control(mode_, old_point_); }

}  // namespace olive
