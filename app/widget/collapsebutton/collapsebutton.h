

#ifndef COLLAPSEBUTTON_H
#define COLLAPSEBUTTON_H

#include <QPushButton>

#include "common/define.h"

namespace olive {

class CollapseButton : public QPushButton {
  Q_OBJECT
 public:
  explicit CollapseButton(QWidget* parent = nullptr);

 private slots:
  void UpdateIcon(bool e);
};

}  // namespace olive

#endif  // COLLAPSEBUTTON_H
