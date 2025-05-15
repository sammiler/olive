#include "viewerbase.h"

#include "window/mainwindow/mainwindow.h"

namespace olive {

#define super TimeBasedPanel

ViewerPanelBase::ViewerPanelBase(const QString &object_name) : super(object_name) {
  connect(PanelManager::instance(), &PanelManager::FocusedPanelChanged, this, &ViewerPanelBase::FocusedPanelChanged);
}

void ViewerPanelBase::PlayPause() { GetViewerWidget()->TogglePlayPause(); }

void ViewerPanelBase::PlayInToOut() { GetViewerWidget()->Play(true); }

void ViewerPanelBase::ShuttleLeft() { GetViewerWidget()->ShuttleLeft(); }

void ViewerPanelBase::ShuttleStop() { GetViewerWidget()->ShuttleStop(); }

void ViewerPanelBase::ShuttleRight() { GetViewerWidget()->ShuttleRight(); }

void ViewerPanelBase::ConnectTimeBasedPanel(TimeBasedPanel *panel) const {
  connect(panel, &TimeBasedPanel::PlayPauseRequested, this, &ViewerPanelBase::PlayPause);
  connect(panel, &TimeBasedPanel::PlayInToOutRequested, this, &ViewerPanelBase::PlayInToOut);
  connect(panel, &TimeBasedPanel::ShuttleLeftRequested, this, &ViewerPanelBase::ShuttleLeft);
  connect(panel, &TimeBasedPanel::ShuttleStopRequested, this, &ViewerPanelBase::ShuttleStop);
  connect(panel, &TimeBasedPanel::ShuttleRightRequested, this, &ViewerPanelBase::ShuttleRight);
}

void ViewerPanelBase::DisconnectTimeBasedPanel(TimeBasedPanel *panel) const {
  disconnect(panel, &TimeBasedPanel::PlayPauseRequested, this, &ViewerPanelBase::PlayPause);
  disconnect(panel, &TimeBasedPanel::PlayInToOutRequested, this, &ViewerPanelBase::PlayInToOut);
  disconnect(panel, &TimeBasedPanel::ShuttleLeftRequested, this, &ViewerPanelBase::ShuttleLeft);
  disconnect(panel, &TimeBasedPanel::ShuttleStopRequested, this, &ViewerPanelBase::ShuttleStop);
  disconnect(panel, &TimeBasedPanel::ShuttleRightRequested, this, &ViewerPanelBase::ShuttleRight);
}

void ViewerPanelBase::SetFullScreen(QScreen *screen) const { GetViewerWidget()->SetFullScreen(screen); }

void ViewerPanelBase::SetGizmos(Node *node) const { GetViewerWidget()->SetGizmos(node); }

void ViewerPanelBase::CacheEntireSequence() const { GetViewerWidget()->CacheEntireSequence(); }

void ViewerPanelBase::CacheSequenceInOut() const { GetViewerWidget()->CacheSequenceInOut(); }

void ViewerPanelBase::SetViewerWidget(ViewerWidget *vw) {
  connect(vw, &ViewerWidget::TextureChanged, this, &ViewerPanelBase::TextureChanged);
  connect(vw, &ViewerWidget::ColorProcessorChanged, this, &ViewerPanelBase::ColorProcessorChanged);
  connect(vw, &ViewerWidget::ColorManagerChanged, this, &ViewerPanelBase::ColorManagerChanged);

  SetTimeBasedWidget(vw);
}

void ViewerPanelBase::FocusedPanelChanged(PanelWidget *panel) {
  if (dynamic_cast<ViewerPanelBase *>(panel)) {
    auto vw = GetViewerWidget();
    if (vw->IsPlaying() && panel != this) {
      vw->Pause();
    }
  }
}

}  // namespace olive
