#ifndef SLIDERLINEEDIT_H
#define SLIDERLINEEDIT_H

#include <QLineEdit>

#include "common/define.h"

namespace olive {

class FocusableLineEdit : public QLineEdit {
  Q_OBJECT
 public:
  explicit FocusableLineEdit(QWidget *parent = nullptr);

 signals:
  void Confirmed();

  void Cancelled();

 protected:
  void keyPressEvent(QKeyEvent *) override;

  void focusOutEvent(QFocusEvent *) override;
};

}  // namespace olive

#endif  // SLIDERLINEEDIT_H
