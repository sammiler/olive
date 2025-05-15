

#ifndef AUDIOMONITORWIDGET_H
#define AUDIOMONITORWIDGET_H

#include <QFile>
#include <QOpenGLWidget>
#include <QTimer>

#include "audio/audiovisualwaveform.h"
#include "common/define.h"
#include "render/audiowaveformcache.h"

namespace olive {

class AudioMonitor : public QOpenGLWidget {
  Q_OBJECT
 public:
  explicit AudioMonitor(QWidget *parent = nullptr);

  ~AudioMonitor() override;

  [[nodiscard]] bool IsPlaying() const { return waveform_; }

  static void StartWaveformOnAll(const AudioWaveformCache *waveform, const rational &start, int playback_speed) {
    foreach (AudioMonitor *m, instances_) {
      m->StartWaveform(waveform, start, playback_speed);
    }
  }

  static void StopOnAll() {
    foreach (AudioMonitor *m, instances_) {
      m->Stop();
    }
  }

  static void PushSampleBufferOnAll(const SampleBuffer &d) {
    foreach (AudioMonitor *m, instances_) {
      m->PushSampleBuffer(d);
    }
  }

 public slots:
  void SetParams(const AudioParams &params);

  void Stop();

  void PushSampleBuffer(const SampleBuffer &d);

  void StartWaveform(const AudioWaveformCache *waveform, const rational &start, int playback_speed);

 protected:
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;

 private:
  void SetUpdateLoop(bool e);

  void UpdateValuesFromWaveform(QVector<double> &v, qint64 delta_time);

  static void AudioVisualWaveformSampleToInternalValues(const AudioVisualWaveform::Sample &in, QVector<double> &out);

  void PushValue(const QVector<double> &v);

  void BytesToSampleSummary(const QByteArray &bytes, QVector<double> &v);

  [[nodiscard]] QVector<double> GetAverages() const;

  AudioParams params_;

  qint64 last_time_{};

  const AudioWaveformCache *waveform_;
  rational waveform_time_;
  rational waveform_length_;

  int playback_speed_{};

  QVector<QVector<double> > values_;
  QVector<bool> peaked_;

  QPixmap cached_background_;
  int cached_channels_;

  static QVector<AudioMonitor *> instances_;
};

}  // namespace olive

#endif  // AUDIOMONITORWIDGET_H
