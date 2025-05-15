

#ifndef PIXELFORMATCOMBOBOX_H
#define PIXELFORMATCOMBOBOX_H

#include <QComboBox>

#include "render/videoparams.h"

namespace olive {

class PixelFormatComboBox : public QComboBox {
  Q_OBJECT
 public:
  explicit PixelFormatComboBox(bool float_only, QWidget* parent = nullptr) : QComboBox(parent) {
    // Set up preview formats
    for (int i = 0; i < PixelFormat::COUNT; i++) {
      auto pix_fmt = PixelFormat(static_cast<PixelFormat::Format>(i));

      if (!float_only || pix_fmt.is_float()) {
        this->addItem(VideoParams::GetFormatName(pix_fmt), static_cast<PixelFormat::Format>(pix_fmt));
      }
    }
  }

  [[nodiscard]] PixelFormat GetPixelFormat() const {
    return PixelFormat(static_cast<PixelFormat::Format>(this->currentData().toInt()));
  }

  void SetPixelFormat(PixelFormat::Format fmt) {
    for (int i = 0; i < this->count(); i++) {
      if (this->itemData(i).toInt() == static_cast<int>(fmt)) {
        this->setCurrentIndex(i);
        break;
      }
    }
  }
};

}  // namespace olive

#endif  // PIXELFORMATCOMBOBOX_H
