

#include "timebased.h"

namespace olive {

TimeBasedPanel::TimeBasedPanel(const QString &object_name)
    : PanelWidget(object_name), widget_(nullptr), show_and_raise_on_connect_(false) {}

TimeBasedPanel::~TimeBasedPanel() { delete widget_; }

const rational &TimeBasedPanel::timebase() { return widget_->timebase(); }

void TimeBasedPanel::GoToStart() { widget_->GoToStart(); }

void TimeBasedPanel::PrevFrame() { widget_->PrevFrame(); }

void TimeBasedPanel::NextFrame() { widget_->NextFrame(); }

void TimeBasedPanel::GoToEnd() { widget_->GoToEnd(); }

void TimeBasedPanel::ZoomIn() { widget_->ZoomIn(); }

void TimeBasedPanel::ZoomOut() { widget_->ZoomOut(); }

void TimeBasedPanel::SetTimebase(const rational &timebase) { widget_->SetTimebase(timebase); }

void TimeBasedPanel::GoToPrevCut() { widget_->GoToPrevCut(); }

void TimeBasedPanel::GoToNextCut() { widget_->GoToNextCut(); }

void TimeBasedPanel::PlayPause() { emit PlayPauseRequested(); }

void TimeBasedPanel::PlayInToOut() { emit PlayInToOutRequested(); }

void TimeBasedPanel::ShuttleLeft() { emit ShuttleLeftRequested(); }

void TimeBasedPanel::ShuttleStop() { emit ShuttleStopRequested(); }

void TimeBasedPanel::ShuttleRight() { emit ShuttleRightRequested(); }

void TimeBasedPanel::ConnectViewerNode(ViewerOutput *node) { widget_->ConnectViewerNode(node); }

void TimeBasedPanel::SetTimeBasedWidget(TimeBasedWidget *widget) {
  if (widget_) {
    disconnect(widget_, &TimeBasedWidget::ConnectedNodeChanged, this, &TimeBasedPanel::ConnectedNodeChanged);
  }

  widget_ = widget;

  if (widget_) {
    connect(widget_, &TimeBasedWidget::ConnectedNodeChanged, this, &TimeBasedPanel::ConnectedNodeChanged);
  }

  SetWidgetWithPadding(widget_);
}

void TimeBasedPanel::Retranslate() {
  if (GetTimeBasedWidget()->GetConnectedNode()) {
    SetSubtitle(GetTimeBasedWidget()->GetConnectedNode()->GetLabel());
  } else {
    SetSubtitle(tr("(none)"));
  }
}

void TimeBasedPanel::ConnectedNodeChanged(ViewerOutput *old, ViewerOutput *now) {
  if (old) {
    disconnect(old, &ViewerOutput::LabelChanged, this, &TimeBasedPanel::SetSubtitle);
  }

  if (now) {
    connect(now, &ViewerOutput::LabelChanged, this, &TimeBasedPanel::SetSubtitle);

    if (show_and_raise_on_connect_) {
      this->show();
      this->raise();
    }
  }

  // Update strings
  Retranslate();
}

void TimeBasedPanel::SetIn() { GetTimeBasedWidget()->SetInAtPlayhead(); }

void TimeBasedPanel::SetOut() { GetTimeBasedWidget()->SetOutAtPlayhead(); }

void TimeBasedPanel::ResetIn() { GetTimeBasedWidget()->ResetIn(); }

void TimeBasedPanel::ResetOut() { GetTimeBasedWidget()->ResetOut(); }

void TimeBasedPanel::ClearInOut() { GetTimeBasedWidget()->ClearInOutPoints(); }

void TimeBasedPanel::SetMarker() { GetTimeBasedWidget()->SetMarker(); }

void TimeBasedPanel::ToggleShowAll() { GetTimeBasedWidget()->ToggleShowAll(); }

void TimeBasedPanel::GoToIn() { GetTimeBasedWidget()->GoToIn(); }

void TimeBasedPanel::GoToOut() { GetTimeBasedWidget()->GoToOut(); }

void TimeBasedPanel::DeleteSelected() { GetTimeBasedWidget()->DeleteSelected(); }

void TimeBasedPanel::CutSelected() { GetTimeBasedWidget()->CopySelected(true); }

void TimeBasedPanel::CopySelected() { GetTimeBasedWidget()->CopySelected(false); }

void TimeBasedPanel::Paste() { GetTimeBasedWidget()->Paste(); }

}  // namespace olive
