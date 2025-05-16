#ifndef PREFERENCESAUDIOTAB_H
#define PREFERENCESAUDIOTAB_H

#include <QComboBox>    // 引入 QComboBox 类，用于创建下拉列表框
#include <QPushButton>  // 引入 QPushButton 类，用于创建按钮

#include "dialog/configbase/configdialogbase.h"  // 引入配置对话框基类相关定义
#include "dialog/export/exportaudiotab.h"        // 引入导出音频选项卡相关定义，可能用于复用某些音频设置UI
#include "dialog/export/exportformatcombobox.h"  // 引入导出格式组合框相关定义

namespace olive {

// 向前声明类，避免头文件循环依赖
class SampleRateComboBox;
class ChannelLayoutComboBox;
class SampleFormatComboBox;

/**
 * @class PreferencesAudioTab
 * @brief “首选项”对话框中的“音频”选项卡类。
 *
 * 继承自 ConfigDialogBaseTab，用于管理应用程序音频相关的设置，
 * 例如音频后端、输入输出设备、录制格式和选项等。
 */
class PreferencesAudioTab : public ConfigDialogBaseTab {
  Q_OBJECT
 public:
  /**
   * @brief PreferencesAudioTab 构造函数。
   *
   * 初始化音频选项卡的用户界面和设置。
   */
  PreferencesAudioTab();

  /**
   * @brief 应用当前选项卡中的更改。
   *
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录撤销/重做操作。
   * 此函数会覆盖基类中的同名虚函数。
   */
  void Accept(MultiUndoCommand* command) override;

 private:
  /**
   * @brief 用于选择音频后端的下拉选择框控件。
   */
  QComboBox* audio_backend_combobox_;

  /**
   * @brief 用于选择输出音频设备的下拉选择框控件。
   */
  QComboBox* audio_output_devices_;

  /**
   * @brief 用于选择输入音频设备的下拉选择框控件。
   */
  QComboBox* audio_input_devices_;

  /**
   * @brief 用于编辑录制声道的下拉选择框控件。
   */
  QComboBox* recording_combobox_{};

  /**
   * @brief 用于触发刷新可用音频设备列表的按钮。
   */
  QPushButton* refresh_devices_btn_;

  /**
   * @brief 指向输出采样率选择框的指针。
   */
  SampleRateComboBox* output_rate_combo_;
  /**
   * @brief 指向输出声道布局选择框的指针。
   */
  ChannelLayoutComboBox* output_ch_layout_combo_;
  /**
   * @brief 指向输出采样格式选择框的指针。
   */
  SampleFormatComboBox* output_fmt_combo_;

  /**
   * @brief 指向录制格式选择框的指针。
   */
  ExportFormatComboBox* record_format_combo_;

  /**
   * @brief 指向录制选项界面的指针，可能复用了导出音频的选项卡。
   */
  ExportAudioTab* record_options_;

 private slots:
  /**
   * @brief 槽函数：刷新音频后端列表。
   */
  void RefreshBackends();

  /**
   * @brief 槽函数：刷新音频设备列表。
   */
  void RefreshDevices();

  /**
   * @brief 槽函数：强制刷新音频后端列表。
   * 与 RefreshBackends 的具体区别需要查看实现。
   */
  void HardRefreshBackends();

  /**
   * @brief 槽函数：尝试从配置中设置音频设备。
   */
  void AttemptToSetDevicesFromConfig();
};

}  // namespace olive

#endif  // PREFERENCESAUDIOTAB_H