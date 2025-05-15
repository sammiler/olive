

#ifndef SAMPLEFORMATCOMBOBOX_H
#define SAMPLEFORMATCOMBOBOX_H

#include <olive/core/core.h>
#include <QComboBox>

#include "ui/humanstrings.h"

namespace olive {

using namespace core;

class SampleFormatComboBox : public QComboBox {
  Q_OBJECT
 public:
  explicit SampleFormatComboBox(QWidget *parent = nullptr) : QComboBox(parent), attempt_to_restore_format_(true) {}

  void SetAttemptToRestoreFormat(bool e) { attempt_to_restore_format_ = e; }

  void SetAvailableFormats(const std::vector<SampleFormat> &formats) {
    auto tmp = SampleFormat(SampleFormat::INVALID);

    if (attempt_to_restore_format_) {
      tmp = GetSampleFormat();
    }

    clear();
    foreach (const SampleFormat &of, formats) {
      AddFormatItem(of);
    }

    if (attempt_to_restore_format_) {
      SetSampleFormat(tmp);
    }
  }

  void SetPackedFormats() {
    auto tmp = SampleFormat(SampleFormat::INVALID);

    if (attempt_to_restore_format_) {
      tmp = GetSampleFormat();
    }

    clear();
    for (int i = SampleFormat::PACKED_START; i < SampleFormat::PACKED_END; i++) {
      AddFormatItem(SampleFormat(static_cast<SampleFormat::Format>(i)));
    }

    if (attempt_to_restore_format_) {
      SetSampleFormat(tmp);
    }
  }

  [[nodiscard]] SampleFormat GetSampleFormat() const {
    return SampleFormat(static_cast<SampleFormat::Format>(this->currentData().toInt()));
  }

  void SetSampleFormat(SampleFormat fmt) {
    for (int i = 0; i < this->count(); i++) {
      if (this->itemData(i).toInt() == static_cast<SampleFormat::Format>(fmt)) {
        this->setCurrentIndex(i);
        break;
      }
    }
  }

 private:
  void AddFormatItem(SampleFormat f) {
    this->addItem(HumanStrings::FormatToString(f), static_cast<SampleFormat::Format>(f));
  }

  bool attempt_to_restore_format_;
};

}  // namespace olive

#endif  // SAMPLEFORMATCOMBOBOX_H
