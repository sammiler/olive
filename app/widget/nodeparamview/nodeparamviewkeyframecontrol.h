

#ifndef NODEPARAMVIEWKEYFRAMECONTROL_H
#define NODEPARAMVIEWKEYFRAMECONTROL_H

#include <QPushButton>
#include <QWidget>

#include "node/param.h"
#include "widget/timetarget/timetarget.h"

namespace olive {

class NodeParamViewKeyframeControl : public QWidget, public TimeTargetObject {
  Q_OBJECT
 public:
  explicit NodeParamViewKeyframeControl(bool right_align, QWidget* parent = nullptr);
  explicit NodeParamViewKeyframeControl(QWidget* parent = nullptr) : NodeParamViewKeyframeControl(true, parent) {}

  [[nodiscard]] const NodeInput& GetConnectedInput() const { return input_; }

  void SetInput(const NodeInput& input);

 protected:
  void TimeTargetDisconnectEvent(ViewerOutput* v) override;
  void TimeTargetConnectEvent(ViewerOutput* v) override;

 private:
  [[nodiscard]] static QPushButton* CreateNewToolButton(const QIcon& icon);

  void SetButtonsEnabled(bool e);

  [[nodiscard]] rational GetCurrentTimeAsNodeTime() const;

  [[nodiscard]] rational ConvertToViewerTime(const rational& r) const;

  QPushButton* prev_key_btn_;
  QPushButton* toggle_key_btn_;
  QPushButton* next_key_btn_;
  QPushButton* enable_key_btn_;

  NodeInput input_;

 private slots:
  void ShowButtonsFromKeyframeEnable(bool e);

  void ToggleKeyframe(bool e);

  void UpdateState();

  void GoToPreviousKey();

  void GoToNextKey();

  void KeyframeEnableBtnClicked(bool e);

  void KeyframeEnableChanged(const NodeInput& input, bool e);
};

}  // namespace olive

#endif  // NODEPARAMVIEWKEYFRAMECONTROL_H
