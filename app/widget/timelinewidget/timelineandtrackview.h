

#ifndef TIMELINEANDTRACKVIEW_H
#define TIMELINEANDTRACKVIEW_H

#include <QSplitter>
#include <QWidget>

#include "trackview/trackview.h"
#include "view/timelineview.h"

namespace olive {

class TimelineAndTrackView : public QWidget {
 public:
  explicit TimelineAndTrackView(Qt::Alignment vertical_alignment = Qt::AlignTop, QWidget* parent = nullptr);

  [[nodiscard]] QSplitter* splitter() const;

  [[nodiscard]] TimelineView* view() const;

  [[nodiscard]] TrackView* track_view() const;

 private:
  QSplitter* splitter_;

  TimelineView* view_;

  TrackView* track_view_;

 private slots:
  void ViewValueChanged(int v);

  void TracksValueChanged(int v);
};

}  // namespace olive

#endif  // TIMELINEANDTRACKVIEW_H
