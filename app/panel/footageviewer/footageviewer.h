#ifndef FOOTAGE_VIEWER_PANEL_H
#define FOOTAGE_VIEWER_PANEL_H

#include <QOpenGLFunctions>

#include "panel/project/footagemanagementpanel.h"
#include "panel/viewer/viewerbase.h"
#include "widget/viewer/footageviewer.h"

namespace olive {

/**
 * @brief Dockable wrapper around a ViewerWidget
 */
class FootageViewerPanel : public ViewerPanelBase, public FootageManagementPanel {
  Q_OBJECT
 public:
  FootageViewerPanel();

  void OverrideWorkArea(const TimeRange &r) const;

  [[nodiscard]] FootageViewerWidget *GetFootageViewerWidget() const {
    return dynamic_cast<FootageViewerWidget *>(GetTimeBasedWidget());
  }

  [[nodiscard]] QVector<ViewerOutput *> GetSelectedFootage() const override;

 protected:
  void Retranslate() override;
};

}  // namespace olive

#endif  // FOOTAGE_VIEWER_PANEL_H
