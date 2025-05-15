#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>

#include "node/color/colormanager/colormanager.h"
#include "render/managedcolor.h"
#include "widget/colorwheel/colorgradientwidget.h"
#include "widget/colorwheel/colorspacechooser.h"
#include "widget/colorwheel/colorswatchchooser.h"
#include "widget/colorwheel/colorvalueswidget.h"
#include "widget/colorwheel/colorwheelwidget.h"

namespace olive {

class ColorDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief ColorDialog Constructor
   *
   * @param color_manager
   *
   * The ColorManager to use for color management. This must be valid.
   *
   * @param start
   *
   * The color to start with. This must be in the color_manager's reference space
   *
   * @param input_cs
   *
   * The input range that the user should see. The start color will be converted to this for UI object.
   *
   * @param parent
   *
   * QWidget parent.
   */
  explicit ColorDialog(ColorManager* color_manager, const ManagedColor& start = ManagedColor(Color(1.0f, 1.0f, 1.0f)),
                       QWidget* parent = nullptr);

  /**
   * @brief Retrieves the color selected by the user
   *
   * The color is always returned in the ColorManager's reference space (usually scene linear).
   */
  [[nodiscard]] ManagedColor GetSelectedColor() const;

  [[nodiscard]] QString GetColorSpaceInput() const;

  [[nodiscard]] ColorTransform GetColorSpaceOutput() const;

 public slots:
  void SetColor(const ManagedColor& start);

 private:
  ColorManager* color_manager_;

  ColorWheelWidget* color_wheel_;

  ColorValuesWidget* color_values_widget_;

  ColorGradientWidget* hsv_value_gradient_;

  ColorProcessorPtr input_to_ref_processor_;

  ColorSpaceChooser* chooser_;

  ColorSwatchChooser* swatch_;

 private slots:
  void ColorSpaceChanged(const QString& input, const ColorTransform& output);
};

}  // namespace olive

#endif  // COLORDIALOG_H
