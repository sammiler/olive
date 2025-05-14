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

#ifndef LIBOLIVECORE_SAMPLEFORMAT_H
#define LIBOLIVECORE_SAMPLEFORMAT_H

#include <stdexcept>
#include <string>

namespace olive::core {

class SampleFormat {
 public:
  enum Format {
    INVALID = -1,

    U8P,
    S16P,
    S32P,
    S64P,
    F32P,
    F64P,

    U8,
    S16,
    S32,
    S64,
    F32,
    F64,

    COUNT,

    PLANAR_START = U8P,
    PACKED_START = U8,
    PLANAR_END = PACKED_START,
    PACKED_END = COUNT,
  };

  explicit SampleFormat(Format f = INVALID) { f_ = f; }

  explicit operator Format() const { return f_; }

  static int byte_count(Format f) {
    switch (f) {
      case U8:
      case U8P:
        return 1;
      case S16:
      case S16P:
        return 2;
      case S32:
      case F32:
      case S32P:
      case F32P:
        return 4;
      case S64:
      case F64:
      case S64P:
      case F64P:
        return 8;
      case INVALID:
      case COUNT:
        break;
    }

    return 0;
  }

  [[nodiscard]] int byte_count() const { return byte_count(f_); }

  static std::string to_string(Format f) {
    switch (f) {
      case INVALID:
      case COUNT:
        break;
      case U8:
        return "u8";
      case S16:
        return "s16";
      case S32:
        return "s32";
      case S64:
        return "s64";
      case F32:
        return "f32";
      case F64:
        return "f64";
      case U8P:
        return "u8p";
      case S16P:
        return "s16p";
      case S32P:
        return "s32p";
      case S64P:
        return "s64p";
      case F32P:
        return "f32p";
      case F64P:
        return "f64p";
    }

    return "";
  }

  [[nodiscard]] std::string to_string() const { return to_string(f_); }

  static SampleFormat from_string(const std::string &s) {
    if (s == "u8") {
      return SampleFormat(U8);
    } else if (s == "s16") {
      return SampleFormat(S16);
    } else if (s == "s32") {
      return SampleFormat(S32);
    } else if (s == "s64") {
      return SampleFormat(S64);
    } else if (s == "f32") {
      return SampleFormat(F32);
    } else if (s == "f64") {
      return SampleFormat(F64);
    } else if (s == "u8p") {
      return SampleFormat(U8P);
    } else if (s == "s16p") {
      return SampleFormat(S16P);
    } else if (s == "s32p") {
      return SampleFormat(S32P);
    } else if (s == "s64p") {
      return SampleFormat(S64P);
    } else if (s == "f32p") {
      return SampleFormat(F32P);
    } else if (s == "f64p") {
      return SampleFormat(F64P);
    } else {
      // Deprecated: sample formats used to be serialized as an integer. Handle that here, but we'll
      //             probably remove that eventually.
      try {
        int i = std::stoi(s);
        if (i > INVALID && i < COUNT) {
          return SampleFormat(static_cast<Format>(i));
        }
      } catch (const std::invalid_argument &e) {
      }

      // Failed to deserialize from string
      return SampleFormat(INVALID);
    }
  }

  static bool is_packed(Format f) { return f >= PACKED_START && f < PACKED_END; }

  [[nodiscard]] bool is_packed() const { return is_packed(f_); }

  static bool is_planar(Format f) { return f >= PLANAR_START && f < PLANAR_END; }

  [[nodiscard]] bool is_planar() const { return is_planar(f_); }

  static SampleFormat to_packed_equivalent(SampleFormat fmt) {
    switch (static_cast<Format>(fmt)) {
      // For packed input, just return input
      case U8:
      case S16:
      case S32:
      case S64:
      case F32:
      case F64:
        return fmt;

      // Convert to packed
      case U8P:
        return SampleFormat(U8);
      case S16P:
        return SampleFormat(S16);
      case S32P:
        return SampleFormat(S32);
      case S64P:
        return SampleFormat(S64);
      case F32P:
        return SampleFormat(F32);
      case F64P:
        return SampleFormat(F64);

      case INVALID:
      case COUNT:
        break;
    }

    return SampleFormat(INVALID);
  }

  [[nodiscard]] SampleFormat to_packed_equivalent() const { return to_packed_equivalent(SampleFormat(f_)); }

  static SampleFormat to_planar_equivalent(SampleFormat fmt) {
    switch (static_cast<Format>(fmt)) {
      // Convert to planar
      case U8:
        return SampleFormat(U8P);
      case S16:
        return SampleFormat(S16P);
      case S32:
        return SampleFormat(S32P);
      case S64:
        return SampleFormat(S64P);
      case F32:
        return SampleFormat(F32P);
      case F64:
        return SampleFormat(F64P);

      // For planar input, just return input
      case U8P:
      case S16P:
      case S32P:
      case S64P:
      case F32P:
      case F64P:
        return fmt;

      case INVALID:
      case COUNT:
        break;
    }

    return SampleFormat(INVALID);
  }

  [[nodiscard]] SampleFormat to_planar_equivalent() const { return to_planar_equivalent(SampleFormat(f_)); }

 private:
  Format f_;
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_SAMPLEFORMAT_H
