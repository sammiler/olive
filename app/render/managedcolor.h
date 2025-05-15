

#ifndef MANAGEDCOLOR_H
#define MANAGEDCOLOR_H

#include <olive/core/core.h>

#include "colortransform.h"

namespace olive {

class ManagedColor : public Color {
 public:
  ManagedColor();
  ManagedColor(const double& r, const double& g, const double& b, const double& a = 1.0);
  ManagedColor(const char* data, const PixelFormat& format, int channel_layout);
  explicit ManagedColor(const Color& c);

  [[nodiscard]] QString color_input() const;
  void set_color_input(const QString& color_input);

  [[nodiscard]] const ColorTransform& color_output() const;
  void set_color_output(const ColorTransform& color_output);

 private:
  QString color_input_;

  ColorTransform color_transform_;
};

}  // namespace olive

#endif  // MANAGEDCOLOR_H
