#ifndef SAMPLERATECOMBOBOX_H
#define SAMPLERATECOMBOBOX_H

#include <olive/core/core.h>
#include <QComboBox>

#include "ui/humanstrings.h"

namespace olive {

using namespace core;

class SampleRateComboBox : public QComboBox {
  Q_OBJECT
 public:
  explicit SampleRateComboBox(QWidget* parent = nullptr) : QComboBox(parent) {
    foreach (int sr, AudioParams::kSupportedSampleRates) {
      this->addItem(HumanStrings::SampleRateToString(sr), sr);
    }
  }

  [[nodiscard]] int GetSampleRate() const { return this->currentData().toInt(); }

  void SetSampleRate(int rate) {
    for (int i = 0; i < this->count(); i++) {
      if (this->itemData(i).toInt() == rate) {
        this->setCurrentIndex(i);
        break;
      }
    }
  }
};

}  // namespace olive

#endif  // SAMPLERATECOMBOBOX_H
