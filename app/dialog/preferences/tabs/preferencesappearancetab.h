#ifndef PREFERENCESAPPEARANCETAB_H
#define PREFERENCESAPPEARANCETAB_H

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

#include "dialog/configbase/configdialogbase.h" // 引入配置对话框基类相关定义
#include "ui/style/style.h"                     // 引入界面样式相关定义
#include "widget/colorlabelmenu/colorcodingcombobox.h" // 引入颜色编码组合框控件

namespace olive {

/**
 * @class PreferencesAppearanceTab
 * @brief “首选项”对话框中的“外观”选项卡类。
 *
 * 继承自 ConfigDialogBaseTab，用于管理应用程序外观相关的设置。
 */
class PreferencesAppearanceTab : public ConfigDialogBaseTab {
  Q_OBJECT
 public:
  /**
   * @brief PreferencesAppearanceTab 构造函数。
   *
   * 初始化外观选项卡的用户界面和设置。
   */
  PreferencesAppearanceTab();

  /**
   * @brief 应用当前选项卡中的更改。
   *
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录撤销/重做操作。
   * 此函数会覆盖基类中的同名虚函数。
   */
  void Accept(MultiUndoCommand* command) override;

private:
  /**
   * @brief 用于选择当前用户界面样式的下拉选择框控件。
   */
  QComboBox* style_combobox_;

  /**
   * @brief 存储颜色编码按钮（ColorCodingComboBox）的 QVector 容器。
   *
   * 可能用于管理界面中多个元素的颜色设置。
   */
  QVector<ColorCodingComboBox*> color_btns_;

  /**
   * @brief 指向标记颜色按钮（ColorCodingComboBox）的指针。
   *
   * 可能用于设置时间轴或其他界面元素的标记颜色。
   */
  ColorCodingComboBox* marker_btn_;
};

}  // namespace olive

#endif  // PREFERENCESAPPEARANCETAB_H