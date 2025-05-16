#ifndef LIBOLIVECORE_BEZIER_H
#define LIBOLIVECORE_BEZIER_H

#include <Imath/ImathVec.h> // 引入 Imath 库的向量类 (Imath::V2d)，用于二维点和向量操作

namespace olive::core { // Olive 核心功能命名空间

/**
 * @brief 表示一个贝塞尔曲线上的点及其控制点。
 *
 * Bezier 类存储一个锚点 (x, y) 以及两个控制点 (cp1_x, cp1_y) 和 (cp2_x, cp2_y)。
 * 这些点共同定义了一条三次贝塞尔曲线段的一部分，或者如果只使用一个控制点，
 * 则可以表示二次贝塞尔曲线。
 * 此类还提供了用于计算二次和三次贝塞尔曲线上特定值的静态方法。
 */
class Bezier {
 public:
  /**
   * @brief 默认构造函数。
   *
   * 初始化所有坐标（锚点和控制点）为0.0。
   */
  Bezier();

  /**
   * @brief 构造函数，仅初始化锚点坐标。
   *
   * 控制点坐标将默认为与锚点相同，或根据实现保持未初始化/零值。
   * @param x 锚点的 x 坐标。
   * @param y 锚点的 y 坐标。
   */
  Bezier(double x, double y);

  /**
   * @brief 构造函数，初始化锚点和两个控制点的坐标。
   * @param x 锚点的 x 坐标。
   * @param y 锚点的 y 坐标。
   * @param cp1_x 第一个控制点的 x 坐标。
   * @param cp1_y 第一个控制点的 y 坐标。
   * @param cp2_x 第二个控制点的 x 坐标。
   * @param cp2_y 第二个控制点的 y 坐标。
   */
  Bezier(double x, double y, double cp1_x, double cp1_y, double cp2_x, double cp2_y);

  /**
   * @brief 获取锚点的 x 坐标。
   * @return 返回锚点 x 坐标的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const double &x() const { return x_; }

  /**
   * @brief 获取锚点的 y 坐标。
   * @return 返回锚点 y 坐标的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const double &y() const { return y_; }

  /**
   * @brief 获取第一个控制点的 x 坐标。
   * @return 返回第一个控制点 x 坐标的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const double &cp1_x() const { return cp1_x_; }

  /**
   * @brief 获取第一个控制点的 y 坐标。
   * @return 返回第一个控制点 y 坐标的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const double &cp1_y() const { return cp1_y_; }

  /**
   * @brief 获取第二个控制点的 x 坐标。
   * @return 返回第二个控制点 x 坐标的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const double &cp2_x() const { return cp2_x_; }

  /**
   * @brief 获取第二个控制点的 y 坐标。
   * @return 返回第二个控制点 y 坐标的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const double &cp2_y() const { return cp2_y_; }

  /**
   * @brief 将锚点坐标转换为 Imath::V2d 向量。
   * @return 返回一个包含锚点 (x, y) 坐标的 Imath::V2d 对象。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Imath::V2d to_vec() const { return {x_, y_}; }

  /**
   * @brief 将第一个控制点坐标转换为 Imath::V2d 向量。
   * @return 返回一个包含第一个控制点 (cp1_x, cp1_y) 坐标的 Imath::V2d 对象。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Imath::V2d control_point_1_to_vec() const { return {cp1_x_, cp1_y_}; }

  /**
   * @brief 将第二个控制点坐标转换为 Imath::V2d 向量。
   * @return 返回一个包含第二个控制点 (cp2_x, cp2_y) 坐标的 Imath::V2d 对象。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Imath::V2d control_point_2_to_vec() const { return {cp2_x_, cp2_y_}; }

  /** @brief 设置锚点的 x 坐标。 @param x 新的 x 坐标。 */
  void set_x(const double &x) { x_ = x; }
  /** @brief 设置锚点的 y 坐标。 @param y 新的 y 坐标。 */
  void set_y(const double &y) { y_ = y; }
  /** @brief 设置第一个控制点的 x 坐标。 @param cp1_x 新的 cp1_x 坐标。 */
  void set_cp1_x(const double &cp1_x) { cp1_x_ = cp1_x; }
  /** @brief 设置第一个控制点的 y 坐标。 @param cp1_y 新的 cp1_y 坐标。 */
  void set_cp1_y(const double &cp1_y) { cp1_y_ = cp1_y; }
  /** @brief 设置第二个控制点的 x 坐标。 @param cp2_x 新的 cp2_x 坐标。 */
  void set_cp2_x(const double &cp2_x) { cp2_x_ = cp2_x; }
  /** @brief 设置第二个控制点的 y 坐标。 @param cp2_y 新的 cp2_y 坐标。 */
  void set_cp2_y(const double &cp2_y) { cp2_y_ = cp2_y; }

  /**
   * @brief 计算二次贝塞尔曲线上给定 x 值对应的参数 t。
   * 二次贝塞尔曲线由三个点定义：P0(a_x, a_y), P1(b_x, b_y), P2(c_x, c_y)。
   * B_x(t) = (1-t)^2 * a_x + 2(1-t)t * b_x + t^2 * c_x
   * 此函数求解 B_x(t) = x 中的 t。
   * @param x 目标 x 坐标。
   * @param a 第一个点（P0）的 x 坐标。
   * @param b 第二个点（控制点 P1）的 x 坐标。
   * @param c 第三个点（P2）的 x 坐标。
   * @return 返回参数 t (0.0 到 1.0 之间)。
   */
  static double QuadraticXtoT(double x, double a, double b, double c);

  /**
   * @brief 计算二次贝塞尔曲线上给定参数 t 对应的 y 值。
   * B_y(t) = (1-t)^2 * a_y + 2(1-t)t * b_y + t^2 * c_y
   * @param a 第一个点（P0）的 y 坐标。
   * @param b 第二个点（控制点 P1）的 y 坐标。
   * @param c 第三个点（P2）的 y 坐标。
   * @param t 参数 t (0.0 到 1.0 之间)。
   * @return 返回计算得到的 y 坐标。
   */
  static double QuadraticTtoY(double a, double b, double c, double t);

  /**
   * @brief 计算二次贝塞尔曲线上给定 x 值对应的 y 值。
   * 首先调用 QuadraticXtoT 计算参数 t，然后用 t 调用 QuadraticTtoY 计算 y。
   * @param x 目标 x 坐标。
   * @param a 第一个点 P0 (Imath::V2d)。
   * @param b 第二个点（控制点 P1）(Imath::V2d)。
   * @param c 第三个点 P2 (Imath::V2d)。
   * @return 返回计算得到的 y 坐标。
   */
  static double QuadraticXtoY(double x, const Imath::V2d &a, const Imath::V2d &b, const Imath::V2d &c) {
    return QuadraticTtoY(a.y, b.y, c.y, QuadraticXtoT(x, a.x, b.x, c.x));
  }

  /**
   * @brief 计算三次贝塞尔曲线上给定 x 值对应的参数 t。
   * 三次贝塞尔曲线由四个点定义：P0(a_x, a_y), P1(b_x, b_y), P2(c_x, c_y), P3(d_x, d_y)。
   * B_x(t) = (1-t)^3 * a_x + 3(1-t)^2 * t * b_x + 3(1-t) * t^2 * c_x + t^3 * d_x
   * 此函数求解 B_x(t) = x 中的 t。
   * @param x 目标 x 坐标。
   * @param a 第一个点（P0）的 x 坐标。
   * @param b 第二个点（控制点 P1）的 x 坐标。
   * @param c 第三个点（控制点 P2）的 x 坐标。
   * @param d 第四个点（P3）的 x 坐标。
   * @return 返回参数 t (0.0 到 1.0 之间)。
   */
  static double CubicXtoT(double x, double a, double b, double c, double d);

  /**
   * @brief 计算三次贝塞尔曲线上给定参数 t 对应的 y 值。
   * B_y(t) = (1-t)^3 * a_y + 3(1-t)^2 * t * b_y + 3(1-t) * t^2 * c_y + t^3 * d_y
   * @param a 第一个点（P0）的 y 坐标。
   * @param b 第二个点（控制点 P1）的 y 坐标。
   * @param c 第三个点（控制点 P2）的 y 坐标。
   * @param d 第四个点（P3）的 y 坐标。
   * @param t 参数 t (0.0 到 1.0 之间)。
   * @return 返回计算得到的 y 坐标。
   */
  static double CubicTtoY(double a, double b, double c, double d, double t);

  /**
   * @brief 计算三次贝塞尔曲线上给定 x 值对应的 y 值。
   * 首先调用 CubicXtoT 计算参数 t，然后用 t 调用 CubicTtoY 计算 y。
   * @param x 目标 x 坐标。
   * @param a 第一个点 P0 (Imath::V2d)。
   * @param b 第二个点（控制点 P1）(Imath::V2d)。
   * @param c 第三个点（控制点 P2）(Imath::V2d)。
   * @param d 第四个点 P3 (Imath::V2d)。
   * @return 返回计算得到的 y 坐标。
   */
  static double CubicXtoY(double x, const Imath::V2d &a, const Imath::V2d &b, const Imath::V2d &c,
                          const Imath::V2d &d) {
    return CubicTtoY(a.y, b.y, c.y, d.y, CubicXtoT(x, a.x, b.x, c.x, d.x));
  }

 private:
  /**
   * @brief 内部辅助函数，用于计算给定 x 值在二次或三次贝塞尔曲线上的参数 t。
   *
   * 此函数可能是 QuadraticXtoT 和 CubicXtoT 的实际实现或其一部分。
   * @param cubic 如果为 true，则按三次贝塞尔曲线计算；否则按二次贝塞尔曲线计算。
   * @param x 目标 x 坐标。
   * @param a 第一个点的 x 坐标。
   * @param b 第二个点的 x 坐标。
   * @param c 第三个点的 x 坐标。
   * @param d 第四个点的 x 坐标 (仅当 cubic 为 true 时使用)。
   * @return 返回计算得到的参数 t。
   */
  static double CalculateTFromX(bool cubic, double x, double a, double b, double c, double d);

  double x_;     ///< 锚点的 x 坐标。
  double y_;     ///< 锚点的 y 坐标。

  double cp1_x_; ///< 第一个控制点的 x 坐标。
  double cp1_y_; ///< 第一个控制点的 y 坐标。

  double cp2_x_; ///< 第二个控制点的 x 坐标。
  double cp2_y_; ///< 第二个控制点的 y 坐标。
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_BEZIER_H
