

#include "table.h"

namespace olive {

NodeTablePanel::NodeTablePanel() : TimeBasedPanel(QStringLiteral("NodeTablePanel")) {
  SetTimeBasedWidget(new NodeTableWidget(this));

  Retranslate();
}

void NodeTablePanel::Retranslate() { SetTitle(tr("Table View")); }

}  // namespace olive
