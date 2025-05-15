#ifndef NODEINPUTIMMEDIATE_H // 防止头文件被多次包含的宏定义开始
#define NODEINPUTIMMEDIATE_H

#include "common/xmlutils.h" // 通用 XML 工具类 (在此头文件中似乎未直接使用，可能在 .cpp 中或间接依赖)
#include "node/keyframe.h"   // 引入 NodeKeyframe 和 NodeKeyframeTrack 定义
#include "node/value.h"      // 引入 NodeValue::Type 枚举定义
#include "splitvalue.h"      // 引入 SplitValue 定义，用于存储和处理多通道参数值

// 可能需要的前向声明
// class NodeInput; // NodeInput 在此被前向声明
// class QObject; // 假设
// class rational; // 假设
// class QVariant; // 假设

namespace olive { // Olive 编辑器的命名空间

class NodeInput; // 前向声明 NodeInput 类，NodeInputImmediate 是 NodeInput 的一个组成部分或实现细节

/**
 * @brief 管理节点输入参数的“立即值”和关键帧数据。
 * “立即值”是指当参数没有关键帧动画时，所使用的固定值。
 * 如果启用了关键帧，则此类的实例会存储和管理该参数的一系列关键帧 (NodeKeyframeTrack)。
 * 它还提供了在给定时间点获取参数值（通过插值关键帧或返回标准值）以及操作关键帧的方法。
 * `SplitValue` 用于处理多通道参数（如颜色RGBA、向量XYZW），允许独立访问或修改每个通道。
 */
class NodeInputImmediate {
 public:
  /**
   * @brief NodeInputImmediate 构造函数。
   * @param type 此输入参数的数据类型 (NodeValue::Type)。
   * @param default_val 此输入参数的默认值 (SplitValue 类型)。
   */
  NodeInputImmediate(NodeValue::Type type, SplitValue default_val);

  /**
   * @brief (内部函数) 插入一个关键帧。
   *  此函数会自动根据关键帧的时间进行插入排序，以保持关键帧列表按时间顺序排列。
   * @param key 指向要插入的 NodeKeyframe 对象的指针。
   */
  void insert_keyframe(NodeKeyframe* key);

  /**
   * @brief 移除一个关键帧。
   * @param key 指向要移除的 NodeKeyframe 对象的指针。
   */
  void remove_keyframe(NodeKeyframe* key);

  /**
   * @brief 删除此输入参数的所有关键帧。
   * @param parent (可选) QObject 父对象，用于管理被删除关键帧的内存（如果关键帧是 QObject 子类）。
   */
  void delete_all_keyframes(QObject* parent = nullptr);

  /**
   * @brief 获取非关键帧的标准值，该值已分解为多个通道/分量（这是其内部存储方式）。
   * @return const SplitValue& 对标准值 (SplitValue) 的常量引用。
   */
  [[nodiscard]] const SplitValue& get_split_standard_value() const { return standard_value_; }

  /**
   * @brief 获取非关键帧标准值在特定通道/轨道上的分量值。
   * @param track 通道/轨道的索引。
   * @return const QVariant& 对该通道标准值的常量引用。
   */
  [[nodiscard]] const QVariant& get_split_standard_value_on_track(int track) const { return standard_value_.at(track); }

  /**
   * @brief 设置非关键帧标准值在特定通道/轨道上的分量值。
   * @param value 新的通道值。
   * @param track 要设置的通道/轨道的索引。
   */
  void set_standard_value_on_track(const QVariant& value, int track);

  /**
   * @brief 设置非关键帧的标准值 (使用 SplitValue 对象)。
   * @param value 新的标准值。
   */
  void set_split_standard_value(const SplitValue& value);

  /**
   * @brief 获取在给定时间点，所有通道/轨道上的关键帧对象列表。
   *  如果此输入未启用关键帧或在该时间点没有关键帧，则列表可能为空。
   * @param time 要查询的时间点。
   * @return QVector<NodeKeyframe*> 包含 NodeKeyframe 指针的向量。
   */
  [[nodiscard]] QVector<NodeKeyframe*> get_keyframe_at_time(const rational& time) const;

  /**
   * @brief 获取在给定时间点、特定通道/轨道上的关键帧对象。
   * @param time 要查询的时间点。
   * @param track 要查询的通道/轨道的索引。
   * @return NodeKeyframe* 指向关键帧对象的指针；如果不存在或未启用关键帧，则为 nullptr。
   */
  [[nodiscard]] NodeKeyframe* get_keyframe_at_time_on_track(const rational& time, int track) const;

  /**
   * @brief 获取在特定通道/轨道上，距离给定时间点最近的关键帧。
   *  如果未启用关键帧或没有关键帧，则返回 nullptr。
   * @param time 参考时间点。
   * @param track 要查询的通道/轨道的索引。
   * @return NodeKeyframe* 指向最近的关键帧的指针。
   */
  [[nodiscard]] NodeKeyframe* get_closest_keyframe_to_time_on_track(const rational& time, int track) const;

  /**
   * @brief 获取在任何通道/轨道上，严格位于给定时间点之前的最近的关键帧。
   *  如果没有关键帧在此时间之前，则返回 nullptr。
   * @param time 参考时间点。
   * @return NodeKeyframe* 指向符合条件的关键帧的指针。
   */
  [[nodiscard]] NodeKeyframe* get_closest_keyframe_before_time(const rational& time) const;

  /**
   * @brief 获取在任何通道/轨道上，严格位于给定时间点之后的最近的关键帧。
   *  如果没有关键帧在此时间之后，则返回 nullptr。
   * @param time 参考时间点。
   * @return NodeKeyframe* 指向符合条件的关键帧的指针。
   */
  [[nodiscard]] NodeKeyframe* get_closest_keyframe_after_time(const rational& time) const;

  /**
   * @brief (启发式方法) 根据给定时间点（通常在两个现有关键帧之间）和特定通道，
   *  推断如果在此处插入新关键帧，其最合适的插值类型应该是什么。
   * @param time 计划插入新关键帧的时间点。
   * @param track 目标通道/轨道的索引。
   * @return NodeKeyframe::Type 推荐的关键帧插值类型。
   */
  [[nodiscard]] NodeKeyframe::Type get_best_keyframe_type_for_time(const rational& time, int track) const;

  /**
   * @brief 返回此输入参数的所有关键帧轨道 (NodeKeyframeTrack) 的列表。
   *  每个 NodeKeyframeTrack 对应参数的一个通道/分量，并包含该通道的所有关键帧。
   * @return const QVector<NodeKeyframeTrack>& 对关键帧轨道向量的常量引用。
   */
  [[nodiscard]] const QVector<NodeKeyframeTrack>& keyframe_tracks() const { return keyframe_tracks_; }

  /**
   * @brief 检查此输入参数当前是否启用了关键帧动画。
   * @return 如果启用了关键帧则返回 true，否则返回 false (此时使用 standard_value_)。
   */
  [[nodiscard]] bool is_keyframing() const { return keyframing_; }

  /**
   * @brief 设置此输入参数是否启用关键帧动画。
   * @param k true 表示启用关键帧，false 表示禁用。
   */
  void set_is_keyframing(bool k) { keyframing_ = k; }

  /**
   * @brief 获取在任何通道/轨道上时间最早的关键帧。
   * @return NodeKeyframe* 指向最早的关键帧的指针，如果没有关键帧则为 nullptr。
   */
  [[nodiscard]] NodeKeyframe* get_earliest_keyframe() const;

  /**
   * @brief 获取在任何通道/轨道上时间最晚的关键帧。
   * @return NodeKeyframe* 指向最晚的关键帧的指针，如果没有关键帧则为 nullptr。
   */
  [[nodiscard]] NodeKeyframe* get_latest_keyframe() const;

  /**
   * @brief 检查在给定的时间点是否存在任何关键帧（在任何通道/轨道上）。
   *  如果未启用关键帧，则此方法总是返回 false。
   * @param time 要检查的时间点。
   * @return 如果至少有一个通道在该时间点有关键帧，则返回 true，否则返回 false。
   */
  [[nodiscard]] bool has_keyframe_at_time(const rational& time) const;

  /**
   * @brief 检查特定通道/轨道当前是否正在使用其标准值（即未启用关键帧，或启用了但该通道无关键帧）。
   * @param track 要检查的通道/轨道的索引。
   * @return 如果正在使用标准值则返回 true，否则返回 false。
   */
  [[nodiscard]] bool is_using_standard_value(int track) const {
    return (!is_keyframing() || keyframe_tracks_.at(track).isEmpty());
  }

  /**
   * @brief 设置此输入参数的数据类型。
   *  这可能会影响 SplitValue 的内部结构或关键帧的处理方式。
   * @param type 新的数据类型 (NodeValue::Type)。
   */
  void set_data_type(NodeValue::Type type);

 private:
  /**
   * @brief 存储非关键帧动画时使用的标准值。
   *  使用 SplitValue 来处理多通道数据。
   */
  SplitValue standard_value_;

  /**
   * @brief 存储此输入参数的默认值。
   *  可能用于重置参数或在某些情况下作为回退值。
   */
  SplitValue default_value_;

  /**
   * @brief 存储所有关键帧轨道的内部数组。
   *  如果启用了关键帧 (keyframing_ 为 true)，则使用此数据代替 standard_value_ 来计算参数在某一时间的值。
   *  QVector<NodeKeyframeTrack> 的每个元素代表参数的一个通道/分量（例如 R, G, B, A 或 X, Y, Z）的关键帧序列。
   */
  QVector<NodeKeyframeTrack> keyframe_tracks_;

  /**
   * @brief 标记此输入参数当前是否启用了关键帧动画的内部设置。
   */
  bool keyframing_;
};

}  // namespace olive

#endif  // NODEINPUTIMMEDIATE_H // 头文件宏定义结束