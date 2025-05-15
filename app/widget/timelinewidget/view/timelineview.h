

#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QGraphicsView>

#include "node/block/clip/clip.h"
#include "timelineviewghostitem.h"
#include "timelineviewmouseevent.h"
#include "widget/timebased/timebasedview.h"

namespace olive {

/**
 * @brief A widget for viewing and interacting Sequences
 *
 * This widget primarily exposes users to viewing and modifying Block nodes, usually through a TimelineOutput node.
 */
class TimelineView : public TimeBasedView {
  Q_OBJECT
 public:
  explicit TimelineView(Qt::Alignment vertical_alignment = Qt::AlignTop, QWidget *parent = nullptr);

  [[nodiscard]] int GetTrackY(int track_index) const;
  [[nodiscard]] int GetTrackHeight(int track_index) const;

  [[nodiscard]] QPoint GetScrollCoordinates() const;
  void SetScrollCoordinates(const QPoint &pt);

  void ConnectTrackList(TrackList *list);

  void SetBeamCursor(const TimelineCoordinate &coord);
  void SetTransitionOverlay(ClipBlock *out, ClipBlock *in);
  void EnableRecordingOverlay(const TimelineCoordinate &coord);
  void DisableRecordingOverlay();

  void SetSelectionList(QHash<Track::Reference, TimeRangeList> *s) { selections_ = s; }

  void SetGhostList(QVector<TimelineViewGhostItem *> *ghosts) { ghosts_ = ghosts; }

  int SceneToTrack(double y);

  [[nodiscard]] Block *GetItemAtScenePos(const rational &time, int track_index) const;

  [[nodiscard]] QVector<Block *> GetItemsAtSceneRect(const QRectF &rect) const;

 signals:
  void MousePressed(TimelineViewMouseEvent *event);
  void MouseMoved(TimelineViewMouseEvent *event);
  void MouseReleased(TimelineViewMouseEvent *event);
  void MouseDoubleClicked(TimelineViewMouseEvent *event);

  void DragEntered(TimelineViewMouseEvent *event);
  void DragMoved(TimelineViewMouseEvent *event);
  void DragLeft(QDragLeaveEvent *event);
  void DragDropped(TimelineViewMouseEvent *event);

 protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;

  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

  void drawBackground(QPainter *painter, const QRectF &rect) override;
  void drawForeground(QPainter *painter, const QRectF &rect) override;

  void ToolChangedEvent(Tool::Item tool) override;

  void SceneRectUpdateEvent(QRectF &rect) override;

 private:
  Track::Type ConnectedTrackType();

  TimelineCoordinate ScreenToCoordinate(const QPoint &pt);
  TimelineCoordinate SceneToCoordinate(const QPointF &pt);

  TimelineViewMouseEvent CreateMouseEvent(QMouseEvent *event);
  TimelineViewMouseEvent CreateMouseEvent(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

  void DrawBlocks(QPainter *painter, bool foreground);

  void DrawBlock(QPainter *painter, bool foreground, Block *block, qreal top, qreal height, const rational &in,
                 const rational &out, const rational &media_in);
  void DrawBlock(QPainter *painter, bool foreground, Block *block, qreal top, qreal height) {
    auto *cb = dynamic_cast<ClipBlock *>(block);
    return DrawBlock(painter, foreground, block, top, height, block->in(), block->out(),
                     cb ? cb->media_in() : rational(0));
  }

  void DrawZebraStripes(QPainter *painter, const QRectF &r);

  [[nodiscard]] int GetHeightOfAllTracks() const;

  void UpdatePlayheadRect();

  [[nodiscard]] qreal GetTimelineLeftBound() const;

  [[nodiscard]] qreal GetTimelineRightBound() const;

  static void DrawThumbnail(QPainter *painter, const FrameHashCache *thumbs, const rational &time, int x,
                            const QRect &preview_rect, QRect *thumb_rect);

  QHash<Track::Reference, TimeRangeList> *selections_;

  QVector<TimelineViewGhostItem *> *ghosts_;

  bool show_beam_cursor_;

  TimelineCoordinate cursor_coord_;

  TrackList *connected_track_list_;

  ClipBlock *transition_overlay_out_;
  ClipBlock *transition_overlay_in_;

  QMap<TimelineMarker *, QRectF> clip_marker_rects_;

  bool recording_overlay_{};
  TimelineCoordinate recording_coord_;

 private slots:
  void TrackListChanged();
};

}  // namespace olive

#endif  // TIMELINEVIEW_H
