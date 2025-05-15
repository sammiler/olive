#ifndef TIMELINEUNDORIPPLE_H
#define TIMELINEUNDORIPPLE_H

#include <utility>

#include "node/block/gap/gap.h"
#include "node/output/track/track.h"
#include "node/output/track/tracklist.h"
#include "node/project/sequence/sequence.h"
#include "timelineundogeneral.h"
#include "timelineundosplit.h"
#include "timelineundotrack.h"

namespace olive {

/**
 * @brief Clears the area between in and out
 *
 * The area between `in` and `out` is guaranteed to be freed. BLocks are trimmed and removed to free this space.
 * By default, nothing takes this area meaning all subsequent clips are pushed backward, however you can specify
 * a block to insert at the `in` point. No checking is done to ensure `insert` is the same length as `in` to `out`.
 */
class TrackRippleRemoveAreaCommand : public UndoCommand {
 public:
  TrackRippleRemoveAreaCommand(Track* track, const TimeRange& range);

  ~TrackRippleRemoveAreaCommand() override;

  [[nodiscard]] Project* GetRelevantProject() const override { return track_->project(); }

  /**
   * @brief Block to insert after if you want to insert something between this ripple
   */
  [[nodiscard]] Block* GetInsertionIndex() const { return insert_previous_; }

  [[nodiscard]] Block* GetSplicedBlock() const {
    if (splice_split_command_) {
      return splice_split_command_->new_block();
    }

    return nullptr;
  }

  void SetAllowSplittingGaps(bool e) { allow_splitting_gaps_ = e; }

 protected:
  void prepare() override;

  void redo() override;

  void undo() override;

 private:
  struct TrimOperation {
    Block* block{};
    rational old_length;
    rational new_length;
  };

  struct RemoveOperation {
    Block* block;
    Block* before;
  };

  Track* track_;
  TimeRange range_;

  TrimOperation trim_out_;
  QVector<RemoveOperation> removals_;
  TrimOperation trim_in_;
  Block* insert_previous_{};
  bool allow_splitting_gaps_;

  BlockSplitCommand* splice_split_command_;
  QVector<UndoCommand*> remove_block_commands_;
};

class TrackListRippleRemoveAreaCommand : public UndoCommand {
 public:
  TrackListRippleRemoveAreaCommand(TrackList* list, rational in, rational out) : list_(list), range_(in, out) {}

  ~TrackListRippleRemoveAreaCommand() override { qDeleteAll(commands_); }

  [[nodiscard]] Project* GetRelevantProject() const override { return list_->parent()->project(); }

 protected:
  void prepare() override;

  void redo() override;

  void undo() override;

 private:
  TrackList* list_;

  QList<Track*> working_tracks_;

  TimeRange range_;

  QVector<TrackRippleRemoveAreaCommand*> commands_;
};

class TimelineRippleRemoveAreaCommand : public MultiUndoCommand {
 public:
  TimelineRippleRemoveAreaCommand(Sequence* timeline, rational in, rational out);

  [[nodiscard]] Project* GetRelevantProject() const override { return timeline_->project(); }

 private:
  Sequence* timeline_;
};

class TrackListRippleToolCommand : public UndoCommand {
 public:
  struct RippleInfo {
    Block* block;
    bool append_gap;
  };

  TrackListRippleToolCommand(TrackList* track_list, const QHash<Track*, RippleInfo>& info,
                             const rational& ripple_movement, const Timeline::MovementMode& movement_mode);

  [[nodiscard]] Project* GetRelevantProject() const override { return track_list_->parent()->project(); }

 protected:
  void redo() override { ripple(true); }

  void undo() override { ripple(false); }

 private:
  void ripple(bool redo);

  TrackList* track_list_;

  QHash<Track*, RippleInfo> info_;
  rational ripple_movement_;
  Timeline::MovementMode movement_mode_;

  struct WorkingData {
    GapBlock* created_gap = nullptr;
    Block* removed_gap_after{};
    rational old_length;
    rational earliest_point_of_change;
  };

  QHash<Track*, WorkingData> working_data_;

  QObject memory_manager_;
};

class TimelineRippleDeleteGapsAtRegionsCommand : public UndoCommand {
 public:
  using RangeList = QVector<QPair<Track*, TimeRange> >;

  TimelineRippleDeleteGapsAtRegionsCommand(Sequence* vo, RangeList regions)
      : timeline_(vo), regions_(std::move(regions)) {}

  ~TimelineRippleDeleteGapsAtRegionsCommand() override { qDeleteAll(commands_); }

  [[nodiscard]] Project* GetRelevantProject() const override { return timeline_->project(); }

  [[nodiscard]] bool HasCommands() const { return !commands_.isEmpty(); }

 protected:
  void prepare() override;

  void redo() override;

  void undo() override;

 private:
  Sequence* timeline_;
  RangeList regions_;

  QVector<UndoCommand*> commands_;

  struct RemovalRequest {
    GapBlock* gap;
    TimeRange range;
  };
};

}  // namespace olive

#endif  // TIMELINEUNDORIPPLE_H
