

#ifndef TIMELINE_PANEL_H
#define TIMELINE_PANEL_H

#include "panel/timebased/timebased.h"
#include "widget/timelinewidget/timelinewidget.h"

namespace olive {

/**
 * @brief Panel container for a TimelineWidget
 */
class TimelinePanel : public TimeBasedPanel {
  Q_OBJECT
 public:
  explicit TimelinePanel(const QString &name);

  [[nodiscard]] inline TimelineWidget *timeline_widget() const {
    return dynamic_cast<TimelineWidget *>(GetTimeBasedWidget());
  }

  void SplitAtPlayhead() const;

  void LoadData(const Info &info) override;
  [[nodiscard]] Info SaveData() const override;

  void SelectAll() override;

  void DeselectAll() override;

  void RippleToIn() override;

  void RippleToOut() override;

  void EditToIn() override;

  void EditToOut() override;

  void DeleteSelected() override;

  void RippleDelete() override;

  void IncreaseTrackHeight() override;

  void DecreaseTrackHeight() override;

  void ToggleLinks() override;

  void PasteInsert() override;

  void DeleteInToOut() override;

  void RippleDeleteInToOut() override;

  void ToggleSelectedEnabled() override;

  void SetColorLabel(int index) override;

  void NudgeLeft() override;

  void NudgeRight() override;

  void MoveInToPlayhead() override;

  void MoveOutToPlayhead() override;

  void RenameSelected() override;

  void AddDefaultTransitionsToSelected() const { timeline_widget()->AddDefaultTransitionsToSelected(); }

  void ShowSpeedDurationDialogForSelectedClips() const { timeline_widget()->ShowSpeedDurationDialogForSelectedClips(); }

  void NestSelectedClips() const { timeline_widget()->NestSelectedClips(); }

  void InsertFootageAtPlayhead(const QVector<ViewerOutput *> &footage) const;

  void OverwriteFootageAtPlayhead(const QVector<ViewerOutput *> &footage) const;

  [[nodiscard]] const QVector<Block *> &GetSelectedBlocks() const { return timeline_widget()->GetSelectedBlocks(); }

  [[nodiscard]] Sequence *GetSequence() const { return dynamic_cast<Sequence *>(GetConnectedViewer()); }

 protected:
  void Retranslate() override;

 signals:
  void BlockSelectionChanged(const QVector<Block *> &selected_blocks);

  void RequestCaptureStart(const TimeRange &time, const Track::Reference &track);

  void RevealViewerInProject(ViewerOutput *r);
  void RevealViewerInFootageViewer(ViewerOutput *r, const TimeRange &range);
};

}  // namespace olive

#endif  // TIMELINE_PANEL_H
