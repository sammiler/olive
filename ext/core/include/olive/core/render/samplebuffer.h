

#ifndef LIBOLIVECORE_SAMPLEBUFFER_H
#define LIBOLIVECORE_SAMPLEBUFFER_H

#include <memory>
#include <vector>

#include "../util/rational.h"
#include "audioparams.h"

namespace olive::core {

/**
 * @brief A buffer of audio samples
 *
 * Audio samples in this structure are always stored in PLANAR (separated by channel). This is done to simplify audio
 * rendering code. This replaces the old system of using QByteArrays (containing packed audio) and while SampleBuffer
 * replaces many of those in the rendering/processing side of things, QByteArrays are currently still in use for
 * playback, including reading to and from the cache.
 */
class SampleBuffer {
 public:
  SampleBuffer();
  SampleBuffer(AudioParams audio_params, const rational& length);
  SampleBuffer(AudioParams audio_params, size_t samples_per_channel);

  [[nodiscard]] const AudioParams& audio_params() const;
  void set_audio_params(const AudioParams& params);

  [[nodiscard]] const size_t& sample_count() const { return sample_count_per_channel_; }
  void set_sample_count(const size_t& sample_count);
  void set_sample_count(const rational& length) { set_sample_count(audio_params_.time_to_samples(length)); }

  float* data(int channel) { return data_[channel].data(); }

  [[nodiscard]] const float* data(int channel) const { return data_.at(channel).data(); }

  std::vector<float*> to_raw_ptrs() {
    std::vector<float*> r(data_.size());
    for (size_t i = 0; i < r.size(); i++) {
      r[i] = data_[i].data();
    }
    return r;
  }

  [[nodiscard]] int channel_count() const { return data_.size(); }

  [[nodiscard]] bool is_allocated() const { return !data_.empty(); }
  void allocate();
  void destroy();

  void reverse();
  void speed(double speed);
  void transform_volume(float f);
  void transform_volume_for_channel(int channel, float volume);
  void transform_volume_for_sample(size_t sample_index, float volume);
  void transform_volume_for_sample_on_channel(size_t sample_index, int channel, float volume);

  void clamp();

  void silence();
  void silence(size_t start_sample, size_t end_sample);
  void silence_bytes(size_t start_byte, size_t end_byte);

  void set(int channel, const float* data, size_t sample_offset, size_t sample_length);
  void set(int channel, const float* data, size_t sample_length) { set(channel, data, 0, sample_length); }

 private:
  void clamp_channel(int channel);

  AudioParams audio_params_;

  size_t sample_count_per_channel_;

  std::vector<std::vector<float> > data_;
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_SAMPLEBUFFER_H
