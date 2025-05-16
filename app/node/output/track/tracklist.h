#ifndef TRACKLIST_H  // 防止头文件被多次包含的宏定义开始
#define TRACKLIST_H

#include <QObject>  // Qt 对象模型基类

#include "node/output/track/track.h"  // 引入 Track 类的定义，TrackList 是 Track 的容器
#include "timeline/timelinecommon.h"  // 引入时间线相关的通用定义，可能包含 rational 等

// 可能需要的前向声明
// class Project; // 假设
// class NodeInput; // 假设
// class Node; // 假设

namespace olive {  // Olive 编辑器的命名空间

class Sequence;  // 前向声明 Sequence 类，TrackList 通常属于某个序列

/**
 * @brief 代表一个特定类型轨道的列表（集合）。
 * 例如，一个序列中所有的视频轨道会由一个 TrackList<Track::kVideo> 管理，
 * 所有的音频轨道由另一个 TrackList<Track::kAudio> 管理。
 * 此类负责管理这些轨道的添加、移除、索引以及获取列表的总长度等。
 */
class TrackList : public QObject {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief TrackList 构造函数。
      * @param parent 指向其所属的 Sequence 对象的指针。
      * @param type 此轨道列表管理的轨道类型 (例如 Track::kVideo)。
      * @param track_input 关联到序列节点上代表此类轨道的输入端口名称（例如 "VideoTracks"）。
      */
     TrackList(Sequence* parent, const Track::Type& type, QString track_input);

  /**
   * @brief 获取此轨道列表管理的轨道类型。
   * @return const Track::Type& 对轨道类型的常量引用。
   */
  [[nodiscard]] const Track::Type& type() const { return type_; }

  /**
   * @brief 获取此列表中所有轨道的指针向量。
   * @return const QVector<Track*>& 对轨道指针向量的常量引用。
   */
  [[nodiscard]] const QVector<Track*>& GetTracks() const { return track_cache_; }

  /**
   * @brief 根据索引获取列表中的特定轨道。
   * @param index 轨道的索引 (在 track_cache_ 中的位置)。
   * @return Track* 指向轨道的指针，如果索引无效则为 nullptr。
   */
  [[nodiscard]] Track* GetTrackAt(int index) const;

  /**
   * @brief 获取此轨道列表中所有轨道的总长度（通常是最长轨道的长度）。
   * @return const rational& 对总长度的常量引用。
   */
  [[nodiscard]] const rational& GetTotalLength() const { return total_length_; }

  /**
   * @brief 获取此列表中轨道的数量。
   * @return int 轨道的数量。
   */
  [[nodiscard]] int GetTrackCount() const { return track_cache_.size(); }

  /**
   * @brief 获取此轨道列表所属的项目 (Project) 对象。
   *  通过其父级 Sequence 对象获取。
   * @return Project* 指向项目对象的指针。
   */
  [[nodiscard]] Project* GetParentGraph() const;

  /**
   * @brief 获取关联到序列节点上代表此类轨道的输入端口名称。
   * @return const QString& 输入端口名称的常量引用。
   */
  [[nodiscard]] const QString& track_input() const;
  /**
   * @brief 根据元素索引获取一个具体的轨道输入 NodeInput 对象。
   * @param element 元素索引（对应于序列节点上轨道输入数组的某个特定轨道）。
   * @return NodeInput 对应的 NodeInput 对象。
   */
  [[nodiscard]] NodeInput track_input(int element) const;

  /**
   * @brief 获取此轨道列表的父级 Sequence 对象。
   * @return Sequence* 指向父级 Sequence 对象的指针。
   */
  [[nodiscard]] Sequence* parent() const;

  /**
   * @brief 获取序列节点上用于连接此类轨道的输入数组的大小。
   * @return int 数组大小。
   */
  [[nodiscard]] int ArraySize() const;

  /**
   * @brief 在序列节点的轨道输入数组末尾追加一个新的连接点（用于连接新轨道）。
   */
  void ArrayAppend() const;
  /**
   * @brief 从序列节点的轨道输入数组末尾移除最后一个连接点。
   */
  void ArrayRemoveLast() const;

  /**
   * @brief 根据缓存索引（track_cache_ 中的索引）获取其在序列节点输入数组中的实际索引。
   * @param index 缓存索引。
   * @return int 数组索引。
   */
  [[nodiscard]] int GetArrayIndexFromCacheIndex(int index) const { return track_array_indexes_.at(index); }

  /**
   * @brief 根据序列节点输入数组中的索引获取其在缓存（track_cache_）中的索引。
   * @param index 数组索引。
   * @return int 缓存索引，如果未找到则为 -1。
   */
  [[nodiscard]] int GetCacheIndexFromArrayIndex(int index) const { return track_array_indexes_.indexOf(index); }

 public slots:
  /**
   * @brief 当一个轨道连接到序列节点的对应输入端口时调用的槽函数。
   *  用于将连接的轨道添加到此 TrackList 中。
   * @param node 被连接的节点 (期望是 Track 类型)。
   * @param element 连接到的输入端口的元素索引。
   */
  void TrackConnected(Node* node, int element);

  /**
   * @brief 当一个轨道从序列节点的对应输入端口断开连接时调用的槽函数。
   *  用于从此 TrackList 中移除该轨道。
   * @param node 被断开连接的节点 (期望是 Track 类型)。
   * @param element 断开连接的输入端口的元素索引。
   */
  void TrackDisconnected(Node* node, int element);

 signals:  // Qt 信号声明区域
  /**
   * @brief 当轨道列表内容发生改变（例如添加、移除轨道，或轨道顺序改变）时发射此信号。
   */
  void TrackListChanged();

  /**
   * @brief 当轨道列表的总长度发生改变时发射此信号。
   * @param length 新的总长度。
   */
  void LengthChanged(const rational& length);

  /**
   * @brief 当有新轨道添加到此列表时发射此信号。
   * @param track 被添加的轨道。
   */
  void TrackAdded(Track* track);

  /**
   * @brief 当有轨道从此列表移除时发射此信号。
   * @param track 被移除的轨道。
   */
  void TrackRemoved(Track* track);

  /**
   * @brief 当列表中某个轨道的高度发生改变时发射此信号。
   * @param track 高度发生改变的轨道。
   * @param height 新的高度（像素值）。
   */
  void TrackHeightChanged(Track* track, int height);

 private:
  /**
   * @brief 从指定的索引开始，更新后续所有轨道的内部索引。
   * @param index 开始更新的轨道的缓存索引。
   */
  void UpdateTrackIndexesFrom(int index);

  /**
   * @brief 存储已连接到此列表的轨道对象的指针的缓存。
   *  顺序可能与它们在序列节点输入数组中的顺序不同。
   */
  QVector<Track*> track_cache_;
  /**
   * @brief 存储 track_cache_ 中每个轨道对应其在父序列节点输入数组中的索引。
   *  track_array_indexes_[i] 是 track_cache_[i] 在序列输入数组中的索引。
   */
  QVector<int> track_array_indexes_;

  QString track_input_;  ///< 关联到序列节点上代表此类轨道的输入端口名称。

  rational total_length_;  ///< 此轨道列表中所有轨道的总长度（通常是最长轨道的长度）。

  enum Track::Type type_;  ///< 此轨道列表管理的轨道类型。

 private slots:
  /**
   * @brief 当列表中任何一个轨道的长度发生变化时调用的槽函数，用于更新轨道列表的总长度。
   */
  void UpdateTotalLength();
};

}  // namespace olive

#endif  // TRACKLIST_H // 头文件宏定义结束