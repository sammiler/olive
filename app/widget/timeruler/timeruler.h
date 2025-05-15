

#ifndef TIMERULER_H
#define TIMERULER_H

#include <QTimer>
#include <QWidget>

#include "render/playbackcache.h"
#include "seekablewidget.h"

namespace olive {

class TimeRuler : public SeekableWidget {
  Q_OBJECT
 public:
  explicit TimeRuler(bool text_visible = true, bool cache_status_visible = false, QWidget* parent = nullptr);

  void SetCenteredText(bool c);

  void SetPlaybackCache(PlaybackCache* cache);

 protected:
  void drawForeground(QPainter* painter, const QRectF& rect) override;

  void TimebaseChangedEvent(const rational& tb) override;

 protected slots:
  bool ShowContextMenu(const QPoint& p) override;

 private:
  void UpdateHeight();

  [[nodiscard]] int CacheStatusHeight() const;

  int minimum_gap_between_lines_;

  bool text_visible_;

  bool centered_text_;

  double timebase_flipped_dbl_{};

  bool show_cache_status_;

  PlaybackCache* playback_cache_;
};

}  // namespace olive

#endif  // TIMERULER_H
