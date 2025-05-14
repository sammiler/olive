#ifndef SEQUENCEDIALOGPARAMETERTAB_H
#define SEQUENCEDIALOGPARAMETERTAB_H

#include <QCheckBox>
#include <QComboBox>
#include <QList>
#include <QSpinBox>

#include "node/project/sequence/sequence.h"
#include "sequencepreset.h"
#include "widget/slider/integerslider.h"
#include "widget/standardcombos/standardcombos.h"

namespace olive {

class SequenceDialogParameterTab : public QWidget {
  Q_OBJECT
 public:
  explicit SequenceDialogParameterTab(Sequence* sequence, QWidget* parent = nullptr);

  [[nodiscard]] int GetSelectedVideoWidth() const { return width_slider_->GetValue(); }

  [[nodiscard]] int GetSelectedVideoHeight() const { return height_slider_->GetValue(); }

  [[nodiscard]] rational GetSelectedVideoFrameRate() const { return framerate_combo_->GetFrameRate(); }

  [[nodiscard]] rational GetSelectedVideoPixelAspect() const { return pixelaspect_combo_->GetPixelAspectRatio(); }

  [[nodiscard]] VideoParams::Interlacing GetSelectedVideoInterlacingMode() const {
    return interlacing_combo_->GetInterlaceMode();
  }

  [[nodiscard]] int GetSelectedAudioSampleRate() const { return audio_sample_rate_field_->GetSampleRate(); }

  [[nodiscard]] uint64_t GetSelectedAudioChannelLayout() const { return audio_channels_field_->GetChannelLayout(); }

  [[nodiscard]] int GetSelectedPreviewResolution() const { return preview_resolution_field_->GetDivider(); }

  [[nodiscard]] PixelFormat GetSelectedPreviewFormat() const { return preview_format_field_->GetPixelFormat(); }

  [[nodiscard]] static bool GetSelectedPreviewAutoCache() {
    // return preview_autocache_field_->isChecked();
    //  TEMP: Disable sequence auto-cache, wanna see if clip cache supersedes it.
    return false;
  }

 public slots:
  void PresetChanged(const SequencePreset& preset);

 signals:
  void SaveParametersAsPreset(const SequencePreset& preset);

 private:
  IntegerSlider* width_slider_;

  IntegerSlider* height_slider_;

  FrameRateComboBox* framerate_combo_;

  PixelAspectRatioComboBox* pixelaspect_combo_;

  InterlacedComboBox* interlacing_combo_;

  SampleRateComboBox* audio_sample_rate_field_;

  ChannelLayoutComboBox* audio_channels_field_;

  VideoDividerComboBox* preview_resolution_field_;

  QLabel* preview_resolution_label_;

  PixelFormatComboBox* preview_format_field_;

  QCheckBox* preview_autocache_field_;

 private slots:
  void SavePresetClicked();

  void UpdatePreviewResolutionLabel();
};

}  // namespace olive

#endif  // SEQUENCEDIALOGPARAMETERTAB_H
