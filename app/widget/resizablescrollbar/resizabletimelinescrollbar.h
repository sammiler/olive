#ifndef RESIZABLETIMELINESCROLLBAR_H
#define RESIZABLETIMELINESCROLLBAR_H

#include "resizablescrollbar.h"
#include "timeline/timelinemarker.h"
#include "timeline/timelineworkarea.h"
#include "widget/timebased/timescaledobject.h"

namespace olive {

class ResizableTimelineScrollBar : public ResizableScrollBar, public TimeScaledObject {
  Q_OBJECT
 public:
  explicit ResizableTimelineScrollBar(QWidget* parent = nullptr);
  explicit ResizableTimelineScrollBar(Qt::Orientation orientation, QWidget* parent = nullptr);

  void ConnectMarkers(TimelineMarkerList* markers);
  void ConnectWorkArea(TimelineWorkArea* workarea);

  void SetScale(double d);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  TimelineMarkerList* markers_;

  TimelineWorkArea* workarea_;

  double scale_;
};

}  // namespace olive

#endif  // RESIZABLETIMELINESCROLLBAR_H
