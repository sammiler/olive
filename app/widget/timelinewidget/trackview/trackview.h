#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QScrollArea>
#include <QSplitter>

#include "node/output/track/tracklist.h"
#include "trackviewitem.h"
#include "trackviewsplitter.h"

namespace olive {

class TrackView : public QScrollArea {
  Q_OBJECT
 public:
  explicit TrackView(Qt::Alignment vertical_alignment = Qt::AlignTop, QWidget* parent = nullptr);

  void ConnectTrackList(TrackList* list);
  void DisconnectTrackList();

 signals:
  void AboutToDeleteTrack(Track* track);

 protected:
  void resizeEvent(QResizeEvent* e) override;

 private:
  TrackList* list_;

  TrackViewSplitter* splitter_;

  Qt::Alignment alignment_;

  int last_scrollbar_max_;

 private slots:
  void ScrollbarRangeChanged(int min, int max);

  void TrackHeightChanged(int index, int height);

  void InsertTrack(Track* track);

  void RemoveTrack(Track* track);
};

}  // namespace olive

#endif  // TRACKVIEW_H
