

#ifndef KEYFRAMEVIEWINPUTCONNECTION_H
#define KEYFRAMEVIEWINPUTCONNECTION_H

#include <QObject>

#include "node/node.h"
#include "node/param.h"

namespace olive {

class KeyframeView;

class KeyframeViewInputConnection : public QObject {
  Q_OBJECT
 public:
  KeyframeViewInputConnection(const NodeKeyframeTrackReference &input, KeyframeView *parent);

  [[nodiscard]] const int &GetKeyframeY() const { return y_; }

  void SetKeyframeY(int y);

  enum YBehavior { kSingleRow, kValueIsHeight };

  void SetYBehavior(YBehavior e);

  [[nodiscard]] const QVector<NodeKeyframe *> &GetKeyframes() const {
    return input_.input().node()->GetKeyframeTracks(input_.input()).at(input_.track());
  }

  [[nodiscard]] const QBrush &GetBrush() const { return brush_; }

  [[nodiscard]] const NodeKeyframeTrackReference &GetReference() const { return input_; }

  void SetBrush(const QBrush &brush);

 signals:
  void RequireUpdate();

  void TypeChanged();

 private:
  KeyframeView *keyframe_view_;

  NodeKeyframeTrackReference input_;

  int y_;

  YBehavior y_behavior_;

  QBrush brush_;

 private slots:
  void AddKeyframe(NodeKeyframe *key);

  void RemoveKeyframe(NodeKeyframe *key);

  void KeyframeChanged(NodeKeyframe *key);

  void KeyframeTypeChanged(NodeKeyframe *key);
};

}  // namespace olive

#endif  // KEYFRAMEVIEWINPUTCONNECTION_H
