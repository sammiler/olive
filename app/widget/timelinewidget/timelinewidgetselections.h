#ifndef TIMELINEWIDGETSELECTIONS_H // 防止头文件被多次包含的宏定义
#define TIMELINEWIDGETSELECTIONS_H

#include <QHash> // 引入 QHash 类，用于实现哈希表（一种键值对存储结构）

#include "node/output/track/track.h" // 引入 Track 类及其嵌套类型 (如 Track::Reference 和 TimeRangeList) 的定义

namespace olive { // olive 命名空间开始

/**
 * @brief TimelineWidgetSelections 类用于表示时间轴控件中的选择状态。
 *
 * 它继承自 QHash<Track::Reference, TimeRangeList>，意味着它将每个轨道的引用
 * (Track::Reference) 映射到一个在该轨道上被选中的时间范围列表 (TimeRangeList)。
 * 此类提供了对这些选择进行整体操作的方法，如时间平移、轨道平移和修剪。
 */
class TimelineWidgetSelections : public QHash<Track::Reference, TimeRangeList> {
 public:
  /**
   * @brief 默认构造函数。
   *
   * 创建一个空的 TimelineWidgetSelections 对象。
   */
  TimelineWidgetSelections() = default;

  /**
   * @brief 将所有选区中的时间范围整体平移指定的差值。
   * @param diff 时间平移的差值 (rational)。正值向后移动，负值向前移动。
   */
  void ShiftTime(const rational& diff);

  /**
   * @brief 将指定类型的轨道上的所有选区整体平移指定的轨道索引差值。
   * @param type 要移动选区的轨道类型 (Track::Type)。
   * @param diff 轨道索引的平移差值 (int)。正值向下移动轨道，负值向上移动轨道。
   */
  void ShiftTracks(Track::Type type, int diff);

  /**
   * @brief 修剪所有选区的入点。
   *
   * 每个选定时间范围的入点将根据 `diff` 进行调整。
   * @param diff 入点修剪的差值 (rational)。正值使入点向后（缩短范围），负值使入点向前（可能延长范围，取决于具体实现）。
   */
  void TrimIn(const rational& diff);

  /**
   * @brief 修剪所有选区的出点。
   *
   * 每个选定时间范围的出点将根据 `diff` 进行调整。
   * @param diff 出点修剪的差值 (rational)。正值使出点向后（可能延长范围，取决于具体实现），负值使出点向前（缩短范围）。
   */
  void TrimOut(const rational& diff);

  /**
   * @brief 从当前选区中减去另一组选区。
   *
   * 对于当前选区中的每个时间范围，如果它与 `selections` 参数中对应轨道的任何时间范围重叠，
   * 则会进行相应的调整或移除。
   * @param selections 要从中减去的另一组 TimelineWidgetSelections。
   */
  void Subtract(const TimelineWidgetSelections& selections);

  /**
   * @brief 返回一个新的 TimelineWidgetSelections 对象，该对象是当前选区减去另一组选区的结果。
   *
   * 这是一个 const 方法，不会修改当前对象。
   * @param selections 要从中减去的另一组 TimelineWidgetSelections。
   * @return 一个新的 TimelineWidgetSelections 对象，表示差集的结果。
   */
  [[nodiscard]] TimelineWidgetSelections Subtracted(const TimelineWidgetSelections& selections) const {
    TimelineWidgetSelections copy = *this; // 创建当前对象的副本
    copy.Subtract(selections);             // 在副本上执行减法操作
    return copy;                           // 返回修改后的副本
  }
};

}  // namespace olive

#endif  // TIMELINEWIDGETSELECTIONS_H
