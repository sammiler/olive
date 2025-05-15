

#ifndef LIBOLIVECORE_VALUE_H
#define LIBOLIVECORE_VALUE_H

#include <stdint.h>
#include <string.h>
#include <map>
#include <string>
#include <vector>

namespace olive::core {

/**
 * @brief Generic type container
 */
class Value {
 public:
  enum Type {
    /// Null/no data
    NONE,

    /// Signed int64
    INT,

    /// Double-precision float
    FLOAT,

    /// UTF-8 string
    STRING
  };

  Value() { type_ = NONE; }

  explicit Value(int64_t v) {
    data_.resize(sizeof(int64_t));
    memcpy(data_.data(), &v, sizeof(int64_t));
    type_ = INT;
  }

  explicit Value(double v) {
    data_.resize(sizeof(double));
    memcpy(data_.data(), &v, sizeof(int64_t));
    type_ = FLOAT;
  }

  explicit Value(const char *s) {
    size_t sz = strlen(s);
    data_.resize(sz);
    memcpy(data_.data(), s, sz);
    type_ = STRING;
  }

  explicit Value(const std::string &s) {
    data_.resize(s.size());
    memcpy(data_.data(), s.data(), data_.size());
    type_ = STRING;
  }

 private:
  std::vector<uint8_t> data_;
  Type type_;
};

using ValueMap = std::map<std::string, Value>;

}  // namespace olive::core

#endif  // LIBOLIVECORE_VALUE_H
