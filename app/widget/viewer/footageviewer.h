#ifndef FOOTAGEVIEWERWIDGET_H
#define FOOTAGEVIEWERWIDGET_H

#include "node/output/viewer/viewer.h"
#include "viewer.h"

namespace olive {

class FootageViewerWidget : public ViewerWidget {
  Q_OBJECT
 public:
  explicit FootageViewerWidget(QWidget *parent = nullptr);

  void OverrideWorkArea(const TimeRange &r);
  void ResetWorkArea();

 private:
  void StartFootageDragInternal(bool enable_video, bool enable_audio);

  TimelineWorkArea *override_workarea_;

 private slots:
  void StartFootageDrag();

  void StartVideoDrag();

  void StartAudioDrag();

  void VideoButtonClicked();

  void AudioButtonClicked();
};

}  // namespace olive

#endif  // FOOTAGEVIEWERWIDGET_H
