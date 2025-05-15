#ifndef COLORSWATCHCHOOSER_H
#define COLORSWATCHCHOOSER_H

#include "node/color/colormanager/colormanager.h"
#include "widget/colorbutton/colorbutton.h"

namespace olive {

class ColorSwatchChooser : public QWidget {
  Q_OBJECT
 public:
  explicit ColorSwatchChooser(ColorManager *manager, QWidget *parent = nullptr);

 public slots:
  void SetCurrentColor(const Color &c) { current_ = ManagedColor(c); }

 signals:
  void ColorClicked(const ManagedColor &c);

 private:
  void SetDefaultColor(int index);

  static QString GetSwatchFilename();

  void LoadSwatches();
  void SaveSwatches();

  static const int kRowCount = 4;
  static const int kColCount = 8;
  static const int kBtnCount = kRowCount * kColCount;
  ColorButton *buttons_[kBtnCount]{};

  ManagedColor current_;
  ColorButton *menu_btn_{};

 private slots:
  void HandleButtonClick();

  void HandleContextMenu();

  void SaveCurrentColor();

  void ResetMenuButton();
};

}  // namespace olive

#endif  // COLORSWATCHCHOOSER_H
