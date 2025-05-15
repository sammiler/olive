#include "viewer.h"

namespace olive {

ViewerPanel::ViewerPanel(const QString& object_name) : ViewerPanelBase(object_name) {
  // Set ViewerWidget as the central widget
  auto* vw = new ViewerWidget(this);
  SetViewerWidget(vw);

  // Set strings
  Retranslate();
}

void ViewerPanel::Retranslate() {
  ViewerPanelBase::Retranslate();

  SetTitle(tr("Viewer"));
}

}  // namespace olive
