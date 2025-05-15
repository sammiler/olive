

#ifndef COLORTRANSFORM_H
#define COLORTRANSFORM_H

#include <QString>

#include "common/define.h"
#include "common/ocioutils.h"

namespace olive {

class ColorTransform {
 public:
  ColorTransform() { is_display_ = false; }

  explicit ColorTransform(const QString& output) {
    is_display_ = false;
    output_ = output;
  }

  ColorTransform(const QString& display, const QString& view, const QString& look) {
    is_display_ = true;
    output_ = display;
    view_ = view;
    look_ = look;
  }

  [[nodiscard]] bool is_display() const { return is_display_; }

  [[nodiscard]] const QString& display() const { return output_; }

  [[nodiscard]] const QString& output() const { return output_; }

  [[nodiscard]] const QString& view() const { return view_; }

  [[nodiscard]] const QString& look() const { return look_; }

 private:
  QString output_;

  bool is_display_;
  QString view_;
  QString look_;
};

}  // namespace olive

#endif  // COLORTRANSFORM_H
