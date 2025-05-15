

#ifndef VIEWERQUEUE_H
#define VIEWERQUEUE_H

#include "codec/frame.h"

namespace olive {

struct ViewerPlaybackFrame {
  rational timestamp;
  QVariant frame;
};

class ViewerQueue : public std::list<ViewerPlaybackFrame> {
 public:
  ViewerQueue() = default;

  void AppendTimewise(const ViewerPlaybackFrame& f, int playback_speed) {
    if (this->empty() || (this->back().timestamp < f.timestamp) == (playback_speed > 0)) {
      this->push_back(f);
    } else {
      for (auto i = this->begin(); i != this->end(); i++) {
        if ((i->timestamp > f.timestamp) == (playback_speed > 0)) {
          this->insert(i, f);
          break;
        }
      }
    }
  }

  void PurgeBefore(const rational& time, int playback_speed) {
    while (!this->empty() && ((playback_speed > 0 && this->front().timestamp < time) ||
                              (playback_speed < 0 && this->front().timestamp > time))) {
      this->pop_front();
    }
  }
};

}  // namespace olive

#endif  // VIEWERQUEUE_H
