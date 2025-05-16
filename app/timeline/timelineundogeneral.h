#ifndef TIMELINEUNDOGENERAL_H
#define TIMELINEUNDOGENERAL_H

#include <ranges> // 引入 C++20 ranges库，用于更方便地处理序列范围

#include "config/config.h"                     // 引入配置文件相关的定义，例如 OLIVE_CONFIG
#include "node/block/clip/clip.h"              // 引入 ClipBlock 类的定义
#include "node/block/gap/gap.h"                // 引入 GapBlock 类的定义
#include "node/output/track/track.h"           // 引入 Track 类的定义
#include "node/output/track/tracklist.h"       // 引入 TrackList 类的定义
#include "node/output/viewer/viewer.h"         // 引入 Viewer 类的定义 (虽然在此文件中未直接使用，但可能是上下文依赖)
#include "node/project/sequence/sequence.h"    // 引入 Sequence 类的定义
#include "timelineundosplit.h"                 // 引入时间轴分割相关的撤销命令定义

namespace olive {

/**
 * @brief 调整区块（Block）长度的撤销命令。
 */
class BlockResizeCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param block 指向要调整大小的 Block 对象的指针。
   * @param new_length 区块的新长度 (rational 类型)。
   */
  BlockResizeCommand(Block* block, rational new_length) : block_(block), new_length_(new_length) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 Block 所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return block_->project(); }

 protected:
  /**
   * @brief 执行调整区块长度的操作。
   */
  void redo() override;

  /**
   * @brief 撤销调整区块长度的操作。
   */
  void undo() override;

 private:
  Block* block_;             // 指向被操作的区块
  rational old_length_;      // 区块的原始长度，在 redo() 中记录
  rational new_length_;      // 区块的新长度
};

/**
 * @brief 调整区块长度并相应调整媒体入点（Media In）的撤销命令。
 *
 * 当区块的开始边缘被拖动以改变长度时，媒体内容的入点也会随之改变。
 */
class BlockResizeWithMediaInCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param block 指向要调整大小的 Block 对象的指针。
   * @param new_length 区块的新长度 (rational 类型)。
   */
  BlockResizeWithMediaInCommand(Block* block, rational new_length) : block_(block), new_length_(new_length) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 Block 所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return block_->project(); }

 protected:
  /**
   * @brief 执行调整区块长度和媒体入点的操作。
   */
  void redo() override;

  /**
   * @brief 撤销调整区块长度和媒体入点的操作。
   */
  void undo() override;

 private:
  Block* block_;             // 指向被操作的区块
  rational old_length_;      // 区块的原始长度
  rational new_length_;      // 区块的新长度
  // 注意：媒体入点的旧值可能也需要在此存储，以便正确撤销。目前代码未显示，但实际实现中通常需要。
};

/**
 * @brief 设置片段区块（ClipBlock）媒体入点（Media In）的撤销命令。
 *
 * 用于“滑动”编辑，即改变片段在时间轴上的位置，但不改变其在源媒体中的内容。
 */
class BlockSetMediaInCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param block 指向要设置媒体入点的 ClipBlock 对象的指针。
   * @param new_media_in 新的媒体入点时间 (rational 类型)。
   */
  BlockSetMediaInCommand(ClipBlock* block, rational new_media_in) : block_(block), new_media_in_(new_media_in) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 ClipBlock 所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return block_->project(); }

 protected:
  /**
   * @brief 执行设置媒体入点的操作。
   */
  void redo() override;

  /**
   * @brief 撤销设置媒体入点的操作。
   */
  void undo() override;

 private:
  ClipBlock* block_;         // 指向被操作的片段区块
  rational old_media_in_;    // 旧的媒体入点时间
  rational new_media_in_;    // 新的媒体入点时间
};

/**
 * @brief 在时间轴上添加新轨道的撤销命令。
 */
class TimelineAddTrackCommand : public UndoCommand {
 public:
  /**
   * @brief 显式构造函数，使用配置决定是否自动合并轨道。
   * @param timeline 指向目标 TrackList (时间轴) 的指针。
   */
  explicit TimelineAddTrackCommand(TrackList* timeline)
      : TimelineAddTrackCommand(timeline, OLIVE_CONFIG("AutoMergeTracks").toBool()) {} // 调用另一个构造函数，并从配置中读取是否自动合并轨道

  /**
   * @brief 构造函数。
   * @param timeline 指向目标 TrackList (时间轴) 的指针。
   * @param automerge_tracks 是否自动为新轨道创建合并节点。
   */
  TimelineAddTrackCommand(TrackList* timeline, bool automerge_tracks);

  /**
   * @brief 静态辅助函数，立即创建并执行添加轨道命令。
   * @param timeline 目标时间轴。
   * @return 返回新创建的 Track 指针。
   */
  static Track* RunImmediately(TrackList* timeline) {
    TimelineAddTrackCommand c(timeline); // 创建命令
    c.redo();                            // 执行命令
    return c.track();                    // 返回创建的轨道
  }

  /**
   * @brief 静态辅助函数，立即创建并执行添加轨道命令，可指定是否自动合并。
   * @param timeline 目标时间轴。
   * @param automerge 是否自动合并轨道。
   * @return 返回新创建的 Track 指针。
   */
  static Track* RunImmediately(TrackList* timeline, bool automerge) {
    TimelineAddTrackCommand c(timeline, automerge); // 创建命令
    c.redo();                                     // 执行命令
    return c.track();                             // 返回创建的轨道
  }

  /**
   * @brief 析构函数。
   *
   * 负责清理 position_command_（如果已创建）。
   */
  ~TimelineAddTrackCommand() override { delete position_command_; }

  /**
   * @brief 获取此命令创建或操作的轨道。
   * @return 返回指向 Track 对象的指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Track* track() const { return track_; }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 TrackList 父级（通常是 Sequence）所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return timeline_->parent()->project(); }

 protected:
  /**
   * @brief 执行添加轨道的操作。
   */
  void redo() override;

  /**
   * @brief 撤销添加轨道的操作。
   */
  void undo() override;

 private:
  TrackList* timeline_; // 指向目标时间轴（轨道列表）

  Track* track_;        // 指向新创建的轨道
  Node* merge_;         // 如果自动合并，指向合并节点
  NodeInput base_;      // 合并节点的基准输入
  NodeInput blend_;     // 合并节点的混合输入

  NodeInput direct_;    // 如果不合并，轨道的直接输出连接

  MultiUndoCommand* position_command_; // 可能用于处理轨道位置相关的子命令

  QObject memory_manager_; // 用于管理 track_ 和 merge_ 等动态分配对象的生命周期
};

/**
 * @brief 从时间轴上移除轨道的撤销命令。
 */
class TimelineRemoveTrackCommand : public UndoCommand {
 public:
  /**
   * @brief 显式构造函数。
   * @param track 指向要移除的 Track 对象的指针。
   */
  explicit TimelineRemoveTrackCommand(Track* track) : track_(track), remove_command_(nullptr) {}

  /**
   * @brief 析构函数。
   *
   * 负责清理 remove_command_（如果已创建）。
   */
  ~TimelineRemoveTrackCommand() override { delete remove_command_; }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 Track 所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  /**
   * @brief 准备阶段，在 redo() 或 undo() 首次执行前调用。
   *
   * 通常用于保存命令执行所需的状态。
   */
  void prepare() override;

  /**
   * @brief 执行移除轨道的操作。
   */
  void redo() override;

  /**
   * @brief 撤销移除轨道的操作（即重新添加轨道）。
   */
  void undo() override;

 private:
  Track* track_; // 指向要被移除的轨道

  TrackList* list_{}; // 指向轨道所属的列表 (在 prepare 中获取)
  int index_{};       // 轨道在列表中的原始索引 (在 prepare 中获取)

  UndoCommand* remove_command_; // 可能用于封装更底层的节点移除命令
};

/**
 * @brief 移除转场区块（TransitionBlock）的撤销命令。
 */
class TransitionRemoveCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param block 指向要移除的 TransitionBlock 对象的指针。
   * @param remove_from_graph 是否同时从节点图中移除转场节点。
   */
  TransitionRemoveCommand(TransitionBlock* block, bool remove_from_graph)
      : block_(block), remove_from_graph_(remove_from_graph), remove_command_(nullptr) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 Track 所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

 protected:
  /**
   * @brief 执行移除转场的操作。
   */
  void redo() override;

  /**
   * @brief 撤销移除转场的操作。
   */
  void undo() override;

 private:
  TransitionBlock* block_; // 指向被移除的转场区块

  Track* track_{};         // 转场区块所在的轨道 (在 redo/undo 前获取)

  Block* out_block_{};     // 转场的前一个区块
  Block* in_block_{};      // 转场的后一个区块

  bool remove_from_graph_; // 是否从节点图中移除转场节点
  UndoCommand* remove_command_; // 可能用于封装转场节点的移除命令
};

/**
 * @brief 将轨道上的一个区块替换为空白区块（GapBlock）的撤销命令。
 */
class TrackReplaceBlockWithGapCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param track 目标轨道。
   * @param block 要被替换的区块。
   * @param handle_transitions 是否处理与被替换区块相关的转场，默认为 true。
   */
  TrackReplaceBlockWithGapCommand(Track* track, Block* block, bool handle_transitions = true)
      : track_(track),
        block_(block),
        existing_gap_(nullptr),
        existing_merged_gap_(nullptr),
        our_gap_(nullptr),
        handle_transitions_(handle_transitions) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 Block 所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return block_->project(); }

 protected:
  /**
   * @brief 执行替换区块为空白的操作。
   */
  void redo() override;

  /**
   * @brief 撤销替换操作（即恢复原区块）。
   */
  void undo() override;

 private:
  /**
   * @brief 如果需要，创建用于移除相邻转场的命令。
   * @param next 如果为 true，则处理后一个转场；否则处理前一个转场。
   */
  void CreateRemoveTransitionCommandIfNecessary(bool next);

  Track* track_;    // 目标轨道
  Block* block_;    // 被替换的区块

  GapBlock* existing_gap_;          // 如果被替换区块的位置已存在空白，则指向该空白
  GapBlock* existing_merged_gap_;   // 如果合并了现有的空白，则指向合并后的空白
  bool existing_gap_precedes_{};    // 标记现有的空白是否在被替换区块之前
  GapBlock* our_gap_;               // 此命令创建的空白区块

  bool handle_transitions_;         // 是否处理转场

  QObject memory_manager_;          // 用于管理动态创建的 GapBlock 的生命周期

  QVector<TransitionRemoveCommand*> transition_remove_commands_; // 存储移除转场的子命令
};

/**
 * @brief 启用或禁用区块的撤销命令。
 */
class BlockEnableDisableCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param block 指向要启用/禁用的 Block 对象的指针。
   * @param enabled true 表示启用，false 表示禁用。
   */
  BlockEnableDisableCommand(Block* block, bool enabled)
      : block_(block), old_enabled_(block_->is_enabled()), new_enabled_(enabled) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 Block 所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return block_->project(); }

 protected:
  /**
   * @brief 执行启用/禁用操作。
   */
  void redo() override { block_->set_enabled(new_enabled_); }

  /**
   * @brief 撤销启用/禁用操作。
   */
  void undo() override { block_->set_enabled(old_enabled_); }

 private:
  Block* block_;       // 指向被操作的区块
  bool old_enabled_;   // 区块原始的启用状态
  bool new_enabled_;   // 区块新的启用状态
};

/**
 * @brief 在轨道列表的指定时间点插入指定长度的空白区块的撤销命令。
 *
 * 此命令会在所有受影响的轨道上插入或扩展空白。
 */
class TrackListInsertGaps : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param track_list 目标轨道列表。
   * @param point 插入空白的起始时间点。
   * @param length 插入空白的长度。
   */
  TrackListInsertGaps(TrackList* track_list, const rational& point, const rational& length)
      : track_list_(track_list), point_(point), length_(length), split_command_(nullptr) {}

  /**
   * @brief 析构函数。
   *
   * 负责清理 split_command_（如果已创建）。
   */
  ~TrackListInsertGaps() override { delete split_command_; }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 TrackList 父级（通常是 Sequence）所属的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return track_list_->parent()->project(); }

 protected:
  /**
   * @brief 准备阶段，在 redo() 或 undo() 首次执行前调用。
   */
  void prepare() override;

  /**
   * @brief 执行插入空白的操作。
   */
  void redo() override;

  /**
   * @brief 撤销插入空白的操作。
   */
  void undo() override;

 private:
  TrackList* track_list_; // 指向目标轨道列表

  rational point_;        // 插入空白的起始点
  rational length_;       // 插入空白的长度

  QVector<Track*> working_tracks_; // 受此操作影响的轨道列表

  QVector<Block*> gaps_to_extend_; // 需要扩展长度的现有空白区块

  /**
   * @brief 存储新添加的空白区块信息的结构体。
   */
  struct AddGap {
    GapBlock* gap;    // 指向新添加的空白区块
    Block* before;    // 空白区块之前的一个区块（用于定位）
    Track* track;     // 空白区块所在的轨道
  };

  QVector<AddGap> gaps_added_; // 记录所有新添加的空白区块

  BlockSplitPreservingLinksCommand* split_command_; // 可能用于在插入点分割现有区块的子命令

  QObject memory_manager_; // 用于管理动态创建的 GapBlock 的生命周期
};

/**
 * @brief 为一组片段区块添加默认转场的撤销命令。
 */
class TimelineAddDefaultTransitionCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param clips 要添加转场的 ClipBlock 列表。
   * @param timebase 时间基准，用于计算转场长度（例如，基于帧率）。
   */
  TimelineAddDefaultTransitionCommand(const QVector<ClipBlock*>& clips, const rational& timebase)
      : clips_(clips), timebase_(timebase) {}

  /**
   * @brief 析构函数。
   *
   * 负责清理 commands_ 列表中所有命令对象。
   */
  ~TimelineAddDefaultTransitionCommand() override { qDeleteAll(commands_); }

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 如果片段列表不为空，则返回第一个片段所属的 Project 指针；否则返回 nullptr。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override {
    return clips_.empty() ? nullptr : clips_.first()->project();
  }

 protected:
  /**
   * @brief 准备阶段，在 redo() 或 undo() 首次执行前调用。
   *
   * 此处用于构建实际执行添加转场操作的子命令列表。
   */
  void prepare() override;

  /**
   * @brief 执行添加默认转场的操作（通过执行所有子命令的 redo）。
   */
  void redo() override {
    for (auto command : commands_) { // 遍历所有子命令
      command->redo_now();           // 执行子命令的 redo
    }
  }

  /**
   * @brief 撤销添加默认转场的操作（通过反向执行所有子命令的 undo）。
   */
  void undo() override {
    for (auto command : std::ranges::reverse_view(commands_)) { // 反向遍历子命令
      command->undo_now();                                     // 执行子命令的 undo
    }
  }

 private:
  /**
   * @brief 创建转场的模式枚举。
   */
  enum CreateTransitionMode {
    kIn,     // 在片段的入点创建转场
    kOut,    // 在片段的出点创建转场
    kOutDual // 在片段的出点创建转场，并且该片段是两个相邻片段中的第一个（用于处理中间转场）
  };

  /**
   * @brief 为指定的片段区块添加转场。
   * @param c 目标 ClipBlock。
   * @param mode 创建转场的模式。
   */
  void AddTransition(ClipBlock* c, CreateTransitionMode mode);

  /**
   * @brief 调整片段长度以适应转场。
   * @param c 目标 ClipBlock。
   * @param transition_length 转场的长度。
   * @param out 如果为 true，则调整出点；否则调整入点。
   */
  void AdjustClipLength(ClipBlock* c, const rational& transition_length, bool out);

  /**
   * @brief 验证并可能调整转场长度，以确保其不超过片段的可用长度。
   * @param c 目标 ClipBlock。
   * @param transition_length 要验证和调整的转场长度（引用传递）。
   */
  void ValidateTransitionLength(ClipBlock* c, rational& transition_length);

  QVector<ClipBlock*> clips_;     // 要操作的片段区块列表
  rational timebase_;             // 时间基准
  QVector<UndoCommand*> commands_; // 存储实际执行操作的子命令列表

  QHash<ClipBlock*, rational> lengths_; // 可能用于存储片段的原始长度或其他相关长度信息
};

}  // namespace olive

#endif  // TIMELINEUNDOGENERAL_H
