#ifndef FOOTAGEJOB_H
#define FOOTAGEJOB_H

#include <utility>

#include "node/project/footage/footage.h"

namespace olive {

class FootageJob : public AcceleratedJob {
 public:
  FootageJob() : type_(Track::kNone) {}

  FootageJob(const TimeRange& time, QString decoder, QString filename, Track::Type type, const rational& length,
             LoopMode loop_mode)
      : time_(time),
        decoder_(std::move(decoder)),
        filename_(std::move(filename)),
        type_(type),
        length_(length),
        loop_mode_(loop_mode) {}

  [[nodiscard]] const QString& decoder() const { return decoder_; }

  [[nodiscard]] const QString& filename() const { return filename_; }

  [[nodiscard]] Track::Type type() const { return type_; }

  [[nodiscard]] const VideoParams& video_params() const { return video_params_; }

  void set_video_params(const VideoParams& p) { video_params_ = p; }

  [[nodiscard]] const AudioParams& audio_params() const { return audio_params_; }

  void set_audio_params(const AudioParams& p) { audio_params_ = p; }

  [[nodiscard]] const QString& cache_path() const { return cache_path_; }

  void set_cache_path(const QString& p) { cache_path_ = p; }

  [[nodiscard]] const rational& length() const { return length_; }

  void set_length(const rational& length) { length_ = length; }

  [[nodiscard]] const TimeRange& time() const { return time_; }

  [[nodiscard]] LoopMode loop_mode() const { return loop_mode_; }
  void set_loop_mode(LoopMode m) { loop_mode_ = m; }

 private:
  TimeRange time_;

  QString decoder_;

  QString filename_;

  Track::Type type_;

  VideoParams video_params_;

  AudioParams audio_params_;

  QString cache_path_;

  rational length_;

  LoopMode loop_mode_;
};

}  // namespace olive

Q_DECLARE_METATYPE(olive::FootageJob)

#endif  // FOOTAGEJOB_H
