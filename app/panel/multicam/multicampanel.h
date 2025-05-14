#ifndef MULTICAMPANEL_H
#define MULTICAMPANEL_H

#include "panel/viewer/viewerbase.h"
#include "widget/multicam/multicamwidget.h"

namespace olive {

class MulticamPanel : public TimeBasedPanel {
  Q_OBJECT
 public:
  MulticamPanel();

  [[nodiscard]] MulticamWidget *GetMulticamWidget() const {
    return dynamic_cast<MulticamWidget *>(GetTimeBasedWidget());
  }

 protected:
  void Retranslate() override;
};

}  // namespace olive

#endif  // MULTICAMPANEL_H
