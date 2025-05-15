

#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QCheckBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

#include "curveview.h"
#include "widget/nodeparamview/nodeparamviewkeyframecontrol.h"
#include "widget/nodeparamview/nodeparamviewwidgetbridge.h"
#include "widget/nodetreeview/nodetreeview.h"
#include "widget/timebased/timebasedwidget.h"

namespace olive {

class CurveWidget : public TimeBasedWidget, public TimeTargetObject {
  Q_OBJECT
 public:
  explicit CurveWidget(QWidget *parent = nullptr);

  const double &GetVerticalScale();
  void SetVerticalScale(const double &vscale);

  void DeleteSelected();

  void SelectAll() { view_->SelectAll(); }

  void DeselectAll() { view_->DeselectAll(); }

  Node *GetSelectedNodeWithID(const QString &id);

  bool CopySelected(bool cut) override;

  bool Paste() override;

 public slots:
  void SetNodes(const QVector<Node *> &nodes);

 protected:
  void TimebaseChangedEvent(const rational &) override;
  void ScaleChangedEvent(const double &) override;

  void TimeTargetChangedEvent(ViewerOutput *target) override;

  void ConnectedNodeChangeEvent(ViewerOutput *n) override;

  [[nodiscard]] const QVector<KeyframeViewInputConnection *> *GetSnapKeyframes() const override {
    return &view_->GetKeyframeTracks();
  }

  [[nodiscard]] const TimeTargetObject *GetKeyframeTimeTarget() const override { return view_; }

  [[nodiscard]] const std::vector<NodeKeyframe *> *GetSnapIgnoreKeyframes() const override {
    return &view_->GetSelectedKeyframes();
  }

 private:
  void SetKeyframeButtonEnabled(bool enable);

  void SetKeyframeButtonChecked(bool checked);

  void SetKeyframeButtonCheckedFromType(NodeKeyframe::Type type);

  void ConnectInput(Node *node, const QString &input, int element);

  void ConnectInputInternal(Node *node, const QString &input, int element);

  QHash<NodeKeyframeTrackReference, QColor> keyframe_colors_;

  NodeTreeView *tree_view_;

  QPushButton *linear_button_;

  QPushButton *bezier_button_;

  QPushButton *hold_button_;

  CurveView *view_;

  NodeParamViewKeyframeControl *key_control_;

  QVector<Node *> nodes_;

  QVector<NodeKeyframeTrackReference> selected_tracks_;

 private slots:
  void SelectionChanged();

  void KeyframeTypeButtonTriggered(bool checked);

  void InputSelectionChanged(const NodeKeyframeTrackReference &ref);

  void KeyframeViewDragged(int x, int y);
  void KeyframeViewReleased();
};

}  // namespace olive

#endif  // CURVEWIDGET_H
