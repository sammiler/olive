#ifndef MARKERPROPERTIESDIALOG_H
#define MARKERPROPERTIESDIALOG_H

#include <QDialog>
#include <QLineEdit>

#include "timeline/timelinemarker.h"
#include "widget/colorlabelmenu/colorcodingcombobox.h"
#include "widget/slider/rationalslider.h"

namespace olive {

class LineEditWithFocusSignal : public QLineEdit {
  Q_OBJECT
 public:
  explicit LineEditWithFocusSignal(QWidget *parent = nullptr) : QLineEdit(parent) {}

 protected:
  void focusInEvent(QFocusEvent *e) override {
    QLineEdit::focusInEvent(e);
    emit Focused();
  }

 signals:
  void Focused();
};

class MarkerPropertiesDialog : public QDialog {
  Q_OBJECT
 public:
  MarkerPropertiesDialog(const std::vector<TimelineMarker *> &markers, const rational &timebase,
                         QWidget *parent = nullptr);

 public slots:
  void accept() override;

 private:
  std::vector<TimelineMarker *> markers_;

  LineEditWithFocusSignal *label_edit_;

  ColorCodingComboBox *color_menu_;

  RationalSlider *in_slider_;

  RationalSlider *out_slider_;
};

}  // namespace olive

#endif  // MARKERPROPERTIESDIALOG_H
