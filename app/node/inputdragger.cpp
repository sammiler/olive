#include "inputdragger.h"

#include "core.h"
#include "node.h"
#include "nodeundo.h"

namespace olive {

int NodeInputDragger::input_being_dragged = 0;

NodeInputDragger::NodeInputDragger() = default;

bool NodeInputDragger::IsStarted() const { return input_.IsValid(); }

void NodeInputDragger::Start(const NodeKeyframeTrackReference& input, const rational& time,
                             bool create_key_on_all_tracks) {
  Q_ASSERT(!IsStarted());

  // Set up new drag
  input_ = input;
  time_ = time;

  Node* node = input_.input().node();

  // Cache current value
  start_value_ = node->GetSplitValueAtTimeOnTrack(input_, time);
  end_value_ = start_value_;

  // Determine whether we are creating a keyframe or not
  if (input_.input().IsKeyframing()) {
    dragging_key_ = node->GetKeyframeAtTimeOnTrack(input_, time);

    if (!dragging_key_) {
      dragging_key_ = new NodeKeyframe(time, start_value_, node->GetBestKeyframeTypeForTimeOnTrack(input_, time),
                                       input_.track(), input_.input().element(), input_.input().input(), node);
      created_keys_.append(dragging_key_);

      if (create_key_on_all_tracks) {
        int nb_tracks =
            NodeValue::get_number_of_keyframe_tracks(input.input().node()->GetInputDataType(input.input().input()));
        for (int i = 0; i < nb_tracks; i++) {
          if (i != input.track()) {
            NodeKeyframeTrackReference this_ref(input.input(), i);
            created_keys_.append(new NodeKeyframe(time, node->GetSplitValueAtTimeOnTrack(this_ref, time),
                                                  node->GetBestKeyframeTypeForTimeOnTrack(this_ref, time), i,
                                                  input.input().element(), input.input().input(), node));
          }
        }
      }
    }
  }

  input_being_dragged++;
}

void NodeInputDragger::Drag(QVariant value) {
  Q_ASSERT(IsStarted());

  Node* node = input_.input().node();
  const QString& input = input_.input().input();

  if (node->HasInputProperty(input, QStringLiteral("min"))) {
    // Assumes the value is a double of some kind
    double min = node->GetInputProperty(input, QStringLiteral("min")).toDouble();
    double v = value.toDouble();
    if (v < min) {
      value = min;
    }
  }

  if (node->HasInputProperty(input, QStringLiteral("max"))) {
    double max = node->GetInputProperty(input, QStringLiteral("max")).toDouble();
    double v = value.toDouble();
    if (v > max) {
      value = max;
    }
  }

  end_value_ = value;

  // input_->blockSignals(true);

  if (input_.input().IsKeyframing()) {
    dragging_key_->set_value(value);
  } else {
    node->SetSplitStandardValueOnTrack(input_, value);
  }

  // input_->blockSignals(false);
}

void NodeInputDragger::End(MultiUndoCommand* command) {
  if (!IsStarted()) {
    return;
  }

  input_being_dragged--;

  if (input_.input().node()->IsInputKeyframing(input_.input())) {
    for (auto created_key : created_keys_) {
      // We created a keyframe in this process
      command->add_child(new NodeParamInsertKeyframeCommand(input_.input().node(), created_key));
    }

    // We just set a keyframe's value
    // We do this even when inserting a keyframe because we don't actually perform an insert in this undo command
    // so this will ensure the ValueChanged() signal is sent correctly
    command->add_child(new NodeParamSetKeyframeValueCommand(dragging_key_, end_value_, start_value_));
  } else {
    // We just set the standard value
    command->add_child(new NodeParamSetStandardValueCommand(input_, end_value_, start_value_));
  }

  input_.Reset();
  created_keys_.clear();
}

}  // namespace olive
