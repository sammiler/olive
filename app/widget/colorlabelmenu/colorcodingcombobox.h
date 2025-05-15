

#ifndef COLORCODINGCOMBOBOX_H
#define COLORCODINGCOMBOBOX_H

#include <QComboBox>

#include "widget/colorlabelmenu/colorlabelmenu.h"

namespace olive {

class ColorCodingComboBox : public QComboBox {
  Q_OBJECT
 public:
  explicit ColorCodingComboBox(QWidget* parent = nullptr);

  void showPopup() override;

  void SetColor(int index);

  [[nodiscard]] int GetSelectedColor() const { return index_; }

 private:
  int index_{};
};

}  // namespace olive

#endif  // COLORCODINGCOMBOBOX_H
