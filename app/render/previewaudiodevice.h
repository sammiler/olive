

#ifndef PREVIEWAUDIODEVICE_H
#define PREVIEWAUDIODEVICE_H

#include "previewautocacher.h"

namespace olive {

class PreviewAudioDevice : public QIODevice {
  Q_OBJECT
 public:
  explicit PreviewAudioDevice(QObject *parent = nullptr);

  ~PreviewAudioDevice() override;

  void StartQueuing();

  [[nodiscard]] bool isSequential() const override;

  qint64 readData(char *data, qint64 maxSize) override;

  qint64 writeData(const char *data, qint64 length) override;

  [[nodiscard]] int bytes_per_frame() const { return bytes_per_frame_; }

  void set_bytes_per_frame(int b) { bytes_per_frame_ = b; }

  void set_notify_interval(qint64 i) { notify_interval_ = i; }

  void clear();

 signals:
  void Notify();

 private:
  QMutex lock_;

  QByteArray buffer_;

  int bytes_per_frame_{};

  qint64 notify_interval_;

  qint64 bytes_read_;
};

}  // namespace olive

#endif  // PREVIEWAUDIODEVICE_H
