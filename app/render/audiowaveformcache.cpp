#include "audiowaveformcache.h"

namespace olive {

#define super PlaybackCache

AudioWaveformCache::AudioWaveformCache(QObject *parent) : super{parent} {
  waveforms_ = std::make_shared<AudioVisualWaveform>();
}

void AudioWaveformCache::WriteWaveform(const TimeRange &range, const TimeRangeList &valid_ranges,
                                       const AudioVisualWaveform *waveform) {
  // Write each valid range to the segments
  foreach (const TimeRange &r, valid_ranges) {
    if (waveform) {
      waveforms_->OverwriteSums(*waveform, r.in(), r.in() - range.in(), r.length());
    }

    Validate(r);
  }
}

void DrawSubRect(QPainter *painter, const QRect &rect, const double &scale, const TimeRange &wave_range,
                 const AudioVisualWaveform &waveform, const TimeRange &subrange) {
  // Find start time of passthrough
  TimeRange intersect = wave_range.Intersected(subrange);

  // Create new rect that starts at the offset of pass_start from start_time
  // Set rect width to either length of passthrough or until the end
  QRect pass_rect(rect.x() + (intersect.in() - wave_range.in()).toDouble() * scale, rect.y(),
                  intersect.length().toDouble() * scale, rect.height());

  // Draw waveform with this info
  AudioVisualWaveform::DrawWaveform(painter, pass_rect, scale, waveform, intersect.in());
}

void AudioWaveformCache::Draw(QPainter *painter, const QRect &rect, const double &scale,
                              const rational &start_time) const {
  if (!passthroughs_.empty()) {
    TimeRange wave_range(start_time, start_time + rational::fromDouble(rect.width() / scale));
    TimeRangeList draw_range = {wave_range};
    for (const WaveformPassthrough &p : passthroughs_) {
      if (draw_range.OverlapsWith(p, true, false)) {
        DrawSubRect(painter, rect, scale, wave_range, *p.waveform, p);

        // Remove this range
        draw_range.remove(p);
      }
    }

    for (const TimeRange &r : draw_range) {
      DrawSubRect(painter, rect, scale, wave_range, *waveforms_, r);
    }
  } else {
    AudioVisualWaveform::DrawWaveform(painter, rect, scale, *waveforms_, start_time);
  }
}

AudioVisualWaveform::Sample AudioWaveformCache::GetSummaryFromTime(const rational &start,
                                                                   const rational &length) const {
  return waveforms_->GetSummaryFromTime(start, length);
}

rational AudioWaveformCache::length() const { return waveforms_->length(); }

void AudioWaveformCache::SetPassthrough(PlaybackCache *cache) {
  auto *c = dynamic_cast<AudioWaveformCache *>(cache);

  for (const TimeRange &r : c->GetValidatedRanges()) {
    WaveformPassthrough t = WaveformPassthrough(r);
    t.waveform = c->waveforms_;
    passthroughs_.push_back(t);
  }
  passthroughs_.insert(passthroughs_.end(), c->passthroughs_.begin(), c->passthroughs_.end());

  SetParameters(c->GetParameters());
  SetSavingEnabled(c->IsSavingEnabled());
}

void AudioWaveformCache::InvalidateEvent(const TimeRange &range) {
  TimeRangeList::util_remove(&passthroughs_, range);

  super::InvalidateEvent(range);
}

}  // namespace olive
