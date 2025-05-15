#ifndef AUDIOWAVEFORMCACHE_H
#define AUDIOWAVEFORMCACHE_H

#include "audio/audiovisualwaveform.h"
#include "playbackcache.h"

namespace olive {

class AudioWaveformCache : public PlaybackCache {
  Q_OBJECT
 public:
  explicit AudioWaveformCache(QObject *parent = nullptr);

  void WriteWaveform(const TimeRange &range, const TimeRangeList &valid_ranges, const AudioVisualWaveform *waveform);

  [[nodiscard]] const AudioParams &GetParameters() const { return params_; }
  void SetParameters(const AudioParams &p) {
    params_ = p;
    waveforms_->set_channel_count(p.channel_count());
  }

  void Draw(QPainter *painter, const QRect &rect, const double &scale, const rational &start_time) const;

  [[nodiscard]] AudioVisualWaveform::Sample GetSummaryFromTime(const rational &start, const rational &length) const;

  [[nodiscard]] rational length() const;

  void SetPassthrough(PlaybackCache *cache) override;

 protected:
  void InvalidateEvent(const TimeRange &range) override;

 private:
  using WaveformPtr = std::shared_ptr<AudioVisualWaveform>;

  WaveformPtr waveforms_;

  AudioParams params_;

  class WaveformPassthrough : public TimeRange {
   public:
    explicit WaveformPassthrough(const TimeRange &r) : TimeRange(r) {}

    WaveformPtr waveform;
  };

  std::vector<WaveformPassthrough> passthroughs_;
};

}  // namespace olive

#endif  // AUDIOWAVEFORMCACHE_H
