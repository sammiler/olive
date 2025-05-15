

#ifndef IMPORTTIMELINETOOL_H
#define IMPORTTIMELINETOOL_H

#include "tool.h"

namespace olive {

class ImportTool : public TimelineTool {
 public:
  explicit ImportTool(TimelineWidget *parent);

  void DragEnter(TimelineViewMouseEvent *event) override;
  void DragMove(TimelineViewMouseEvent *event) override;
  void DragLeave(QDragLeaveEvent *event) override;
  void DragDrop(TimelineViewMouseEvent *event) override;

  using DraggedFootageData = QVector<QPair<ViewerOutput *, QVector<Track::Reference> > >;

  void PlaceAt(const QVector<ViewerOutput *> &footage, const rational &start, bool insert, MultiUndoCommand *command,
               int track_offset = 0, bool jump_to_end = false);
  void PlaceAt(const DraggedFootageData &footage, const rational &start, bool insert, MultiUndoCommand *command,
               int track_offset = 0, bool jump_to_end = false);

  enum DropWithoutSequenceBehavior { kDWSAsk, kDWSAuto, kDWSManual, kDWSDisable };

 private:
  void FootageToGhosts(rational ghost_start, const DraggedFootageData &sorted, const rational &dest_tb,
                       const int &track_start);

  void PrepGhosts(const rational &frame, const int &track_index);

  void DropGhosts(bool insert, MultiUndoCommand *parent_command);

  TimelineViewGhostItem *CreateGhost(const TimeRange &range, const rational &media_in, const Track::Reference &track);

  DraggedFootageData dragged_footage_;

  int import_pre_buffer_;

  rational ghost_offset_;
};

}  // namespace olive

#endif  // IMPORTTIMELINETOOL_H
