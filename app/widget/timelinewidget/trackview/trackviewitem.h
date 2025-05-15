#ifndef TRACKVIEWITEM_H
#define TRACKVIEWITEM_H

#include <QPushButton>
#include <QStackedWidget>
#include <QWidget>

#include "node/output/track/track.h"
#include "widget/clickablelabel/clickablelabel.h"
#include "widget/focusablelineedit/focusablelineedit.h"
#include "widget/timelinewidget/view/timelineviewmouseevent.h"

namespace olive {

class TrackViewItem : public QWidget {
  Q_OBJECT
 public:
  explicit TrackViewItem(Track* track, QWidget* parent = nullptr);

 signals:
  void AboutToDeleteTrack(Track* track);

 private:
  [[nodiscard]] static QPushButton* CreateMSLButton(const QColor& checked_color);

  QStackedWidget* stack_;

  ClickableLabel* label_;
  FocusableLineEdit* line_edit_;

  QPushButton* mute_button_;
  QPushButton* solo_button_{};
  QPushButton* lock_button_;

  Track* track_;

 private slots:
  void LabelClicked();

  void LineEditConfirmed();

  void LineEditCancelled();

  void UpdateLabel();

  void ShowContextMenu(const QPoint& p);

  void DeleteTrack();

  void DeleteAllEmptyTracks();

  void UpdateMuteButton(bool e);

  void UpdateLockButton(bool e);
};

}  // namespace olive

#endif  // TRACKVIEWITEM_H
