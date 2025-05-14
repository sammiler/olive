/***

  Olive - Non-Linear Video Editor
  Copyright (C) 2022 Olive Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

***/

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

  [[nodiscard]] PixelFormat GetPixelFormat() const { return PixelFormat(static_cast<PixelFormat::Format>(this->currentData().toInt())); }

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
