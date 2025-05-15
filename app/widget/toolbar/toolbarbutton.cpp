#include "toolbarbutton.h"

namespace olive {

ToolbarButton::ToolbarButton(QWidget *parent, const Tool::Item &tool) : QPushButton(parent), tool_(tool) {
  setCheckable(true);
}

const Tool::Item &ToolbarButton::tool() { return tool_; }

}  // namespace olive
