

#include "planarfiledevice.h"

namespace olive {

PlanarFileDevice::PlanarFileDevice(QObject *parent) : QObject(parent) {}

PlanarFileDevice::~PlanarFileDevice() { close(); }

bool PlanarFileDevice::open(const QVector<QString> &filenames, QIODevice::OpenMode mode) {
  if (isOpen()) {
    // Already open
    return false;
  }

  files_.resize(filenames.size());
  files_.fill(nullptr);

  for (int i = 0; i < files_.size(); i++) {
    files_[i] = new QFile(filenames.at(i));
    if (!files_[i]->open(mode)) {
      close();
      return false;
    }
  }

  return true;
}

qint64 PlanarFileDevice::read(char **data, qint64 bytes_per_channel, qint64 offset) {
  qint64 ret = -1;

  if (isOpen()) {
    for (int i = 0; i < files_.size(); i++) {
      // Kind of clunky but should be largely fine
      ret = files_[i]->read(data[i] + offset, bytes_per_channel);
    }
  }

  return ret;
}

qint64 PlanarFileDevice::write(const char **data, qint64 bytes_per_channel, qint64 offset) {
  qint64 ret = -1;

  if (isOpen()) {
    for (int i = 0; i < files_.size(); i++) {
      // Kind of clunky but should be largely fine
      ret = files_[i]->write(data[i] + offset, bytes_per_channel);
    }
  }

  return ret;
}

qint64 PlanarFileDevice::size() const {
  if (isOpen()) {
    return files_.first()->size();
  } else {
    return 0;
  }
}

bool PlanarFileDevice::seek(qint64 pos) {
  bool ret = true;

  for (auto &file : files_) {
    ret = file->seek(pos) & ret;
  }

  return ret;
}

void PlanarFileDevice::close() {
  for (auto f : files_) {
    if (f) {
      if (f->isOpen()) {
        f->close();
      }
      delete f;
    }
  }
  files_.clear();
}

}  // namespace olive
