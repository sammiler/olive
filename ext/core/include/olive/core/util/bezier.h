

#ifndef LIBOLIVECORE_BEZIER_H
#define LIBOLIVECORE_BEZIER_H

#include <Imath/ImathVec.h>

namespace olive::core {

class Bezier {
 public:
  Bezier();
  Bezier(double x, double y);
  Bezier(double x, double y, double cp1_x, double cp1_y, double cp2_x, double cp2_y);

  [[nodiscard]] const double &x() const { return x_; }
  [[nodiscard]] const double &y() const { return y_; }
  [[nodiscard]] const double &cp1_x() const { return cp1_x_; }
  [[nodiscard]] const double &cp1_y() const { return cp1_y_; }
  [[nodiscard]] const double &cp2_x() const { return cp2_x_; }
  [[nodiscard]] const double &cp2_y() const { return cp2_y_; }

  [[nodiscard]] Imath::V2d to_vec() const { return {x_, y_}; }

  [[nodiscard]] Imath::V2d control_point_1_to_vec() const { return {cp1_x_, cp1_y_}; }

  [[nodiscard]] Imath::V2d control_point_2_to_vec() const { return {cp2_x_, cp2_y_}; }

  void set_x(const double &x) { x_ = x; }
  void set_y(const double &y) { y_ = y; }
  void set_cp1_x(const double &cp1_x) { cp1_x_ = cp1_x; }
  void set_cp1_y(const double &cp1_y) { cp1_y_ = cp1_y; }
  void set_cp2_x(const double &cp2_x) { cp2_x_ = cp2_x; }
  void set_cp2_y(const double &cp2_y) { cp2_y_ = cp2_y; }

  static double QuadraticXtoT(double x, double a, double b, double c);

  static double QuadraticTtoY(double a, double b, double c, double t);

  static double QuadraticXtoY(double x, const Imath::V2d &a, const Imath::V2d &b, const Imath::V2d &c) {
    return QuadraticTtoY(a.y, b.y, c.y, QuadraticXtoT(x, a.x, b.x, c.x));
  }

  static double CubicXtoT(double x, double a, double b, double c, double d);

  static double CubicTtoY(double a, double b, double c, double d, double t);

  static double CubicXtoY(double x, const Imath::V2d &a, const Imath::V2d &b, const Imath::V2d &c,
                          const Imath::V2d &d) {
    return CubicTtoY(a.y, b.y, c.y, d.y, CubicXtoT(x, a.x, b.x, c.x, d.x));
  }

 private:
  static double CalculateTFromX(bool cubic, double x, double a, double b, double c, double d);

  double x_;
  double y_;

  double cp1_x_;
  double cp1_y_;

  double cp2_x_;
  double cp2_y_;
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_BEZIER_H
