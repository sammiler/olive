#ifndef TOOLBARBUTTON_H
#define TOOLBARBUTTON_H

#include <QPushButton>

#include "tool/tool.h"

namespace olive {

/**
 * @brief Simple derived class of QPushButton to contain an Tool ID. Used as the main widget through Toolbar.
 */
class ToolbarButton : public QPushButton {
 public:
  /**
   * @brief ToolbarButton Constructor
   *
   * @param parent
   *
   * QWidget parent. Almost always an instance of Toolbar.
   *
   * @param tool
   *
   * Tool object. Must be a member of enum Tool::Item, including kNone if this button does not represent a tool.
   */
  ToolbarButton(QWidget* parent, const Tool::Item& tool);

  /**
   * @brief Retrieve tool ID that this button represents
   *
   * Set in the constructor and shouldn't change throughout its lifetime.
   */
  const Tool::Item& tool();

 private:
  /**
   * @brief Internal tool value
   */
  Tool::Item tool_;
};

}  // namespace olive

#endif  // TOOLBARBUTTON_H
