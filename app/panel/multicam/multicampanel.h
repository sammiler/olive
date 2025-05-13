#ifndef MULTICAMPANEL_H
#define MULTICAMPANEL_H

#include "panel/viewer/viewerbase.h"
#include "widget/multicam/multicamwidget.h"

namespace olive {

class MulticamPanel : public TimeBasedPanel {
  Q_OBJECT
 public:
  MulticamPanel();

  MulticamWidget *GetMulticamWidget() const { return dynamic_cast<MulticamWidget *>(GetTimeBasedWidget()); }

 protected:
  virtual void Retranslate() override;
};

}  // namespace olive

#endif  // MULTICAMPANEL_H
