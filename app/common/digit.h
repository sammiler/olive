

#ifndef DIGIT_H
#define DIGIT_H

#include <cstdint>

namespace olive {

inline int64_t GetDigitCount(int64_t input) {
  input = std::abs(input);

  int64_t lim = 10;
  int64_t digit = 1;

  while (input >= lim) {
    lim *= 10;
    digit++;
  }

  return digit;
}

}  // namespace olive

#endif  // DIGIT_H
