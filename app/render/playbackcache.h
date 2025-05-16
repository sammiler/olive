#ifndef PLAYBACKCACHE_H  // 防止头文件被重复包含的宏
#define PLAYBACKCACHE_H  // 定义 PLAYBACKCACHE_H 宏

#include <olive/core/core.h>  // 包含 Olive 核心定义 (可能包含 TimeRange, rational, TimeRangeList 等)
#include <QDir>               // Qt 目录操作类
#include <QMutex>             // Qt 互斥锁类
#include <QObject>            // Qt 对象模型基类
#include <QPainter>           // Qt 绘图类 (用于绘制缓存状态指示)
#include <QUuid>              // Qt 通用唯一标识符类

#include "common/jobtime.h"  // 可能包含与时间或任务相关的定义 (TimeRangeList 如果未在core中定义)

using namespace olive::core;  // 使用 olive::core 命名空间中的类型 (如 rational, TimeRange)

// 假设 Node, Project, ViewerOutput, QDataStream, QFont, QFontMetrics, QRect, std::vector
// 已通过其他方式被间接包含。

namespace olive {  // olive 项目的命名空间

class Node;          // 向前声明 Node 类
class Project;       // 向前声明 Project 类
class ViewerOutput;  // 向前声明 ViewerOutput 类 (通常代表可输出帧的节点)

/**
 * @brief PlaybackCache 类是所有用于播放时缓存数据的基类。
 *
 * 它提供了管理缓存数据有效性 (validated/invalidated ranges)、
 * 缓存文件存储位置、以及与其他缓存实例进行“透传”(passthrough)交互的通用机制。
 * 派生类 (如 AudioPlaybackCache, FrameHashCache) 会针对特定类型的数据 (音频、视频帧)
 * 实现具体的缓存逻辑。
 *
 * 主要功能包括：
 * - 跟踪已验证 (已缓存且有效) 和已失效的时间范围。
 * - 管理缓存的唯一标识符 (UUID) 和磁盘存储目录。
 * - 提供使缓存范围失效 (Invalidate) 和验证缓存范围 (Validate) 的接口。
 * - 支持“透传”模式，即如果当前缓存没有数据，可以尝试从另一个缓存获取。
 * - 发出信号通知缓存状态的改变或数据请求。
 */
class PlaybackCache : public QObject {  // PlaybackCache 继承自 QObject
 Q_OBJECT                               // 声明此类使用 Qt 的元对象系统

     public :
     /**
      * @brief 构造函数。
      * @param parent 父对象指针，默认为 nullptr。
      */
     explicit PlaybackCache(QObject *parent = nullptr);

  /**
   * @brief 获取此缓存实例的唯一标识符 (UUID)。
   * @return 返回 QUuid 的常量引用。
   */
  [[nodiscard]] const QUuid &GetUuid() const { return uuid_; }
  /**
   * @brief 设置此缓存实例的唯一标识符。
   * @param u 新的 QUuid。
   */
  void SetUuid(const QUuid &u);

  /**
   * @brief 获取与给定时间范围相交的已失效的时间范围列表。
   * @param intersecting 要检查相交的时间范围。
   * @return 返回一个 TimeRangeList，包含所有与 `intersecting` 相交的失效范围。
   */
  [[nodiscard]] TimeRangeList GetInvalidatedRanges(TimeRange intersecting) const;
  /**
   * @brief 获取从时间点0到指定长度范围内的已失效的时间范围列表。
   */
  [[nodiscard]] TimeRangeList GetInvalidatedRanges(const rational &length) const {
    return GetInvalidatedRanges(TimeRange(rational(0), length));
  }

  /**
   * @brief 检查是否存在与给定时间范围相交的已失效的时间范围。
   * @param intersecting 要检查相交的时间范围。
   * @return 如果存在相交的失效范围，则返回 true。
   */
  [[nodiscard]] bool HasInvalidatedRanges(const TimeRange &intersecting) const;
  /**
   * @brief 检查从时间点0到指定长度范围内是否存在已失效的时间范围。
   */
  [[nodiscard]] bool HasInvalidatedRanges(const rational &length) const {
    return HasInvalidatedRanges(TimeRange(rational(0), length));
  }

  /**
   * @brief 获取此缓存数据存储的根目录路径。
   * 实际文件可能存储在此目录下的子目录中 (由 UUID 区分)。
   * @return 返回缓存目录的 QString。
   */
  [[nodiscard]] QString GetCacheDirectory() const;

  /**
   * @brief 使指定时间范围内的缓存数据失效。
   * 这通常在源数据发生改变时调用，以确保下次访问时重新生成缓存。
   * @param r 要失效的时间范围。
   */
  void Invalidate(const TimeRange &r);

  /**
   * @brief 检查是否存在任何已验证 (有效) 的缓存范围。
   * @return 如果存在有效的缓存范围，则返回 true。
   */
  [[nodiscard]] bool HasValidatedRanges() const { return !validated_.isEmpty(); }
  /**
   * @brief 获取所有已验证 (有效) 的缓存时间范围列表。
   * @return 返回 TimeRangeList 的常量引用。
   */
  [[nodiscard]] const TimeRangeList &GetValidatedRanges() const { return validated_; }

  /**
   * @brief 获取此缓存所属的父节点 (Node)。
   * 缓存通常与某个特定节点关联。
   * @return 返回 Node 指针。
   */
  [[nodiscard]] Node *parent() const;

  /**
   * @brief 获取此特定缓存实例在磁盘上的存储目录 (通常是 cache_path/uuid)。
   * @return 返回 QDir 对象。
   */
  [[nodiscard]] QDir GetThisCacheDirectory() const;
  /**
   * @brief (静态) 根据缓存根路径和缓存ID获取特定缓存实例的磁盘存储目录。
   */
  static QDir GetThisCacheDirectory(const QString &cache_path, const QUuid &cache_id);

  // 加载缓存的状态 (例如，从磁盘文件恢复已验证范围列表)
  void LoadState();
  // 保存缓存的当前状态
  void SaveState();

  /**
   * @brief (虚函数，可能由派生类实现) 在给定的 QPainter 上绘制缓存状态的指示器。
   * 例如，在时间轴上用不同颜色标记已缓存和未缓存的区域。
   * @param painter 用于绘制的 QPainter 对象指针。
   * @param start 绘制区域的起始时间。
   * @param scale 时间轴的缩放比例。
   * @param rect 绘制的目标矩形区域。
   */
  void Draw(QPainter *painter, const rational &start, double scale, const QRect &rect) const;

  // (静态) 获取缓存指示器的高度 (通常基于字体大小)
  static int GetCacheIndicatorHeight() { return QFontMetrics(QFont()).height() / 4; }

  /**
   * @brief 检查是否启用了将缓存状态保存到磁盘。
   * @return 如果启用保存，则返回 true。
   */
  [[nodiscard]] bool IsSavingEnabled() const { return saving_enabled_; }
  /**
   * @brief 设置是否启用将缓存状态保存到磁盘。
   * @param e 如果为 true 则启用。
   */
  void SetSavingEnabled(bool e) { saving_enabled_ = e; }

  /**
   * @brief (虚函数) 设置一个“透传”缓存。
   * 当此缓存未命中时，可以尝试从透传缓存中获取数据。
   * @param cache 指向要设置为透传的 PlaybackCache 对象的指针。
   */
  virtual void SetPassthrough(PlaybackCache *cache);

  /**
   * @brief 获取用于线程同步的互斥锁。
   * @return 返回 QMutex 指针。
   */
  QMutex *mutex() { return &mutex_; }

  // 内部类，用于表示一个透传缓存及其有效的时间范围
  class Passthrough : public TimeRange {  // 继承自 TimeRange
   public:
    // 构造函数，初始化时间范围
    explicit Passthrough(const TimeRange &r) : TimeRange(r) {}

    QUuid cache;  // 透传缓存的 UUID
  };

  /**
   * @brief 获取当前配置的透传缓存列表。
   * @return 返回包含 Passthrough 对象的 std::vector 的常量引用。
   */
  [[nodiscard]] const std::vector<Passthrough> &GetPassthroughs() const { return passthroughs_; }

  /**
   * @brief 从数据请求列表中移除指定的时间范围。
   * 当一个请求被满足或取消时调用。
   * @param r 要移除的时间范围。
   */
  void ClearRequestRange(const TimeRange &r) { requested_.remove(r); }

  /**
   * @brief 重新发出所有当前待处理的数据请求信号。
   * 可能在某些状态改变后需要重新触发请求逻辑。
   */
  void ResignalRequests() {
    for (const TimeRange &r : requested_) {  // Qt 的 foreach 风格循环
      emit Requested(request_context_, r);
    }
  }

 public slots:  // Qt 公有槽函数
  /**
   * @brief 使此缓存中的所有数据失效。
   */
  void InvalidateAll();

  /**
   * @brief 请求缓存指定时间范围的数据。
   * 这个请求通常由需要显示或播放数据的组件 (如查看器) 发出。
   * @param context 发出请求的 ViewerOutput 上下文 (例如，哪个序列或素材需要数据)。
   * @param r 请求的时间范围。
   */
  void Request(ViewerOutput *context, const TimeRange &r);

 signals:  // Qt 信号声明
  /**
   * @brief 当缓存中的某个时间范围变为无效时发出的信号。
   * @param r 失效的时间范围。
   */
  void Invalidated(const TimeRange &r);

  /**
   * @brief 当缓存中的某个时间范围被验证为有效 (数据已缓存) 时发出的信号。
   * @param r 被验证的时间范围。
   */
  void Validated(const TimeRange &r);

  /**
   * @brief 当外部请求缓存某个时间范围的数据时发出的信号。
   * 渲染系统或缓存填充逻辑会监听此信号以生成所需数据。
   * @param context 请求数据的 ViewerOutput 上下文。
   * @param r 请求的时间范围。
   */
  void Requested(ViewerOutput *context, const TimeRange &r);

  /**
   * @brief 请求取消所有与此缓存相关的正在进行的后台操作 (例如缓存生成)。
   */
  void CancelAll();

 protected:
  /**
   * @brief 将指定时间范围标记为已验证 (有效)。
   * @param r 要验证的时间范围。
   * @param signal (可选) 是否在验证后发出 Validated 信号，默认为 true。
   */
  void Validate(const TimeRange &r, bool signal = true);

  /**
   * @brief (虚函数) 当缓存的某个时间范围失效时调用的内部事件处理函数。
   * 派生类可以重写此方法以执行特定于其数据类型的失效逻辑。
   * @param range 失效的时间范围。
   */
  virtual void InvalidateEvent(const TimeRange &range);

  /**
   * @brief (虚函数) 从数据流加载特定于派生类的状态信息。
   * 默认实现为空。
   * @param stream QDataStream 对象。
   */
  virtual void LoadStateEvent(QDataStream &stream) {}

  /**
   * @brief (虚函数) 将特定于派生类的状态信息保存到数据流。
   * 默认实现为空。
   * @param stream QDataStream 对象。
   */
  virtual void SaveStateEvent(QDataStream &stream) {}

  /**
   * @brief 获取此缓存所属的项目。
   * 通过父节点 (Node) 向上查找其所属的 Project。
   * @return 返回 Project 指针。
   */
  [[nodiscard]] Project *GetProject() const;

 private:
  TimeRangeList validated_;  // 存储所有已验证 (有效) 的缓存时间范围的列表

  TimeRangeList requested_;          // 存储当前已请求但尚未完全缓存的时间范围列表
  ViewerOutput *request_context_{};  // 存储最近一次数据请求的上下文 (哪个 ViewerOutput 请求的)

  QUuid uuid_;  // 此缓存实例的唯一标识符

  bool saving_enabled_;  // 标记是否启用将缓存状态持久化保存

  QMutex mutex_;  // 用于保护对内部数据结构 (如 validated_, requested_) 的线程安全访问

  std::vector<Passthrough> passthroughs_;  // 存储配置的透传缓存信息

  qint64 last_loaded_state_;  // 上次加载状态的时间戳或标记 (可能用于避免不必要的重复加载)
};

}  // namespace olive

#endif  // PLAYBACKCACHE_H