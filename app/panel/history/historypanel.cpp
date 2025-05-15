

#include "historypanel.h"

#include "widget/history/historywidget.h"

namespace olive {

HistoryPanel::HistoryPanel() : PanelWidget(QStringLiteral("HistoryPanel")) {
  SetWidgetWithPadding(new HistoryWidget(this));

  Retranslate();
}

void HistoryPanel::Retranslate() { SetTitle(tr("History")); }

}  // namespace olive
