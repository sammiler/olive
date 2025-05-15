

#include "sequenceviewer.h"
#include "panel/timeline/timeline.h"

namespace olive {

SequenceViewerPanel::SequenceViewerPanel() : ViewerPanel(QStringLiteral("SequenceViewerPanel")) {
  // Set strings
  Retranslate();
}

void SequenceViewerPanel::StartCapture(const TimeRange &time, const Track::Reference &track) {
  auto *tp = dynamic_cast<TimelinePanel *>(sender());
  dynamic_cast<ViewerWidget *>(GetTimeBasedWidget())->StartCapture(tp->timeline_widget(), time, track);
}

void SequenceViewerPanel::Retranslate() {
  ViewerPanel::Retranslate();

  SetTitle(tr("Sequence Viewer"));
}

}  // namespace olive
