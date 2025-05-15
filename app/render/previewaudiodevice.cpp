#include "previewaudiodevice.h"

namespace olive {

PreviewAudioDevice::PreviewAudioDevice(QObject *parent) : notify_interval_(0), bytes_read_(0) {}

PreviewAudioDevice::~PreviewAudioDevice() { close(); }

bool PreviewAudioDevice::isSequential() const { return true; }

qint64 PreviewAudioDevice::readData(char *data, qint64 maxSize) {
  QMutexLocker locker(&lock_);

  qint64 copy_length = qMin(maxSize, qint64(buffer_.size()));

  if (copy_length) {
    qint64 new_bytes_read = bytes_read_ + copy_length;

    if (notify_interval_ > 0) {
      if ((bytes_read_ / notify_interval_) != (new_bytes_read / notify_interval_)) {
        emit Notify();
      }
    }

    bytes_read_ = new_bytes_read;

    memcpy(data, buffer_.constData(), copy_length);
    buffer_ = buffer_.mid(copy_length);
  }

  return copy_length;
}

qint64 PreviewAudioDevice::writeData(const char *data, qint64 length) {
  QMutexLocker locker(&lock_);

  buffer_.append(data, length);

  return length;
}

void PreviewAudioDevice::clear() {
  QMutexLocker locker(&lock_);

  buffer_.clear();
  bytes_read_ = 0;
}

}  // namespace olive
