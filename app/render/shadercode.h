

#ifndef SHADERCODE_H
#define SHADERCODE_H

#include <utility>

#include "common/filefunctions.h"

namespace olive {

class ShaderCode {
 public:
  explicit ShaderCode(QString frag_code = QString(), QString vert_code = QString())
      : frag_code_(std::move(frag_code)), vert_code_(std::move(vert_code)) {}

  [[nodiscard]] const QString& frag_code() const { return frag_code_; }
  void set_frag_code(const QString& f) { frag_code_ = f; }

  [[nodiscard]] const QString& vert_code() const { return vert_code_; }
  void set_vert_code(const QString& v) { vert_code_ = v; }

 private:
  QString frag_code_;

  QString vert_code_;
};

}  // namespace olive

#endif  // SHADERCODE_H
