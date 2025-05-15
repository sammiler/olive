

#include "util/color.h"

#include <Imath/half.h>
#include <cmath>
#include <cstdint>

namespace olive::core {

Color Color::fromHsv(const DataType &h, const DataType &s, const DataType &v) {
  DataType C = s * v;
  DataType X = C * (1.0 - std::abs(std::fmod(h / 60.0, 2.0) - 1.0));
  DataType m = v - C;
  DataType Rs, Gs, Bs;

  if (h >= 0.0 && h < 60.0) {
    Rs = C;
    Gs = X;
    Bs = 0.0;
  } else if (h >= 60.0 && h < 120.0) {
    Rs = X;
    Gs = C;
    Bs = 0.0;
  } else if (h >= 120.0 && h < 180.0) {
    Rs = 0.0;
    Gs = C;
    Bs = X;
  } else if (h >= 180.0 && h < 240.0) {
    Rs = 0.0;
    Gs = X;
    Bs = C;
  } else if (h >= 240.0 && h < 300.0) {
    Rs = X;
    Gs = 0.0;
    Bs = C;
  } else {
    Rs = C;
    Gs = 0.0;
    Bs = X;
  }

  return Color(Rs + m, Gs + m, Bs + m);
}

Color::Color(const char *data, const PixelFormat &format, int ch_layout) { *this = fromData(data, format, ch_layout); }

void Color::toHsv(DataType *hue, DataType *sat, DataType *val) const {
  DataType fCMax = std::max(std::max(red(), green()), blue());
  DataType fCMin = std::min(std::min(red(), green()), blue());
  DataType fDelta = fCMax - fCMin;

  if (fDelta > 0) {
    if (fCMax == red()) {
      *hue = 60 * (fmod(((green() - blue()) / fDelta), 6));
    } else if (fCMax == green()) {
      *hue = 60 * (((blue() - red()) / fDelta) + 2);
    } else if (fCMax == blue()) {
      *hue = 60 * (((red() - green()) / fDelta) + 4);
    }

    if (fCMax > 0) {
      *sat = fDelta / fCMax;
    } else {
      *sat = 0;
    }

    *val = fCMax;
  } else {
    *hue = 0;
    *sat = 0;
    *val = fCMax;
  }

  if (*hue < 0) {
    *hue = 360 + *hue;
  }
}

Color::DataType Color::hsv_hue() const {
  DataType h, s, v;
  toHsv(&h, &s, &v);
  return h;
}

Color::DataType Color::hsv_saturation() const {
  DataType h, s, v;
  toHsv(&h, &s, &v);
  return s;
}

Color::DataType Color::value() const {
  DataType h, s, v;
  toHsv(&h, &s, &v);
  return v;
}

void Color::toHsl(DataType *hue, DataType *sat, DataType *lightness) const {
  DataType fCMin = std::min(red(), std::min(green(), blue()));
  DataType fCMax = std::max(red(), std::max(green(), blue()));

  *lightness = 0.5 * (fCMin + fCMax);

  if (fCMin == fCMax) {
    *sat = 0;
    *hue = 0;
    return;

  } else if (*lightness < 0.5) {
    *sat = (fCMax - fCMin) / (fCMax + fCMin);
  } else {
    *sat = (fCMax - fCMin) / (2.0 - fCMax - fCMin);
  }

  if (fCMax == red()) {
    *hue = 60 * (green() - blue()) / (fCMax - fCMin);
  }
  if (fCMax == green()) {
    *hue = 60 * (blue() - red()) / (fCMax - fCMin) + 120;
  }
  if (fCMax == blue()) {
    *hue = 60 * (red() - green()) / (fCMax - fCMin) + 240;
  }
  if (*hue < 0) {
    *hue = *hue + 360;
  }
}

Color::DataType Color::hsl_hue() const {
  DataType h, s, l;
  toHsl(&h, &s, &l);
  return h;
}

Color::DataType Color::hsl_saturation() const {
  DataType h, s, l;
  toHsl(&h, &s, &l);
  return s;
}

Color::DataType Color::lightness() const {
  DataType h, s, l;
  toHsl(&h, &s, &l);
  return l;
}

void Color::toData(char *out, const PixelFormat &format, unsigned int nb_channels) const {
  unsigned int count = std::min(RGBA, nb_channels);

  for (unsigned int i = 0; i < count; i++) {
    DataType f = data_[i];

    switch (static_cast<PixelFormat::Format>(format)) {
      case PixelFormat::INVALID:
      case PixelFormat::COUNT:
        break;
      case PixelFormat::U8:
        reinterpret_cast<uint8_t *>(out)[i] = f * 255.0;
        break;
      case PixelFormat::U16:
        reinterpret_cast<uint16_t *>(out)[i] = f * 65535.0;
        break;
      case PixelFormat::F16:
        reinterpret_cast<Imath::half *>(out)[i] = f;
        break;
      case PixelFormat::F32:
        reinterpret_cast<float *>(out)[i] = f;
        break;
    }
  }
}

Color Color::fromData(const char *in, const PixelFormat &format, unsigned int nb_channels) {
  Color c;

  unsigned int count = std::min(RGBA, nb_channels);

  for (unsigned int i = 0; i < count; i++) {
    DataType &f = c.data_[i];

    switch (static_cast<PixelFormat::Format>(format)) {
      case PixelFormat::INVALID:
      case PixelFormat::COUNT:
        break;
      case PixelFormat::U8:
        f = DataType(reinterpret_cast<const uint8_t *>(in)[i]) / 255.0;
        break;
      case PixelFormat::U16:
        f = DataType(reinterpret_cast<const uint16_t *>(in)[i]) / 65535.0;
        break;
      case PixelFormat::F16:
        f = DataType(reinterpret_cast<const Imath::half *>(in)[i]);
        break;
      case PixelFormat::F32:
        f = DataType(reinterpret_cast<const float *>(in)[i]);
        break;
    }
  }

  return c;
}

Color::DataType Color::GetRoughLuminance() const { return (2 * red() + blue() + 3 * green()) / 6.0; }

Color &Color::operator+=(const Color &rhs) {
  for (int i = 0; i < RGBA; i++) {
    data_[i] += rhs.data_[i];
  }

  return *this;
}

Color &Color::operator-=(const Color &rhs) {
  for (int i = 0; i < RGBA; i++) {
    data_[i] -= rhs.data_[i];
  }

  return *this;
}

Color &Color::operator+=(const DataType &rhs) {
  for (float &i : data_) {
    i += rhs;
  }

  return *this;
}

Color &Color::operator-=(const DataType &rhs) {
  for (float &i : data_) {
    i -= rhs;
  }

  return *this;
}

Color &Color::operator*=(const DataType &rhs) {
  for (float &i : data_) {
    i *= rhs;
  }

  return *this;
}

Color &Color::operator/=(const DataType &rhs) {
  for (float &i : data_) {
    i /= rhs;
  }

  return *this;
}

}  // namespace olive::core
