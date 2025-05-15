#ifndef VIEWER_PANEL_H
#define VIEWER_PANEL_H

#include <QOpenGLFunctions>

#include "viewerbase.h"

namespace olive {

/**
 * @brief Dockable wrapper around a ViewerWidget
 */
class ViewerPanel : public ViewerPanelBase {
  Q_OBJECT
 public:
  explicit ViewerPanel(const QString& object_name);

 protected:
  void Retranslate() override;
};

}  // namespace olive

#endif  // VIEWER_PANEL_H
