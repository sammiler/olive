

#include "keyframeviewinputconnection.h"

#include "keyframeview.h"

namespace olive {

KeyframeViewInputConnection::KeyframeViewInputConnection(const NodeKeyframeTrackReference &input, KeyframeView *parent)
    : QObject(parent), keyframe_view_(parent), input_(input), y_(0), y_behavior_(kSingleRow), brush_(Qt::white) {
  Node *n = input.input().node();

  connect(n, &Node::KeyframeAdded, this, &KeyframeViewInputConnection::AddKeyframe);
  connect(n, &Node::KeyframeRemoved, this, &KeyframeViewInputConnection::RemoveKeyframe);
  connect(n, &Node::KeyframeTimeChanged, this, &KeyframeViewInputConnection::KeyframeChanged);
  connect(n, &Node::KeyframeTypeChanged, this, &KeyframeViewInputConnection::KeyframeChanged);
  connect(n, &Node::KeyframeTypeChanged, this, &KeyframeViewInputConnection::KeyframeTypeChanged);
  connect(n, &Node::KeyframeValueChanged, this, &KeyframeViewInputConnection::KeyframeChanged);
}

void KeyframeViewInputConnection::SetKeyframeY(int y) {
  if (y_ != y) {
    y_ = y;

    emit RequireUpdate();
  }
}

void KeyframeViewInputConnection::SetYBehavior(YBehavior e) {
  if (y_behavior_ != e) {
    y_behavior_ = e;

    emit RequireUpdate();
  }
}

void KeyframeViewInputConnection::SetBrush(const QBrush &brush) {
  if (brush_ != brush) {
    brush_ = brush;

    emit RequireUpdate();
  }
}

void KeyframeViewInputConnection::AddKeyframe(NodeKeyframe *key) {
  if (key->key_track_ref() == input_) {
    emit RequireUpdate();
  }
}

void KeyframeViewInputConnection::RemoveKeyframe(NodeKeyframe *key) {
  if (key->key_track_ref() == input_) {
    emit RequireUpdate();
  }
}

void KeyframeViewInputConnection::KeyframeChanged(NodeKeyframe *key) {
  if (key->key_track_ref() == input_) {
    emit RequireUpdate();
  }
}

void KeyframeViewInputConnection::KeyframeTypeChanged(NodeKeyframe *key) {
  if (key->key_track_ref() == input_) {
    emit TypeChanged();
  }
}

}  // namespace olive
