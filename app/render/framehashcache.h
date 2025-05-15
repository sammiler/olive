

#ifndef VIDEORENDERFRAMECACHE_H
#define VIDEORENDERFRAMECACHE_H

#include "codec/frame.h"
#include "render/playbackcache.h"
#include "render/videoparams.h"

namespace olive {

class FrameHashCache : public PlaybackCache {
  Q_OBJECT
 public:
  explicit FrameHashCache(QObject *parent = nullptr);

  [[nodiscard]] const rational &GetTimebase() const { return timebase_; }

  void SetTimebase(const rational &tb);

  void ValidateTimestamp(const int64_t &ts);
  void ValidateTime(const rational &time);

  [[nodiscard]] bool IsFrameCached(const rational &time) const { return GetValidatedRanges().contains(time); }

  [[nodiscard]] QString GetValidCacheFilename(const rational &time) const;

  static bool SaveCacheFrame(const QString &filename, const FramePtr &frame);
  [[nodiscard]] bool SaveCacheFrame(const int64_t &time, FramePtr frame) const;
  static bool SaveCacheFrame(const QString &cache_path, const QUuid &uuid, const int64_t &time, const FramePtr &frame);
  static bool SaveCacheFrame(const QString &cache_path, const QUuid &uuid, const rational &time, const rational &tb,
                             const FramePtr &frame);
  static FramePtr LoadCacheFrame(const QString &cache_path, const QUuid &uuid, const int64_t &time);
  [[nodiscard]] FramePtr LoadCacheFrame(const int64_t &hash) const;
  static FramePtr LoadCacheFrame(const QString &fn);

  void SetPassthrough(PlaybackCache *cache) override;

 protected:
  void LoadStateEvent(QDataStream &stream) override;
  void SaveStateEvent(QDataStream &stream) override;

 private:
  [[nodiscard]] rational ToTime(const int64_t &ts) const;
  [[nodiscard]] int64_t ToTimestamp(const rational &ts, Timecode::Rounding rounding = Timecode::kRound) const;

  /**
   * @brief Return the path of the cached image at this time
   */
  [[nodiscard]] QString CachePathName(const int64_t &time) const;
  [[nodiscard]] QString CachePathName(const rational &time) const;

  static QString CachePathName(const QString &cache_path, const QUuid &cache_id, const int64_t &time);
  static QString CachePathName(const QString &cache_path, const QUuid &cache_id, const rational &time,
                               const rational &tb);

  rational timebase_;

 private slots:
  void HashDeleted(const QString &path, const QString &filename);

  void ProjectInvalidated(Project *p);
};

class ThumbnailCache : public FrameHashCache {
  Q_OBJECT
 public:
  explicit ThumbnailCache(QObject *parent = nullptr) : FrameHashCache(parent) { SetTimebase(rational(1, 10)); }
};

}  // namespace olive

#endif  // VIDEORENDERFRAMECACHE_H
