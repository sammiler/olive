#ifndef TIMELINEUNDOPOINTER_H
#define TIMELINEUNDOPOINTER_H

#include "node/block/gap/gap.h"                // 引入 GapBlock 类的定义，用于处理空白区块
#include "node/output/track/track.h"           // 引入 Track 类的定义
#include "node/output/track/tracklist.h"       // 引入 TrackList 类的定义
#include "node/project/sequence/sequence.h"    // 引入 Sequence 类的定义
#include "timelineundogeneral.h"               // 引入通用的时间轴撤销命令定义
#include "timelineundoripple.h"                // 引入涟漪编辑相关的撤销命令定义

namespace olive {

/**
 * @brief 在时间轴上执行裁切操作的命令，仅影响当前区块和相邻区块。
 *
 * 此命令改变一个区块的长度，同时补偿性地改变直接相邻区块的长度，
 * 以确保轨道的其余部分不受影响。
 *
 * 默认情况下，这只会影响空白区块（Gap）的长度。如果相邻区块需要增加长度但它不是空白区块，
 * 则会创建一个新的空白区块来填充该时间。可以通过 SetTrimIsARollEdit() 设置此命令
 * 即使相邻片段不是空白区块也总是进行裁切（类似于卷动编辑）。
 */
class BlockTrimCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param track 目标轨道。
   * @param block 要裁切的区块。
   * @param new_length 区块的新长度。
   * @param mode 裁切模式 (例如，裁切入点或出点)。
   */
  BlockTrimCommand(Track* track, Block* block, rational new_length, Timeline::MovementMode mode)
      : track_(track),
        block_(block),
        new_length_(new_length),
        mode_(mode),
        deleted_adjacent_command_(nullptr),
        trim_is_a_roll_edit_(false) {}

  /**
   * @brief 析构函数。
   *
   * 负责清理 deleted_adjacent_command_（如果已创建）。
   */
  ~BlockTrimCommand() override { delete deleted_adjacent_command_; }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回轨道所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

  /**
   * @brief 设置裁切是否应始终影响相邻片段（表现为卷动编辑），而不是创建空白。
   * @param e 如果为 true，则表现为卷动编辑。
   */
  void SetTrimIsARollEdit(bool e) { trim_is_a_roll_edit_ = e; }

  /**
   * @brief 设置长度变为零的相邻区块是否应从整个节点图中移除。
   *
   * 如果相邻区块的长度变为0，它会自动从轨道中移除。
   * 默认情况下，它也会从整个节点图中移除。将此设置为 FALSE 可禁用该功能。
   * @param e 如果为 true（默认），则从图中移除；否则不移除。
   */
  void SetRemoveZeroLengthFromGraph(bool e) { remove_block_from_graph_ = e; }

 protected:
  /**
   * @brief 命令准备阶段，在首次 redo() 或 undo() 前调用。
   */
  void prepare() override;
  /**
   * @brief 执行裁切操作。
   */
  void redo() override;
  /**
   * @brief 撤销裁切操作。
   */
  void undo() override;

 private:
  bool doing_nothing_{};        // 标记此命令是否实际执行任何操作
  rational trim_diff_;          // 裁切的长度差值

  Track* track_;                // 目标轨道
  Block* block_;                // 被裁切的区块
  rational old_length_;         // 区块的原始长度
  rational new_length_;         // 区块的新长度
  Timeline::MovementMode mode_; // 裁切模式 (入点或出点)

  Block* adjacent_{};           // 相邻的区块
  bool needs_adjacent_{};       // 是否需要处理相邻区块
  bool we_created_adjacent_{};  // 标记是否是此命令创建了相邻区块（通常是空白）
  bool we_removed_adjacent_{};  // 标记是否是此命令移除了相邻区块
  UndoCommand* deleted_adjacent_command_; // 如果移除了相邻区块，存储其移除命令以供撤销

  bool trim_is_a_roll_edit_;    // 标记是否为卷动编辑模式
  bool remove_block_from_graph_{}; // 标记是否从节点图中移除零长度区块

  QObject memory_manager_;      // 用于管理此命令作用域内动态创建的对象的生命周期
};

/**
 * @brief 在轨道上滑动一个或多个区块的撤销命令。
 *
 * 滑动操作会移动选定的区块，并调整相邻区块的长度（或创建/移除空白）以适应移动。
 */
class TrackSlideCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param track 目标轨道。
   * @param moving_blocks 要滑动的区块列表。
   * @param in_adjacent 滑动区块组入点侧的相邻区块。
   * @param out_adjacent 滑动区块组出点侧的相邻区块。
   * @param movement 滑动的距离和方向 (rational 类型)。
   */
  TrackSlideCommand(Track* track, const QList<Block*>& moving_blocks, Block* in_adjacent, Block* out_adjacent,
                    const rational& movement)
      : track_(track),
        blocks_(moving_blocks),
        movement_(movement),
        we_removed_in_adjacent_(false),
        in_adjacent_(in_adjacent),
        in_adjacent_remove_command_(nullptr),
        we_removed_out_adjacent_(false),
        out_adjacent_(out_adjacent),
        out_adjacent_remove_command_(nullptr) {
    Q_ASSERT(!movement_.isNull()); // 断言移动量不为空或无效
  }

  /**
   * @brief 析构函数。
   *
   * 负责清理与相邻区块移除相关的命令。
   */
  ~TrackSlideCommand() override {
    delete in_adjacent_remove_command_;
    delete out_adjacent_remove_command_;
  }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回轨道所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  /**
   * @brief 命令准备阶段。
   */
  void prepare() override;
  /**
   * @brief 执行滑动操作。
   */
  void redo() override;
  /**
   * @brief 撤销滑动操作。
   */
  void undo() override;

 private:
  Track* track_;             // 目标轨道
  QList<Block*> blocks_;     // 被滑动的区块列表
  rational movement_;        // 滑动的量

  bool we_created_in_adjacent_{};  // 标记是否是此命令在入点侧创建了相邻区块
  bool we_removed_in_adjacent_{};  // 标记是否是此命令在入点侧移除了相邻区块
  Block* in_adjacent_;             // 入点侧的相邻区块
  UndoCommand* in_adjacent_remove_command_; // 如果移除了入点侧相邻区块，存储其移除命令

  bool we_created_out_adjacent_{}; // 标记是否是此命令在出点侧创建了相邻区块
  bool we_removed_out_adjacent_{}; // 标记是否是此命令在出点侧移除了相邻区块
  Block* out_adjacent_;            // 出点侧的相邻区块
  UndoCommand* out_adjacent_remove_command_; // 如果移除了出点侧相邻区块，存储其移除命令

  QObject memory_manager_; // 用于管理此命令作用域内动态创建的对象的生命周期
};

/**
 * @brief 以破坏性方式将 `block` 放置在入点 `start` 的命令。
 *
 * 区块保证被放置在指定的起始点。如果此区域中存在其他区块，
 * 它们将被裁切或移除，以便为该区块腾出空间。此外，如果区块
 * 被放置在序列末尾之后，将插入一个 GapBlock 来补偿。
 */
class TrackPlaceBlockCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param timeline 指向目标 TrackList (时间轴) 的指针。
   * @param track 要放置区块的轨道索引。
   * @param block 要放置的 Block 指针。
   * @param in 区块的入点时间。
   */
  TrackPlaceBlockCommand(TrackList* timeline, int track, Block* block, rational in)
      : timeline_(timeline),
        track_index_(track),
        in_(in),
        gap_(nullptr),
        insert_(block),
        ripple_remove_command_(nullptr) {}

  /**
   * @brief 析构函数。
   *
   * 负责清理 add_track_commands_ 和 ripple_remove_command_。
   */
  ~TrackPlaceBlockCommand() override;

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 TrackList 父级（通常是 Sequence）所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return timeline_->parent()->project(); }

 protected:
  /**
   * @brief 执行放置区块的操作。
   */
  void redo() override;

  /**
   * @brief 撤销放置区块的操作。
   */
  void undo() override;

 private:
  TrackList* timeline_;     // 目标时间轴
  int track_index_;         // 目标轨道的索引
  rational in_;             // 区块的放置入点
  GapBlock* gap_;           // 如果需要，用于填充序列末尾的空白区块
  Block* insert_;           // 要插入的区块
  QVector<TimelineAddTrackCommand*> add_track_commands_; // 如果目标轨道不存在，存储用于添加轨道的命令
  QObject memory_manager_;  // 用于管理动态创建的对象的生命周期
  TrackRippleRemoveAreaCommand* ripple_remove_command_; // 用于处理因放置区块而可能触发的涟漪删除区域的命令
};

}  // namespace olive

#endif  // TIMELINEUNDOPOINTER_H
