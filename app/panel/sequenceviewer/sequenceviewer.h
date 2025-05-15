

#ifndef SEQUENCEVIEWERPANEL_H
#define SEQUENCEVIEWERPANEL_H

#include "panel/viewer/viewer.h"

namespace olive {

class SequenceViewerPanel : public ViewerPanel {
  Q_OBJECT
 public:
  SequenceViewerPanel();

 public slots:
  void StartCapture(const TimeRange &time, const Track::Reference &track);

 protected:
  void Retranslate() override;
};

}  // namespace olive

#endif  // SEQUENCEVIEWERPANEL_H
