#ifndef EXPORTSUBTITLESTAB_H
#define EXPORTSUBTITLESTAB_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

#include "codec/exportformat.h"
#include "common/qtutils.h"
#include "dialog/export/exportformatcombobox.h"

namespace olive {

class ExportSubtitlesTab : public QWidget {
  Q_OBJECT
 public:
  explicit ExportSubtitlesTab(QWidget *parent = nullptr);

  [[nodiscard]] bool GetSidecarEnabled() const { return sidecar_checkbox_->isChecked(); }
  void SetSidecarEnabled(bool e) { sidecar_checkbox_->setEnabled(e); }

  [[nodiscard]] ExportFormat::Format GetSidecarFormat() const { return sidecar_format_combobox_->GetFormat(); }
  void SetSidecarFormat(ExportFormat::Format f) { sidecar_format_combobox_->SetFormat(f); }

  int SetFormat(ExportFormat::Format format);

  ExportCodec::Codec GetSubtitleCodec() {
    return static_cast<ExportCodec::Codec>(codec_combobox_->currentData().toInt());
  }

  void SetSubtitleCodec(ExportCodec::Codec c) { QtUtils::SetComboBoxData(codec_combobox_, c); }

 private:
  QCheckBox *sidecar_checkbox_;

  QLabel *sidecar_format_label_;
  ExportFormatComboBox *sidecar_format_combobox_;

  QComboBox *codec_combobox_;
};

}  // namespace olive

#endif  // EXPORTSUBTITLESTAB_H
