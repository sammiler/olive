

#include "util/stringutils.h"

#include <cstdarg>
#include <cstring>
#include <stdexcept>

namespace olive::core {

std::vector<std::string> StringUtils::split(const std::string &s, char separator) {
  std::vector<std::string> output;

  std::string::size_type prev_pos = 0, pos = 0;

  while ((pos = s.find(separator, pos)) != std::string::npos) {
    std::string substring(s.substr(prev_pos, pos - prev_pos));

    output.push_back(substring);

    prev_pos = ++pos;
  }

  output.push_back(s.substr(prev_pos, pos - prev_pos));  // Last word

  return output;
}

std::vector<std::string> StringUtils::split_regex(const std::string &s, const std::regex &regex) {
  std::vector<std::string> output;

  std::sregex_token_iterator iter(s.begin(), s.end(), regex, -1);
  std::sregex_token_iterator end;
  for (; iter != end; iter++) {
    output.push_back(*iter);
  }

  return output;
}

int StringUtils::to_int(const std::string &s, int base, bool *ok) {
  try {
    int x = std::stoi(s, nullptr, base);
    if (ok) {
      *ok = true;
    }
    return x;
  } catch (const std::invalid_argument &e) {
    if (ok) {
      *ok = false;
    }
    return 0;
  }
}

std::string StringUtils::format(const char *fmt, ...) {
  va_list ap1, ap2;
  va_start(ap1, fmt);

  // Need to duplicate because we call vsnprintf twice and it consumes the va_list each time
  va_copy(ap2, ap1);

  int s = std::vsnprintf(nullptr, 0, fmt, ap1);

  // Create string with size, adding 1 because vsnprintf will want to write a null terminator
  std::string r;
  s++;
  r.resize(s);

  // Write into string
  std::vsnprintf(r.data(), s, fmt, ap2);

  // Pop null terminator
  r.pop_back();

  va_end(ap2);
  va_end(ap1);

  return r;
}

}  // namespace olive::core
