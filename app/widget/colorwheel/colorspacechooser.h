#ifndef COLORSPACECHOOSER_H
#define COLORSPACECHOOSER_H

#include <QComboBox>
#include <QGroupBox>

#include "node/color/colormanager/colormanager.h"

namespace olive {

class ColorSpaceChooser : public QGroupBox {
  Q_OBJECT
 public:
  explicit ColorSpaceChooser(ColorManager* color_manager, bool enable_input_field = true,
                             bool enable_display_fields = true, QWidget* parent = nullptr);

  [[nodiscard]] QString input() const;
  [[nodiscard]] ColorTransform output() const;

  void set_input(const QString& s);
  void set_output(const ColorTransform& out);

 signals:
  void InputColorSpaceChanged(const QString& input);

  void OutputColorSpaceChanged(const ColorTransform& out);

  void ColorSpaceChanged(const QString& input, const ColorTransform& out);

 private slots:
  void UpdateViews(const QString& display);

 private:
  ColorManager* color_manager_;

  QComboBox* input_combobox_;

  QComboBox* display_combobox_;

  QComboBox* view_combobox_;

  QComboBox* look_combobox_;

 private slots:
  void ComboBoxChanged();
};

}  // namespace olive

#endif  // COLORSPACECHOOSER_H
