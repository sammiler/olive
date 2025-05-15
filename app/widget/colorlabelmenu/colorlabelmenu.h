#ifndef COLORLABELMENU_H
#define COLORLABELMENU_H

#include "widget/menu/menu.h"

namespace olive {

class ColorLabelMenu : public Menu {
  Q_OBJECT
 public:
  explicit ColorLabelMenu(QWidget* parent = nullptr);

  void changeEvent(QEvent* event) override;

 signals:
  void ColorSelected(int i);

 private:
  void Retranslate();

  QVector<QAction*> color_items_;

 private slots:
  void ActionTriggered();
};

}  // namespace olive

#endif  // COLORLABELMENU_H
