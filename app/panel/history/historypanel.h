

#ifndef HISTORYPANEL_H
#define HISTORYPANEL_H

#include "panel/panel.h"

namespace olive {

class HistoryPanel : public PanelWidget {
  Q_OBJECT
 public:
  HistoryPanel();

 protected:
  void Retranslate() override;
};

}  // namespace olive

#endif  // HISTORYPANEL_H
