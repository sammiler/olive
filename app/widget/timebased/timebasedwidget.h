

#ifndef TIMEBASEDWIDGET_H
#define TIMEBASEDWIDGET_H

#include <QWidget>

#include "node/output/viewer/viewer.h"
#include "timeline/timelinecommon.h"
#include "widget/keyframeview/keyframeviewinputconnection.h"
#include "widget/resizablescrollbar/resizabletimelinescrollbar.h"
#include "widget/timebased/timescaledobject.h"
#include "widget/timelinewidget/view/timelineview.h"
#include "widget/timetarget/timetarget.h"

namespace olive {

class TimeRuler;

class TimeBasedWidget : public TimelineScaledWidget {
  Q_OBJECT
 public:
  explicit TimeBasedWidget(bool ruler_text_visible = true, bool ruler_cache_status_visible = false,
                           QWidget *parent = nullptr);

  void ZoomIn();

  void ZoomOut();

  [[nodiscard]] ViewerOutput *GetConnectedNode() const;

  void ConnectViewerNode(ViewerOutput *node);

  [[nodiscard]] TimelineWorkArea *GetConnectedWorkArea() const { return workarea_; }
  [[nodiscard]] TimelineMarkerList *GetConnectedMarkers() const { return markers_; }
  void ConnectWorkArea(TimelineWorkArea *workarea);
  void ConnectMarkers(TimelineMarkerList *markers);

  void SetScaleAndCenterOnPlayhead(const double &scale);

  [[nodiscard]] TimeRuler *ruler() const;

  using SnapMask = uint32_t;
  enum SnapPoints {
    kSnapToClips = 0x1,
    kSnapToPlayhead = 0x2,
    kSnapToMarkers = 0x4,
    kSnapToKeyframes = 0x8,
    kSnapToWorkarea = 0x10,
    kSnapAll = UINT32_MAX
  };

  /**
   * @brief Snaps point `start_point` that is moving by `movement` to currently existing clips
   */
  bool SnapPoint(const std::vector<rational> &start_times, rational *movement, SnapMask snap_points = kSnapAll);
  void ShowSnaps(const std::vector<rational> &times);
  void HideSnaps();

  virtual bool CopySelected(bool cut);

  virtual bool Paste();

 public slots:
  void SetTimebase(const rational &timebase);

  void SetScale(const double &scale);

  void GoToStart();

  void PrevFrame();

  void NextFrame();

  void GoToEnd();

  void GoToPrevCut();

  void GoToNextCut();

  void SetInAtPlayhead();

  void SetOutAtPlayhead();

  void ResetIn();

  void ResetOut();

  void ClearInOutPoints() const;

  void SetMarker();

  void ToggleShowAll();

  void GoToIn();

  void GoToOut();

  void DeleteSelected();

 protected:
  [[nodiscard]] ResizableTimelineScrollBar *scrollbar() const;

  void TimebaseChangedEvent(const rational &) override;

  virtual void TimeChangedEvent(const rational &) {}

  void ScaleChangedEvent(const double &) override;

  virtual void ConnectedNodeChangeEvent(ViewerOutput *) {}

  virtual void ConnectedWorkAreaChangeEvent(TimelineWorkArea *) {}
  virtual void ConnectedMarkersChangeEvent(TimelineMarkerList *) {}

  virtual void ConnectNodeEvent(ViewerOutput *) {}

  virtual void DisconnectNodeEvent(ViewerOutput *) {}

  void SetAutoMaxScrollBar(bool e);

  void resizeEvent(QResizeEvent *event) override;

  void ConnectTimelineView(TimeBasedView *base);

  void SetCatchUpScrollValue(QScrollBar *b, int v, int maximum);
  void StopCatchUpScrollTimer(QScrollBar *b);

  [[nodiscard]] virtual const QVector<Block *> *GetSnapBlocks() const { return nullptr; }
  [[nodiscard]] virtual const QVector<KeyframeViewInputConnection *> *GetSnapKeyframes() const { return nullptr; }
  [[nodiscard]] virtual const TimeTargetObject *GetKeyframeTimeTarget() const { return nullptr; }
  [[nodiscard]] virtual const std::vector<NodeKeyframe *> *GetSnapIgnoreKeyframes() const { return nullptr; }
  [[nodiscard]] virtual const std::vector<TimelineMarker *> *GetSnapIgnoreMarkers() const { return nullptr; }

 protected slots:
  /**
   * @brief Slot to center the horizontal scroll bar on the playhead's current position
   */
  void CenterScrollOnPlayhead();

  /**
   * @brief By default, TimeBasedWidget will set the timebase to the viewer node's video timebase.
   * Set this to false if you want to set your own timebase.
   */
  void SetAutoSetTimebase(bool e);

  static void PageScrollInternal(QScrollBar *bar, int maximum, int screen_position, bool whole_page_scroll);

  void StopCatchUpScrollTimer() { StopCatchUpScrollTimer(scrollbar_); }

  void SetCatchUpScrollValue(int v);

 signals:
  void TimebaseChanged(const rational &);

  void ConnectedNodeChanged(ViewerOutput *old, ViewerOutput *now);

 protected slots:
  virtual void SendCatchUpScrollEvent();

 private:
  /**
   * @brief Set either in or out point to the current playhead
   *
   * @param m
   *
   * Set to kTrimIn or kTrimOut for setting the in point or out point respectively.
   */
  void SetPoint(Timeline::MovementMode m, const rational &time);

  /**
   * @brief Reset either the in or out point
   *
   * Sets either the in point to 0 or the out point to `RATIONAL_MAX`.
   *
   * @param m
   *
   * Set to kTrimIn or kTrimOut for setting the in point or out point respectively.
   */
  void ResetPoint(Timeline::MovementMode m) const;

  void PageScrollInternal(int screen_position, bool whole_page_scroll);

  [[nodiscard]] bool UserIsDraggingPlayhead() const;

  ViewerOutput *viewer_node_;

  TimeRuler *ruler_;

  ResizableTimelineScrollBar *scrollbar_;

  bool auto_max_scrollbar_;

  QList<TimeBasedView *> timeline_views_;

  bool toggle_show_all_;

  double toggle_show_all_old_scale_{};
  int toggle_show_all_old_scroll_{};

  bool auto_set_timebase_;

  int scrollbar_start_width_{};
  double scrollbar_start_value_{};
  double scrollbar_start_scale_{};
  bool scrollbar_top_handle_{};

  TimelineWorkArea *workarea_;
  TimelineMarkerList *markers_;

  QTimer *catchup_scroll_timer_;
  struct CatchUpScrollData {
    qint64 last_forced = 0;
    int maximum{};
    int value{};
  };
  QMap<QScrollBar *, CatchUpScrollData> catchup_scroll_values_;

 private slots:
  void UpdateMaximumScroll();

  void ScrollBarResizeBegan(int current_bar_width, bool top_handle);

  void ScrollBarResizeMoved(int movement);

  /**
   * @brief Slot to handle page scrolling of the playhead
   *
   * If the playhead is outside the current scroll bounds, this function will scroll to where it is. Otherwise it will
   * do nothing.
   */
  void PageScrollToPlayhead();

  void CatchUpScrollToPlayhead();

  void CatchUpScrollToPoint(int point);

  void CatchUpTimerTimeout();

  void AutoUpdateTimebase();

  void ConnectedNodeRemovedFromGraph();

  void PlayheadTimeChanged(const rational &time);
};

}  // namespace olive

#endif  // TIMEBASEDWIDGET_H
