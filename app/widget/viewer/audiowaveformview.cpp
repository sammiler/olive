#include "audiowaveformview.h"

#include <QFile>
#include <QPainter>
#include <QtMath>

#include "config/config.h"
#include "timeline/timelinecommon.h"

namespace olive {

#define super SeekableWidget

AudioWaveformView::AudioWaveformView(QWidget *parent) : super(parent), playback_(nullptr) {
  setAutoFillBackground(true);
  setBackgroundRole(QPalette::Base);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // NOTE: At some point it might make sense for this to be AlignCenter since the waveform
  //       originates from the center. But we're leaving it top/left for now since it was just
  //       ported from a QWidget's paintEvent.
  setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void AudioWaveformView::SetViewer(ViewerOutput *playback) {
  if (playback_) {
    pool_.clear();
    pool_.waitForDone();

    disconnect(playback_, &ViewerOutput::ConnectedWaveformChanged, viewport(),
               static_cast<void (QWidget::*)()>(&QWidget::update));

    SetTimebase(rational(0));
  }

  playback_ = playback;

  if (playback_) {
    connect(playback_, &ViewerOutput::ConnectedWaveformChanged, viewport(),
            static_cast<void (QWidget::*)()>(&QWidget::update));

    SetTimebase(playback_->GetAudioParams().sample_rate_as_time_base());
  }
}

void AudioWaveformView::drawForeground(QPainter *p, const QRectF &rect) {
  super::drawForeground(p, rect);

  if (!playback_) {
    return;
  }

  const AudioWaveformCache *wave = playback_->GetConnectedWaveform();
  if (!wave) {
    return;
  }

  const AudioParams &params = wave->GetParameters();
  if (!params.is_valid()) {
    return;
  }

  // Draw in/out points
  DrawWorkArea(p);
  DrawMarkers(p);

  // Draw waveform
  p->setPen(QColor(64, 255, 160));  // FIXME: Hardcoded color
  wave->Draw(p, rect.toRect(), GetScale(), SceneToTime(GetScroll()));

  // Draw playhead
  p->setPen(PLAYHEAD_COLOR);

  int playhead_x = TimeToScene(GetViewerNode()->GetPlayhead());
  p->drawLine(playhead_x, 0, playhead_x, height());
}

}  // namespace olive
