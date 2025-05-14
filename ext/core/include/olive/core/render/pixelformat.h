/***

  Olive - Non-Linear Video Editor
  Copyright (C) 2023 Olive Studios LLC

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

#ifndef LIBOLIVECORE_PIXELFORMAT_H
#define LIBOLIVECORE_PIXELFORMAT_H

namespace olive::core {

class PixelFormat {
 public:
  enum Format { INVALID = -1, U8, U16, F16, F32, COUNT };

  explicit PixelFormat(Format f = INVALID) { f_ = f; }

  explicit operator Format() const { return f_; }

  static int byte_count(Format f) {
    switch (f) {
      case INVALID:
      case COUNT:
        break;
      case U8:
        return 1;
      case U16:
      case F16:
        return 2;
      case F32:
        return 4;
    }

    return 0;
  }

  [[nodiscard]]  int byte_count() const { return byte_count(f_); }

  static bool is_float(Format f) {
    switch (f) {
      case INVALID:
      case COUNT:
      case U8:
      case U16:
        break;
      case F16:
      case F32:
        return true;
    }

    return false;
  }

  [[nodiscard]]  bool is_float() const { return is_float(f_); }

 private:
  Format f_;
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_PIXELFORMAT_H
