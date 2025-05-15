#ifndef TIMEBASEDPANEL_H
#define TIMEBASEDPANEL_H

#include "panel/panel.h"
#include "widget/timebased/timebasedwidget.h"

namespace olive {

class TimeBasedPanel : public PanelWidget {
  Q_OBJECT
 public:
  explicit TimeBasedPanel(const QString& object_name);

  ~TimeBasedPanel() override;

  void ConnectViewerNode(ViewerOutput* node);

  void DisconnectViewerNode() { ConnectViewerNode(nullptr); }

  // Get the timebase of this panels widget
  const rational& timebase();

  [[nodiscard]] ViewerOutput* GetConnectedViewer() const { return widget_->GetConnectedNode(); }

  [[nodiscard]] TimeRuler* ruler() const { return widget_->ruler(); }

  void ZoomIn() override;

  void ZoomOut() override;

  void GoToStart() override;

  void PrevFrame() override;

  void NextFrame() override;

  void GoToEnd() override;

  void GoToPrevCut() override;

  void GoToNextCut() override;

  void PlayPause() override;

  void PlayInToOut() override;

  void ShuttleLeft() override;

  void ShuttleStop() override;

  void ShuttleRight() override;

  void SetIn() override;

  void SetOut() override;

  void ResetIn() override;

  void ResetOut() override;

  void ClearInOut() override;

  void SetMarker() override;

  void ToggleShowAll() override;

  void GoToIn() override;

  void GoToOut() override;

  void DeleteSelected() override;

  void CutSelected() override;

  void CopySelected() override;

  void Paste() override;

  [[nodiscard]] TimeBasedWidget* GetTimeBasedWidget() const { return widget_; }

 public slots:
  void SetTimebase(const rational& timebase);

 signals:
  void PlayPauseRequested();

  void PlayInToOutRequested();

  void ShuttleLeftRequested();

  void ShuttleStopRequested();

  void ShuttleRightRequested();

 protected:
  void SetTimeBasedWidget(TimeBasedWidget* widget);

  void Retranslate() override;

  void SetShowAndRaiseOnConnect() { show_and_raise_on_connect_ = true; }

 private:
  TimeBasedWidget* widget_;

  bool show_and_raise_on_connect_;

 private slots:
  void ConnectedNodeChanged(ViewerOutput* old, ViewerOutput* now);
};

}  // namespace olive

#endif  // TIMEBASEDPANEL_H
