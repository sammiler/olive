

#ifndef SLIDERLABEL_H
#define SLIDERLABEL_H

#include <QLabel>

#include "common/define.h"

namespace olive {

class SliderLabel : public QLabel {
  Q_OBJECT
 public:
  explicit SliderLabel(QWidget *parent);

  void SetColor(const QColor &c);

 protected:
  void mousePressEvent(QMouseEvent *e) override;

  void mouseReleaseEvent(QMouseEvent *e) override;

  void focusInEvent(QFocusEvent *event) override;

  void changeEvent(QEvent *event) override;

 signals:
  void LabelPressed();

  void LabelReleased();

  void focused();

  void RequestReset();

  void ChangeSliderType();

 private:
  bool override_color_enabled_;
  QColor override_color_;
};

}  // namespace olive

#endif  // SLIDERLABEL_H
