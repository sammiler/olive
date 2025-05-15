#include "audioplaybackcache.h"

#include <QDir>
#include <QFile>
#include <QRandomGenerator>
#include <QUuid>

#include "common/filefunctions.h"
#include "node/output/viewer/viewer.h"

namespace olive {

const qint64 AudioPlaybackCache::kDefaultSegmentSizePerChannel = 10 * 1024 * 1024;

AudioPlaybackCache::AudioPlaybackCache(QObject *parent) : PlaybackCache(parent) {}

AudioPlaybackCache::~AudioPlaybackCache() = default;

void AudioPlaybackCache::SetParameters(const AudioParams &params) {
  if (params_ == params) {
    return;
  }

  params_ = params;
}

void AudioPlaybackCache::WritePCM(const TimeRange &range, const TimeRangeList &valid_ranges,
                                  const SampleBuffer &samples) {
  for (const TimeRange &r : valid_ranges) {
    if (WritePartOfSampleBuffer(samples, r.in(), r.in() - range.in(), r.length())) {
      Validate(r);
    }
  }
}

void AudioPlaybackCache::WriteSilence(const TimeRange &range) {
  // WritePCM will automatically fill non-existent bytes with silence, so we just have to send
  // it an empty sample buffer
  WritePCM(range, {range}, SampleBuffer());
}

bool AudioPlaybackCache::WritePartOfSampleBuffer(const SampleBuffer &samples, const rational &write_start,
                                                 const rational &buffer_start, const rational &length) {
  int64_t length_in_bytes = params_.time_to_bytes_per_channel(length);

  int64_t start_cache_offset = params_.time_to_bytes_per_channel(write_start);
  int64_t end_cache_offset = start_cache_offset + length_in_bytes;

  int64_t start_buffer_offset = params_.time_to_bytes_per_channel(buffer_start);
  int64_t end_buffer_offset =
      std::min(start_buffer_offset + length_in_bytes, params_.samples_to_bytes_per_channel(samples.sample_count()));

  int64_t current_cache_offset = start_cache_offset;
  int64_t current_buffer_offset = start_buffer_offset;

  bool success = true;

  while (current_cache_offset != end_cache_offset) {
    int64_t segment = current_cache_offset / kDefaultSegmentSizePerChannel;
    int64_t segment_start = segment * kDefaultSegmentSizePerChannel;
    int64_t segment_end = segment_start + kDefaultSegmentSizePerChannel;

    int64_t offset_in_segment = current_cache_offset - segment_start;
    int64_t write_len = segment_end - offset_in_segment;
    int64_t max_buffer_len = end_buffer_offset - current_buffer_offset;
    int64_t zero_len = 0;

    if (write_len > max_buffer_len) {
      zero_len = write_len - max_buffer_len;
      write_len = max_buffer_len;
    }

    for (int channel = 0; channel < params_.channel_count(); channel++) {
      QString filename = GetSegmentFilename(segment, channel);

      if (!FileFunctions::DirectoryIsValid(QFileInfo(filename).dir())) {
        success = false;
        break;
      }

      QFile f(filename);
      if (f.open(QFile::ReadWrite)) {
        f.seek(offset_in_segment);
        f.write(reinterpret_cast<const char *>(samples.data(channel)) + current_buffer_offset, write_len);

        if (zero_len > 0) {
          QByteArray b(zero_len, 0);
          f.write(b.constData());
        }

        f.close();
      } else {
        success = false;
      }
    }

    current_cache_offset += write_len;
    current_buffer_offset += write_len;
  }

  return success;
}

QString AudioPlaybackCache::GetSegmentFilename(qint64 segment_index, int channel) {
  return GetThisCacheDirectory().filePath(
      QStringLiteral("%1.%2").arg(QString::number(segment_index), QString::number(channel)));
}

}  // namespace olive
