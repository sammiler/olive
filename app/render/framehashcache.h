#ifndef VIDEORENDERFRAMECACHE_H // 防止头文件被重复包含的宏
#define VIDEORENDERFRAMECACHE_H // 定义 VIDEORENDERFRAMECACHE_H 宏

#include "codec/frame.h"           // 包含 Frame (或 FramePtr) 相关的定义
#include "render/playbackcache.h"  // 包含 PlaybackCache 基类的定义
#include "render/videoparams.h"    // 包含 VideoParams (视频参数) 相关的定义 (虽然未直接使用，但逻辑上相关)

// 假设 rational, Timecode, QUuid, QDataStream, QString 等类型
// 已通过上述 include 或其他方式被间接包含。
// Project 类定义也可能需要被包含，因为 ProjectInvalidated 槽函数使用了它。

namespace olive { // olive 项目的命名空间

class Project; // 向前声明 Project 类

/**
 * @brief FrameHashCache 类是一个用于缓存视频渲染帧的系统。
 *
 * 它继承自 PlaybackCache，专门为视频帧设计。与 AudioPlaybackCache 不同，视频帧通常是独立的图像，
 * 因此缓存策略可能更侧重于按帧（或时间戳）进行管理。
 * 这个类可能使用时间戳或基于时间的哈希作为文件名的一部分来存储和检索缓存的帧。
 * 它负责将渲染好的视频帧 (FramePtr) 保存到磁盘，并在需要时从磁盘加载它们，
 * 同时管理这些缓存文件的有效性。
 *
 * “Hash”在类名中可能指的是使用某种哈希算法来生成文件名或索引，或者仅仅是按时间戳/帧号来索引。
 */
class FrameHashCache : public PlaybackCache { // FrameHashCache 继承自 PlaybackCache
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param parent 父对象指针，默认为 nullptr。
   */
  explicit FrameHashCache(QObject *parent = nullptr);

  /**
   * @brief 获取此帧缓存使用的时间基准 (例如帧率)。
   * @return 返回 rational 类型的时间基准。
   */
  [[nodiscard]] const rational &GetTimebase() const { return timebase_; }

  /**
   * @brief 设置此帧缓存使用的时间基准。
   * @param tb 新的时间基准。
   */
  void SetTimebase(const rational &tb);

  /**
   * @brief 验证指定的时间戳是否在有效的缓存范围内，并可能将其标记为已访问或有效。
   * (具体行为取决于实现，可能用于更新内部的有效范围列表)
   * @param ts 要验证的时间戳 (通常是帧号或基于时间基准的整数时间单位)。
   */
  void ValidateTimestamp(const int64_t &ts);
  /**
   * @brief 验证指定的时间点是否在有效的缓存范围内。
   * @param time 要验证的时间点 (rational 类型)。
   */
  void ValidateTime(const rational &time);

  /**
   * @brief 检查指定时间点的帧是否已被缓存且有效。
   * @param time 要检查的时间点。
   * @return 如果帧已缓存且有效，则返回 true。
   */
  [[nodiscard]] bool IsFrameCached(const rational &time) const { return GetValidatedRanges().contains(time); }

  /**
   * @brief 获取指定时间点的有效缓存文件的完整路径名。
   * 如果该时间点的帧未缓存或无效，可能返回空字符串或特定错误指示。
   * @param time 要获取缓存文件名的时间点。
   * @return 返回缓存文件的路径名字符串。
   */
  [[nodiscard]] QString GetValidCacheFilename(const rational &time) const;

  // --- 静态和成员函数，用于保存和加载缓存帧 ---

  /**
   * @brief (静态) 将给定的视频帧保存到指定的文件名。
   * @param filename 要保存到的完整文件路径。
   * @param frame 指向要保存的帧数据的 FramePtr。
   * @return 如果保存成功，返回 true。
   */
  static bool SaveCacheFrame(const QString &filename, const FramePtr &frame);
  /**
   * @brief 将给定的视频帧保存到由此缓存实例管理的位置，使用时间戳作为标识。
   * @param time 帧对应的时间戳 (整数)。
   * @param frame 指向要保存的帧数据的 FramePtr。
   * @return 如果保存成功，返回 true。
   */
  [[nodiscard]] bool SaveCacheFrame(const int64_t &time, FramePtr frame) const;
  /**
   * @brief (静态) 将给定的视频帧保存到指定的缓存路径，使用 UUID 和时间戳作为标识。
   * @param cache_path 缓存的根路径。
   * @param uuid 缓存的唯一标识符 (通常与特定节点或项目关联)。
   * @param time 帧对应的时间戳 (整数)。
   * @param frame 指向要保存的帧数据的 FramePtr。
   * @return 如果保存成功，返回 true。
   */
  static bool SaveCacheFrame(const QString &cache_path, const QUuid &uuid, const int64_t &time, const FramePtr &frame);
  /**
   * @brief (静态) 将给定的视频帧保存到指定的缓存路径，使用 UUID、rational 时间和时间基准作为标识。
   */
  static bool SaveCacheFrame(const QString &cache_path, const QUuid &uuid, const rational &time, const rational &tb,
                             const FramePtr &frame);
  /**
   * @brief (静态) 从指定的缓存路径加载由 UUID 和时间戳标识的缓存帧。
   * @param cache_path 缓存的根路径。
   * @param uuid 缓存的唯一标识符。
   * @param time 帧对应的时间戳 (整数)。
   * @return 返回加载的 FramePtr，如果加载失败则可能为 nullptr。
   */
  static FramePtr LoadCacheFrame(const QString &cache_path, const QUuid &uuid, const int64_t &time);
  /**
   * @brief 从此缓存实例管理的位置加载由时间戳 (哈希) 标识的缓存帧。
   * @param hash 帧对应的时间戳或基于时间的哈希值。
   * @return 返回加载的 FramePtr。
   */
  [[nodiscard]] FramePtr LoadCacheFrame(const int64_t &hash) const;
  /**
   * @brief (静态) 从指定的完整文件名加载缓存帧。
   * @param fn 缓存文件的完整路径。
   * @return 返回加载的 FramePtr。
   */
  static FramePtr LoadCacheFrame(const QString &fn);

  /**
   * @brief (重写 PlaybackCache::SetPassthrough) 设置一个“透传”缓存。
   * 对于帧缓存，这可能意味着如果当前缓存未命中，会尝试从透传缓存中获取数据。
   * @param cache 指向要设置为透传的 PlaybackCache 对象的指针。
   */
  void SetPassthrough(PlaybackCache *cache) override;

 protected:
  /**
   * @brief (重写 PlaybackCache::LoadStateEvent) 从数据流加载缓存的状态信息。
   * 例如，加载已缓存帧的时间范围列表。
   * @param stream QDataStream 对象，用于读取数据。
   */
  void LoadStateEvent(QDataStream &stream) override;
  /**
   * @brief (重写 PlaybackCache::SaveStateEvent) 将缓存的状态信息保存到数据流。
   * @param stream QDataStream 对象，用于写入数据。
   */
  void SaveStateEvent(QDataStream &stream) override;

 private:
  /**
   * @brief 将整数时间戳转换为 rational 类型的时间 (基于当前 timebase_)。
   */
  [[nodiscard]] rational ToTime(const int64_t &ts) const;
  /**
   * @brief 将 rational 类型的时间转换为整数时间戳 (基于当前 timebase_)。
   * @param rounding 时间转换时的取整方式。
   */
  [[nodiscard]] int64_t ToTimestamp(const rational &ts, Timecode::Rounding rounding = Timecode::kRound) const;

  /**
   * @brief 返回指定时间戳的缓存图像文件的路径名。
   * (这是此缓存实例内部使用的路径生成逻辑)
   */
  [[nodiscard]] QString CachePathName(const int64_t &time) const;
  /**
   * @brief 返回指定 rational 时间的缓存图像文件的路径名。
   */
  [[nodiscard]] QString CachePathName(const rational &time) const;

  /**
   * @brief (静态) 根据缓存根路径、UUID和时间戳生成缓存文件的路径名。
   */
  static QString CachePathName(const QString &cache_path, const QUuid &cache_id, const int64_t &time);
  /**
   * @brief (静态) 根据缓存根路径、UUID、rational 时间和时间基准生成缓存文件的路径名。
   */
  static QString CachePathName(const QString &cache_path, const QUuid &cache_id, const rational &time,
                               const rational &tb);

  rational timebase_; // 此帧缓存使用的时间基准 (例如，25/1 表示25fps)

 private slots: // Qt 私有槽函数
  /**
   * @brief 当磁盘管理器删除了一个缓存文件时调用的槽函数。
   * FrameHashCache 需要更新其内部状态以反映此删除。
   * @param path 被删除文件所在的缓存文件夹路径。
   * @param filename 被删除的文件名。
   */
  void HashDeleted(const QString &path, const QString &filename);

  /**
   * @brief 当一个项目的数据可能失效时调用的槽函数。
   * 如果此缓存与该项目相关，可能需要清除或重新验证其内容。
   * @param p 相关的 Project 指针。
   */
  void ProjectInvalidated(Project *p);
};

/**
 * @brief ThumbnailCache 类专门用于缓存缩略图。
 *
 * 它继承自 FrameHashCache，重用了大部分帧缓存的逻辑。
 * 主要区别在于它通常使用一个较低的时间基准 (例如，每秒10帧或更少)，
 * 因为缩略图不需要像全帧率回放那样密集。
 * 构造函数中默认设置了一个较低的时间基准 (1/10)。
 */
class ThumbnailCache : public FrameHashCache { // ThumbnailCache 继承自 FrameHashCache
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param parent 父对象指针，默认为 nullptr。
   * 构造函数中将时间基准默认设置为 10fps (rational(1, 10))。
   */
  explicit ThumbnailCache(QObject *parent = nullptr) : FrameHashCache(parent) { SetTimebase(rational(1, 10)); }
};

}  // namespace olive

#endif  // VIDEORENDERFRAMECACHE_H