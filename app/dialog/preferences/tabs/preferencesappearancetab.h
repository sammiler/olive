

#ifndef PREFERENCESAPPEARANCETAB_H
#define PREFERENCESAPPEARANCETAB_H

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

#include "dialog/configbase/configdialogbase.h"
#include "ui/style/style.h"
#include "widget/colorlabelmenu/colorcodingcombobox.h"

namespace olive {

class PreferencesAppearanceTab : public ConfigDialogBaseTab {
  Q_OBJECT
 public:
  PreferencesAppearanceTab();

  void Accept(MultiUndoCommand* command) override;

 private:
  /**
   * @brief UI widget for selecting the current UI style
   */
  QComboBox* style_combobox_;

  QVector<ColorCodingComboBox*> color_btns_;

  ColorCodingComboBox* marker_btn_;
};

}  // namespace olive

#endif  // PREFERENCESAPPEARANCETAB_H
