

#include "managedcolor.h"

namespace olive {

ManagedColor::ManagedColor() = default;

ManagedColor::ManagedColor(const double &r, const double &g, const double &b, const double &a) : Color(r, g, b, a) {}

ManagedColor::ManagedColor(const char *data, const PixelFormat &format, int channel_layout)
    : Color(data, format, channel_layout) {}

ManagedColor::ManagedColor(const Color &c) : Color(c) {}

QString ManagedColor::color_input() const { return color_input_; }

void ManagedColor::set_color_input(const QString &color_input) { color_input_ = color_input; }

const ColorTransform &ManagedColor::color_output() const { return color_transform_; }

void ManagedColor::set_color_output(const ColorTransform &color_output) { color_transform_ = color_output; }

}  // namespace olive
