#ifndef SEQUENCE_H // 防止头文件被多次包含的宏定义开始
#define SEQUENCE_H

#include "node/output/track/tracklist.h" // 引入 TrackList 定义，序列管理多个轨道列表
#include "node/output/viewer/viewer.h"   // 引入基类 ViewerOutput 的定义

// 可能需要的前向声明
// class MultiUndoCommand; // 假设
// class QVariant; // 假设
// class TimeRange; // 假设
// struct InvalidateCacheOptions; // 假设
// class rational; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表主要的时间线对象，是一个由已编辑剪辑（片段）组成的图，构成一个完整的编辑成果。
 * Sequence 继承自 ViewerOutput，意味着它可以被视为一个可供查看和渲染的最终输出。
 * 它内部管理着多个轨道列表（如视频轨道列表、音频轨道列表等）。
 */
class Sequence : public ViewerOutput {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief Sequence 构造函数。
   *  通常会在这里初始化轨道列表等。
   */
  Sequence();

  NODE_DEFAULT_FUNCTIONS(Sequence) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /** @brief 获取此序列节点的名称 (例如 "序列")，支持国际化。 */
  [[nodiscard]] QString Name() const override { return tr("Sequence"); }
  /** @brief 获取此序列节点的唯一标识符。 */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.sequence"); }
  /** @brief 获取此节点所属的分类 ID 列表 (通常是 "项目")。 */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryProject}; }
  /** @brief 获取此序列节点的描述信息 (例如 "一系列剪辑操作最终形成的编辑视频。也称为时间线。")，支持国际化。 */
  [[nodiscard]] QString Description() const override {
    return tr("A series of cuts that result in an edited video. Also called a timeline.");
  }

  /**
   * @brief (可选地) 为此序列添加默认的节点或轨道。
   * @param command (可选) 指向 MultiUndoCommand 的指针，用于将此操作添加到撤销/重做栈。
   */
  void add_default_nodes(MultiUndoCommand *command = nullptr) const;

  /**
   * @brief 获取特定类型的数据（例如，序列特定的元数据或配置）。
   * @param d 要获取的数据类型标识。
   * @return QVariant 包含所请求数据的 QVariant 对象。
   */
  [[nodiscard]] QVariant data(const DataType &d) const override;

  /**
   * @brief 获取此序列中所有轨道的一个扁平化列表（缓存）。
   * @return const QVector<Track *>& 对轨道指针向量的常量引用。
   */
  [[nodiscard]] const QVector<Track *> &GetTracks() const { return track_cache_; }

  /**
   * @brief 根据轨道引用 (Track::Reference) 获取具体的轨道对象。
   * @param track_ref 轨道的引用 (包含类型和在该类型中的索引)。
   * @return Track* 指向轨道对象的指针，如果引用无效则行为未定义或返回 nullptr。
   */
  [[nodiscard]] Track *GetTrackFromReference(const Track::Reference &track_ref) const {
    return track_lists_.at(track_ref.type())->GetTrackAt(track_ref.index());
  }

  /**
   * @brief 与 GetTracks() 类似，但会排除（过滤掉）所有被锁定的轨道。
   * @return QVector<Track *> 包含所有未锁定轨道的指针向量。
   */
  [[nodiscard]] QVector<Track *> GetUnlockedTracks() const;

  /**
   * @brief 根据轨道类型获取对应的轨道列表 (TrackList) 对象。
   * @param type 轨道类型 (例如 Track::kVideo, Track::kAudio)。
   * @return TrackList* 指向对应类型轨道列表的指针。
   */
  [[nodiscard]] TrackList *track_list(Track::Type type) const { return track_lists_.at(type); }

  /** @brief 当界面语言等需要重新翻译时调用。 */
  void Retranslate() override;

  /**
   * @brief 使指定时间范围内的缓存无效。
   *  会传递给其内部的轨道和媒体块。
   * @param range 需要作废的时间范围。
   * @param from 触发作废的源头标识。
   * @param element 相关的元素索引。
   * @param options 作废缓存的选项。
   */
  void InvalidateCache(const TimeRange &range, const QString &from, int element,
                       InvalidateCacheOptions options) override;

  // 用于格式化轨道输入端口名称的字符串，例如 "VideoTracks%1", "AudioTracks%1"
  // %1 会被轨道的元素索引替换。
  static const QString kTrackInputFormat; ///< 例如 "VideoTracks%1", "AudioTracks%1" 等格式化字符串。

 protected:
  /**
   * @brief 当序列的某个输入端口（通常是连接轨道的端口）连接上一个输出节点（即轨道）时调用的事件处理函数。
   * @param input 连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param output 连接到的输出节点 (即轨道)。
   */
  void InputConnectedEvent(const QString &input, int element, Node *output) override;

  /**
   * @brief 当序列的某个输入端口（轨道连接）断开时调用的事件处理函数。
   * @param input 断开连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param output 之前连接的输出节点 (即轨道)。
   */
  void InputDisconnectedEvent(const QString &input, int element, Node *output) override;

  /**
   * @brief (虚保护函数) 验证并返回特定类型流（轨道）的内部计算长度。
   *  对于序列，这通常是其对应类型轨道列表的总长度。
   * @param type 流类型 (视频、音频等)。
   * @return rational 计算得到的长度。
   */
  [[nodiscard]] rational VerifyLengthInternal(Track::Type type) const override;

 signals: // Qt 信号声明区域
  /**
   * @brief 当有新轨道添加到此序列时发射此信号。
   *  注意：这可能是通过 TrackList 发射的信号中继而来。
   * @param track 被添加的轨道。
   */
  void TrackAdded(Track *track);
  /**
   * @brief 当有轨道从此序列移除时发射此信号。
   *  注意：这可能是通过 TrackList 发射的信号中继而来。
   * @param track 被移除的轨道。
   */
  void TrackRemoved(Track *track);

  /**
   * @brief 当字幕内容在指定时间范围可能发生变化时发射此信号。
   *  用于通知相关组件（如查看器）刷新字幕显示。
   * @param range 字幕内容发生变化的时间范围。
   */
  void SubtitlesChanged(const TimeRange &range);

 private:
  // 存储不同类型轨道列表的向量。
  // 向量的索引通常对应 Track::Type 枚举值 (例如 track_lists_[Track::kVideo] 是视频轨道列表)。
  QVector<TrackList *> track_lists_; ///< 管理不同类型 (视频、音频、字幕) 轨道的列表。

  // 所有轨道的一个扁平化列表的缓存，方便快速访问所有轨道而不区分类型。
  // 这个缓存通常在轨道列表发生变化时通过 UpdateTrackCache() 更新。
  QVector<Track *> track_cache_; ///< 所有轨道的扁平化列表缓存。

 private slots:
  /**
   * @brief 当任何一个 TrackList 的内容发生改变时调用的槽函数，用于更新 track_cache_。
   */
  void UpdateTrackCache();
};

}  // namespace olive

#endif  // SEQUENCE_H // 头文件宏定义结束