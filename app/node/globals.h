#ifndef NODEGLOBALS_H
#define NODEGLOBALS_H

#include <QVector2D>
#include <utility>

#include "render/loopmode.h"
#include "render/videoparams.h"

namespace olive {

class NodeGlobals {
 public:
  NodeGlobals() = default;

  NodeGlobals(VideoParams vparam, AudioParams aparam, const TimeRange &time, LoopMode loop_mode)
      : video_params_(std::move(vparam)), audio_params_(std::move(aparam)), time_(time), loop_mode_(loop_mode) {}

  NodeGlobals(const VideoParams &vparam, const AudioParams &aparam, const rational &time, LoopMode loop_mode)
      : NodeGlobals(vparam, aparam, TimeRange(time, time + vparam.frame_rate_as_time_base()), loop_mode) {}

  [[nodiscard]] QVector2D square_resolution() const { return video_params_.square_resolution(); }
  [[nodiscard]] QVector2D nonsquare_resolution() const { return video_params_.resolution(); }
  [[nodiscard]] const AudioParams &aparams() const { return audio_params_; }
  [[nodiscard]] const VideoParams &vparams() const { return video_params_; }
  [[nodiscard]] const TimeRange &time() const { return time_; }
  [[nodiscard]] LoopMode loop_mode() const { return loop_mode_; }

 private:
  VideoParams video_params_;
  AudioParams audio_params_;
  TimeRange time_;
  LoopMode loop_mode_;
};

}  // namespace olive

#endif  // NODEGLOBALS_H
