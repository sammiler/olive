

#ifndef CHANNELLAYOUTCOMBOBOX_H
#define CHANNELLAYOUTCOMBOBOX_H

#include <olive/core/core.h>
#include <QComboBox>

#include "ui/humanstrings.h"

namespace olive {

using namespace core;

class ChannelLayoutComboBox : public QComboBox {
  Q_OBJECT
 public:
  explicit ChannelLayoutComboBox(QWidget* parent = nullptr) : QComboBox(parent) {
    foreach (const uint64_t& ch_layout, AudioParams::kSupportedChannelLayouts) {
      this->addItem(HumanStrings::ChannelLayoutToString(ch_layout), QVariant::fromValue(ch_layout));
    }
  }

  [[nodiscard]] uint64_t GetChannelLayout() const { return this->currentData().toULongLong(); }

  void SetChannelLayout(uint64_t ch) {
    for (int i = 0; i < this->count(); i++) {
      if (this->itemData(i).toULongLong() == ch) {
        this->setCurrentIndex(i);
        break;
      }
    }
  }
};

}  // namespace olive

#endif  // CHANNELLAYOUTCOMBOBOX_H
