

#ifndef AUDIOWAVEFORMVIEW_H
#define AUDIOWAVEFORMVIEW_H

#include <QWidget>
#include <QtConcurrent/QtConcurrent>

#include "render/audioplaybackcache.h"
#include "widget/timeruler/seekablewidget.h"

namespace olive {

class AudioWaveformView : public SeekableWidget {
  Q_OBJECT
 public:
  explicit AudioWaveformView(QWidget *parent = nullptr);

  void SetViewer(ViewerOutput *playback);

 protected:
  void drawForeground(QPainter *painter, const QRectF &rect) override;

 private:
  QThreadPool pool_;

  ViewerOutput *playback_;
};

}  // namespace olive

#endif  // AUDIOWAVEFORMVIEW_H
