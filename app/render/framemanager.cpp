#include "framemanager.h"

#include <QDateTime>
#include <QDebug>

namespace olive {

FrameManager* FrameManager::instance_ = nullptr;
const int FrameManager::kFrameLifetime = 5000;

void FrameManager::CreateInstance() { instance_ = new FrameManager(); }

void FrameManager::DestroyInstance() {
  delete instance_;
  instance_ = nullptr;
}

FrameManager* FrameManager::instance() { return instance_; }

char* FrameManager::Allocate(int size) {
  if (instance()) {
    return instance()->AllocateFromPool(size);
  } else {
    return new char[size];
  }
}

void FrameManager::Deallocate(int size, char* buffer) {
  if (instance()) {
    instance()->DeallocateToPool(size, buffer);
  } else {
    delete[] buffer;
  }
}

FrameManager::FrameManager() {
  clear_timer_.setInterval(kFrameLifetime);
  connect(&clear_timer_, &QTimer::timeout, this, &FrameManager::GarbageCollection);
  clear_timer_.start();
}

char* FrameManager::AllocateFromPool(int size) {
  QMutexLocker locker(&mutex_);

  std::list<Buffer>& buffer_list = pool_[size];
  char* buf = nullptr;

  if (buffer_list.empty()) {
    buf = new char[size];
  } else {
    // Take this buffer from the list
    buf = buffer_list.front().data;
    buffer_list.pop_front();
  }

  return buf;
}

void FrameManager::DeallocateToPool(int size, char* buffer) {
  QMutexLocker locker(&mutex_);

  std::list<Buffer>& buffer_list = pool_[size];

  buffer_list.push_back({QDateTime::currentMSecsSinceEpoch(), buffer});
}

void FrameManager::GarbageCollection() {
  QMutexLocker locker(&mutex_);

  qint64 min_life = QDateTime::currentMSecsSinceEpoch() - kFrameLifetime;

  for (auto& it : pool_) {
    std::list<Buffer>& list = it.second;

    while (!list.empty() && list.front().time < min_life) {
      delete[] list.front().data;
      list.pop_front();
    }
  }
}

FrameManager::~FrameManager() {
  QMutexLocker locker(&mutex_);

  for (auto& it : pool_) {
    std::list<Buffer>& list = it.second;
    for (auto& jt : list) {
      delete[] jt.data;
    }
  }

  pool_.clear();
}

}  // namespace olive
