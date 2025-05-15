#ifndef SPEEDDURATIONDIALOG_H
#define SPEEDDURATIONDIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>

#include "node/block/clip/clip.h"
#include "node/block/gap/gap.h"
#include "undo/undocommand.h"
#include "widget/slider/floatslider.h"
#include "widget/slider/rationalslider.h"

namespace olive {

class SpeedDurationDialog : public QDialog {
  Q_OBJECT
 public:
  explicit SpeedDurationDialog(const QVector<ClipBlock *> &clips, const rational &timebase, QWidget *parent = nullptr);

 public slots:
  void accept() override;

 signals:

 private:
  static rational GetLengthAdjustment(const rational &original_length, double original_speed, double new_speed,
                                      const rational &timebase);

  static double GetSpeedAdjustment(double original_speed, const rational &original_length, const rational &new_length);

  QVector<ClipBlock *> clips_;

  FloatSlider *speed_slider_;

  RationalSlider *dur_slider_;

  QCheckBox *link_box_;

  QCheckBox *reverse_box_;

  QCheckBox *maintain_audio_pitch_box_;

  QCheckBox *ripple_box_;

  QComboBox *loop_combo_;

  int start_reverse_;

  int start_maintain_audio_pitch_;

  double start_speed_;

  rational start_duration_;

  int start_loop_;

  rational timebase_;

 private slots:
  void SpeedChanged(double s);

  void DurationChanged(const rational &r);
};

}  // namespace olive

#endif  // SPEEDDURATIONDIALOG_H
