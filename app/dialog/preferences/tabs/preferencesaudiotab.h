

#ifndef PREFERENCESAUDIOTAB_H
#define PREFERENCESAUDIOTAB_H

#include <QComboBox>
#include <QPushButton>

#include "dialog/configbase/configdialogbase.h"
#include "dialog/export/exportaudiotab.h"
#include "dialog/export/exportformatcombobox.h"

namespace olive {

class PreferencesAudioTab : public ConfigDialogBaseTab {
  Q_OBJECT
 public:
  PreferencesAudioTab();

  void Accept(MultiUndoCommand* command) override;

 private:
  QComboBox* audio_backend_combobox_;

  /**
   * @brief UI widget for selecting the output audio device
   */
  QComboBox* audio_output_devices_;

  /**
   * @brief UI widget for selecting the input audio device
   */
  QComboBox* audio_input_devices_;

  /**
   * @brief UI widget for editing the recording channels
   */
  QComboBox* recording_combobox_{};

  /**
   * @brief Button that triggers a refresh of the available audio devices
   */
  QPushButton* refresh_devices_btn_;

  SampleRateComboBox* output_rate_combo_;
  ChannelLayoutComboBox* output_ch_layout_combo_;
  SampleFormatComboBox* output_fmt_combo_;

  ExportFormatComboBox* record_format_combo_;

  ExportAudioTab* record_options_;

 private slots:
  void RefreshBackends();

  void RefreshDevices();

  void HardRefreshBackends();

  void AttemptToSetDevicesFromConfig();
};

}  // namespace olive

#endif  // PREFERENCESAUDIOTAB_H
