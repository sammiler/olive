#ifndef TOOL_PANEL_H
#define TOOL_PANEL_H

#include "panel/panel.h"

namespace olive {

/**
 * @brief A PanelWidget wrapper around a Toolbar
 */
class ToolPanel : public PanelWidget {
  Q_OBJECT
 public:
  ToolPanel();

 private:
  void Retranslate() override;
};

}  // namespace olive

#endif  // TOOL_PANEL_H
