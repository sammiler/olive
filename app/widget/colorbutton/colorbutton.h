

#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

#include "node/color/colormanager/colormanager.h"
#include "render/managedcolor.h"

namespace olive {

class ColorButton : public QPushButton {
  Q_OBJECT
 public:
  ColorButton(ColorManager* color_manager, bool show_dialog_on_click, QWidget* parent = nullptr);
  explicit ColorButton(ColorManager* color_manager, QWidget* parent = nullptr)
      : ColorButton(color_manager, true, parent) {}

  [[nodiscard]] const ManagedColor& GetColor() const;

 public slots:
  void SetColor(const ManagedColor& c);

 signals:
  void ColorChanged(const ManagedColor& c);

 private slots:
  void ShowColorDialog();

  void ColorDialogFinished(int e);

 private:
  void UpdateColor();

  ColorManager* color_manager_;

  ManagedColor color_;

  ColorProcessorPtr color_processor_;

  bool dialog_open_;
};

}  // namespace olive

#endif  // COLORBUTTON_H
