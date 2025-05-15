#include "footageviewer.h"

namespace olive {

#define super ViewerPanelBase

FootageViewerPanel::FootageViewerPanel() : super(QStringLiteral("FootageViewerPanel")) {
  // Set ViewerWidget as the central widget
  auto *fvw = new FootageViewerWidget(this);
  SetViewerWidget(fvw);

  // Set strings
  Retranslate();

  // Show and raise on connect
  SetShowAndRaiseOnConnect();
}

void FootageViewerPanel::OverrideWorkArea(const TimeRange &r) const { GetFootageViewerWidget()->OverrideWorkArea(r); }

QVector<ViewerOutput *> FootageViewerPanel::GetSelectedFootage() const {
  QVector<ViewerOutput *> list;

  if (GetConnectedViewer()) {
    list.append(GetConnectedViewer());
  }

  return list;
}

void FootageViewerPanel::Retranslate() {
  super::Retranslate();

  SetTitle(tr("Footage Viewer"));
}

}  // namespace olive
