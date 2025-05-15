

#ifndef EXPORTAUDIOTAB_H
#define EXPORTAUDIOTAB_H

#include <QComboBox>
#include <QWidget>

#include "codec/exportformat.h"
#include "common/define.h"
#include "widget/slider/integerslider.h"
#include "widget/standardcombos/standardcombos.h"

namespace olive {

class ExportAudioTab : public QWidget {
  Q_OBJECT
 public:
  explicit ExportAudioTab(QWidget* parent = nullptr);

  [[nodiscard]] ExportCodec::Codec GetCodec() const {
    return static_cast<ExportCodec::Codec>(codec_combobox_->currentData().toInt());
  }

  void SetCodec(ExportCodec::Codec c) {
    for (int i = 0; i < codec_combobox_->count(); i++) {
      if (codec_combobox_->itemData(i) == c) {
        codec_combobox_->setCurrentIndex(i);
        break;
      }
    }
  }

  [[nodiscard]] SampleRateComboBox* sample_rate_combobox() const { return sample_rate_combobox_; }

  [[nodiscard]] SampleFormatComboBox* sample_format_combobox() const { return sample_format_combobox_; }

  [[nodiscard]] ChannelLayoutComboBox* channel_layout_combobox() const { return channel_layout_combobox_; }

  [[nodiscard]] IntegerSlider* bit_rate_slider() const { return bit_rate_slider_; }

 public slots:
  int SetFormat(ExportFormat::Format format);

 private:
  ExportFormat::Format fmt_;
  QComboBox* codec_combobox_;
  SampleRateComboBox* sample_rate_combobox_;
  ChannelLayoutComboBox* channel_layout_combobox_;
  SampleFormatComboBox* sample_format_combobox_;
  IntegerSlider* bit_rate_slider_;

  static const int kDefaultBitRate;

 private slots:
  void UpdateSampleFormats();

  void UpdateBitRateEnabled();
};

}  // namespace olive

#endif  // EXPORTAUDIOTAB_H
