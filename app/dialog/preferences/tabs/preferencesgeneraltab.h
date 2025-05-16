#ifndef PREFERENCESGENERALTAB_H
#define PREFERENCESGENERALTAB_H

#include <QCheckBox>  // 引入 QCheckBox 类，用于创建复选框控件
#include <QComboBox>  // 引入 QComboBox 类，用于创建下拉列表框控件
#include <QSpinBox>   // 引入 QSpinBox 类，用于创建整数输入框控件

#include "dialog/configbase/configdialogbase.h"  // 引入配置对话框基类相关定义
#include "node/project/sequence/sequence.h"      // 引入序列相关的定义，可能用于某些默认设置
#include "widget/slider/integerslider.h"         // 引入整数滑块控件
#include "widget/slider/rationalslider.h"        // 引入有理数滑块控件

namespace olive {

/**
 * @class PreferencesGeneralTab
 * @brief “首选项”对话框中的“常规”选项卡类。
 *
 * 继承自 ConfigDialogBaseTab，用于管理应用程序的常规设置，
 * 例如语言、自动滚动方式、波形显示、默认静止图像时长以及自动恢复功能等。
 */
class PreferencesGeneralTab : public ConfigDialogBaseTab {
  Q_OBJECT
 public:
  /**
   * @brief PreferencesGeneralTab 构造函数。
   *
   * 初始化常规选项卡的用户界面和各项设置。
   */
  PreferencesGeneralTab();

  /**
   * @brief 应用当前选项卡中的更改。
   *
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录撤销/重做操作。
   * 此函数会覆盖基类中的同名虚函数。
   */
  void Accept(MultiUndoCommand* command) override;

 private:
  /**
   * @brief 向语言选择下拉框中添加一个语言选项。
   *
   * @param locale_name 语言的区域设置名称 (例如 "en_US", "zh_CN")。
   */
  void AddLanguage(const QString& locale_name);

  /**
   * @brief 指向语言选择下拉框控件的指针。
   */
  QComboBox* language_combobox_;

  /**
   * @brief 指向自动滚动方式选择下拉框控件的指针。
   */
  QComboBox* autoscroll_method_;

  /**
   * @brief 指向是否启用校正波形显示的复选框控件的指针。
   */
  QCheckBox* rectified_waveforms_;

  /**
   * @brief 指向默认静止图像长度设置滑块控件的指针 (使用有理数表示)。
   */
  RationalSlider* default_still_length_;

  /**
   * @brief 指向是否启用自动恢复功能的复选框控件的指针。
   */
  QCheckBox* autorecovery_enabled_;

  /**
   * @brief 指向自动恢复时间间隔设置滑块控件的指针 (整数)。
   */
  IntegerSlider* autorecovery_interval_;

  /**
   * @brief 指向自动恢复最大备份数量设置滑块控件的指针 (整数)。
   */
  IntegerSlider* autorecovery_maximum_;
};

}  // namespace olive

#endif  // PREFERENCESGENERALTAB_H