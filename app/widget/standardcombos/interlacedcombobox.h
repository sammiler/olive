

#ifndef INTERLACEDCOMBOBOX_H
#define INTERLACEDCOMBOBOX_H

#include <QComboBox>

#include "render/videoparams.h"

namespace olive {

class InterlacedComboBox : public QComboBox {
  Q_OBJECT
 public:
  explicit InterlacedComboBox(QWidget* parent = nullptr) : QComboBox(parent) {
    // These must match the Interlacing enum in VideoParams
    this->addItem(tr("None (Progressive)"));
    this->addItem(tr("Top-Field First"));
    this->addItem(tr("Bottom-Field First"));
  }

  [[nodiscard]] VideoParams::Interlacing GetInterlaceMode() const {
    return static_cast<VideoParams::Interlacing>(this->currentIndex());
  }

  void SetInterlaceMode(VideoParams::Interlacing mode) { this->setCurrentIndex(mode); }
};

}  // namespace olive

#endif  // INTERLACEDCOMBOBOX_H
