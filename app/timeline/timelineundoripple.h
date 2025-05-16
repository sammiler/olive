#ifndef TIMELINEUNDORIPPLE_H
#define TIMELINEUNDORIPPLE_H

#include <utility> // 引入 C++ 标准库工具，例如 std::move

#include "node/block/gap/gap.h"                // 引入 GapBlock 类的定义，用于处理空白区块
#include "node/output/track/track.h"           // 引入 Track 类的定义
#include "node/output/track/tracklist.h"       // 引入 TrackList 类的定义
#include "node/project/sequence/sequence.h"    // 引入 Sequence 类的定义
#include "timelineundogeneral.h"               // 引入通用的时间轴撤销命令定义
#include "timelineundosplit.h"                 // 引入时间轴分割相关的撤销命令定义
#include "timelineundotrack.h"                 // 引入轨道操作相关的撤销命令定义 (虽然在此文件中未直接使用，但可能是上下文依赖)

namespace olive {

/**
 * @brief 清除轨道上指定时间范围（in 到 out）内区域的命令。
 *
 * 此命令保证 `in` 和 `out` 之间的时间区域被清空。此区域内的区块将被裁切或移除以释放空间。
 * 默认情况下，此区域不会被任何内容填充，意味着所有后续的片段都会向前（左）推移。
 * 但是，您可以指定一个区块在 `in` 点插入。此命令不检查 `insert` 区块的长度是否与
 * `in` 到 `out` 的范围长度相同。
 */
class TrackRippleRemoveAreaCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param track 目标轨道。
   * @param range 要移除的时间范围。
   */
  TrackRippleRemoveAreaCommand(Track* track, const TimeRange& range);

  /**
   * @brief 析构函数。
   *
   * 负责清理 splice_split_command_ 和 remove_block_commands_ 中的命令。
   */
  ~TrackRippleRemoveAreaCommand() override;

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回轨道所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

  /**
   * @brief 获取在此涟漪操作中，如果要插入内容，应在其后插入的区块。
   * @return 返回指向插入点前一个区块的指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Block* GetInsertionIndex() const { return insert_previous_; }

  /**
   * @brief 获取因分割操作而产生的新区块（如果发生了分割）。
   * @return 如果执行了分割操作，返回新创建的 Block 指针；否则返回 nullptr。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Block* GetSplicedBlock() const {
    if (splice_split_command_) {
      return splice_split_command_->new_block();
    }
    return nullptr;
  }

  /**
   * @brief 设置是否允许分割空白区块。
   * @param e 如果为 true，则允许分割空白区块。
   */
  void SetAllowSplittingGaps(bool e) { allow_splitting_gaps_ = e; }

 protected:
  /**
   * @brief 命令准备阶段，在首次 redo() 或 undo() 前调用。
   */
  void prepare() override;
  /**
   * @brief 执行涟漪移除区域的操作。
   */
  void redo() override;
  /**
   * @brief 撤销涟漪移除区域的操作。
   */
  void undo() override;

 private:
  /**
   * @brief 描述一个裁切操作的内部结构体。
   */
  struct TrimOperation {
    Block* block{};        ///< 指向被裁切的区块。
    rational old_length;   ///< 区块的原始长度。
    rational new_length;   ///< 区块的新长度。
  };

  /**
   * @brief 描述一个移除操作的内部结构体。
   */
  struct RemoveOperation {
    Block* block;  ///< 指向被移除的区块。
    Block* before; ///< 被移除区块之前的一个区块，用于恢复时的定位。
  };

  Track* track_;          // 目标轨道
  TimeRange range_;       // 要移除的时间范围

  TrimOperation trim_out_; // 对范围右侧区块的裁切操作信息
  QVector<RemoveOperation> removals_; // 在范围内被完整移除的区块列表
  TrimOperation trim_in_;  // 对范围左侧区块的裁切操作信息
  Block* insert_previous_{}; // 标记插入点的前一个区块
  bool allow_splitting_gaps_; // 是否允许分割空白区块

  BlockSplitCommand* splice_split_command_; // 如果需要分割区块，则存储分割命令
  QVector<UndoCommand*> remove_block_commands_; // 存储移除区块的子命令
};

/**
 * @brief 在轨道列表（TrackList）的所有适用轨道上执行涟漪移除区域操作的命令。
 *
 * 此命令通常会为每个受影响的轨道创建一个 TrackRippleRemoveAreaCommand 实例。
 */
class TrackListRippleRemoveAreaCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param list 目标轨道列表。
   * @param in 涟漪移除区域的入点。
   * @param out 涟漪移除区域的出点。
   */
  TrackListRippleRemoveAreaCommand(TrackList* list, rational in, rational out) : list_(list), range_(in, out) {}

  /**
   * @brief 析构函数。
   *
   * 负责清理 commands_ 向量中的所有命令对象。
   */
  ~TrackListRippleRemoveAreaCommand() override { qDeleteAll(commands_); }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回轨道列表父级（通常是 Sequence）所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return list_->parent()->project(); }

 protected:
  /**
   * @brief 命令准备阶段。
   */
  void prepare() override;
  /**
   * @brief 执行在轨道列表上涟漪移除区域的操作。
   */
  void redo() override;
  /**
   * @brief 撤销在轨道列表上涟漪移除区域的操作。
   */
  void undo() override;

 private:
  TrackList* list_; // 目标轨道列表

  QList<Track*> working_tracks_; // 实际执行操作的轨道列表

  TimeRange range_; // 要移除的时间范围

  QVector<TrackRippleRemoveAreaCommand*> commands_; // 为每个受影响轨道创建的子命令
};

/**
 * @brief 对整个序列（Sequence）执行涟漪移除区域操作的复合命令。
 *
 * 这是一个 MultiUndoCommand，可能内部聚合了一个或多个 TrackListRippleRemoveAreaCommand。
 */
class TimelineRippleRemoveAreaCommand : public MultiUndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param timeline 目标序列。
   * @param in 涟漪移除区域的入点。
   * @param out 涟漪移除区域的出点。
   */
  TimelineRippleRemoveAreaCommand(Sequence* timeline, rational in, rational out);

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回序列所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return timeline_->project(); }

 private:
  Sequence* timeline_; // 目标序列
};

/**
 * @brief 使用涟漪工具（Ripple Tool）在轨道列表上进行编辑的命令。
 *
 * 当用户使用涟漪工具拖动区块边缘时，此命令负责处理所有相关的区块长度和位置调整。
 */
class TrackListRippleToolCommand : public UndoCommand {
 public:
  /**
   * @brief 存储涟漪操作相关信息的结构体。
   */
  struct RippleInfo {
    Block* block;      ///< 指向被操作的区块。
    bool append_gap;   ///< 是否需要在区块后附加一个空白区块。
  };

  /**
   * @brief 构造函数。
   * @param track_list 目标轨道列表。
   * @param info 一个哈希表，键是轨道指针，值是该轨道上的 RippleInfo。
   * @param ripple_movement 涟漪移动的量。
   * @param movement_mode 编辑的移动模式（例如裁切入点、裁切出点）。
   */
  TrackListRippleToolCommand(TrackList* track_list, const QHash<Track*, RippleInfo>& info,
                             const rational& ripple_movement, const Timeline::MovementMode& movement_mode);

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回轨道列表父级（通常是 Sequence）所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_list_->parent()->project(); }

 protected:
  /**
   * @brief 执行涟漪工具操作。
   */
  void redo() override { ripple(true); }

  /**
   * @brief 撤销涟漪工具操作。
   */
  void undo() override { ripple(false); }

 private:
  /**
   * @brief 执行或撤销涟漪操作的核心逻辑。
   * @param redo 如果为 true，则执行 redo 操作；否则执行 undo 操作。
   */
  void ripple(bool redo);

  TrackList* track_list_; // 目标轨道列表

  QHash<Track*, RippleInfo> info_; // 每个轨道的涟漪操作信息
  rational ripple_movement_;       // 涟漪移动的总量
  Timeline::MovementMode movement_mode_; // 当前的编辑模式

  /**
   * @brief 存储每个轨道在操作过程中的工作数据。
   */
  struct WorkingData {
    GapBlock* created_gap = nullptr;    ///< 如果创建了空白区块，则指向它。
    Block* removed_gap_after{};         ///< 如果移除了某个区块后的空白，则记录该区块。
    rational old_length;                ///< 区块的原始长度。
    rational earliest_point_of_change;  ///< 此轨道上发生改变的最早时间点。
  };

  QHash<Track*, WorkingData> working_data_; // 每个轨道对应的工作数据

  QObject memory_manager_; // 用于管理动态创建的 GapBlock 的生命周期
};

/**
 * @brief 在序列的指定区域内涟漪式删除空白区块的命令。
 */
class TimelineRippleDeleteGapsAtRegionsCommand : public UndoCommand {
 public:
  /**
   * @brief 定义区域列表的类型别名，每个区域由一个轨道和该轨道上的时间范围组成。
   */
  using RangeList = QVector<QPair<Track*, TimeRange> >;

  /**
   * @brief 构造函数。
   * @param vo 目标序列。
   * @param regions 要删除空白的区域列表。
   */
  TimelineRippleDeleteGapsAtRegionsCommand(Sequence* vo, RangeList regions)
      : timeline_(vo), regions_(std::move(regions)) {}

  /**
   * @brief 析构函数。
   *
   * 负责清理 commands_ 向量中的所有命令对象。
   */
  ~TimelineRippleDeleteGapsAtRegionsCommand() override { qDeleteAll(commands_); }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回序列所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return timeline_->project(); }

  /**
   * @brief 检查此命令是否包含任何实际要执行的子命令。
   * @return 如果子命令列表不为空，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool HasCommands() const { return !commands_.isEmpty(); }

 protected:
  /**
   * @brief 命令准备阶段。
   *
   * 遍历指定的区域，识别出可以被删除的空白区块，并为它们创建子命令。
   */
  void prepare() override;

  /**
   * @brief 执行涟漪删除空白的操作。
   */
  void redo() override;

  /**
   * @brief 撤销涟漪删除空白的操作。
   */
  void undo() override;

 private:
  Sequence* timeline_; // 目标序列
  RangeList regions_;  // 要处理的区域列表

  QVector<UndoCommand*> commands_; // 存储实际执行删除操作的子命令

  /**
   * @brief 存储待移除空白区块请求的内部结构体。
   */
  struct RemovalRequest {
    GapBlock* gap;    ///< 指向要移除的空白区块。
    TimeRange range;  ///< 空白区块实际被影响（移除）的时间范围。
  };
};

}  // namespace olive

#endif  // TIMELINEUNDORIPPLE_H
