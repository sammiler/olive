

#ifndef VIDEODIVIDERCOMBOBOX_H
#define VIDEODIVIDERCOMBOBOX_H

#include <QComboBox>

#include "render/videoparams.h"

namespace olive {

class VideoDividerComboBox : public QComboBox {
  Q_OBJECT
 public:
  explicit VideoDividerComboBox(QWidget* parent = nullptr) : QComboBox(parent) {
    foreach (int d, VideoParams::kSupportedDividers) {
      this->addItem(VideoParams::GetNameForDivider(d), d);
    }
  }

  [[nodiscard]] int GetDivider() const { return this->currentData().toInt(); }

  void SetDivider(int d) {
    for (int i = 0; i < this->count(); i++) {
      if (this->itemData(i).toInt() == d) {
        this->setCurrentIndex(i);
        break;
      }
    }
  }
};

}  // namespace olive

#endif  // VIDEODIVIDERCOMBOBOX_H
