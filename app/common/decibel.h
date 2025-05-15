#ifndef DECIBEL_H
#define DECIBEL_H

#include <QtGlobal> // 为了 qFuzzyIsNull
#include <cmath>    // 为了 std::log10, std::pow, std::isinf, std::exp, std::log
#include <limits>   // 为了 std::numeric_limits (如果 ALLOW_RETURNING_INFINITY 被定义)

// #define ALLOW_RETURNING_INFINITY // 控制是否允许返回无穷大值

namespace olive {

/**
 * @brief 提供分贝(dB)与线性振幅值及对数值之间转换的工具类。
 *
 * 此类包含一系列静态方法，用于处理音频领域中常见的单位转换，
 * 例如将线性音量（范围通常0.0到1.0）转换为分贝，或转换为对数刻度
 * （常用于UI滑块以提供更符合人耳感知的分辨率）。
 */
class Decibel {
 public:
  /**
   * @brief 定义一个非常低的分贝值，通常代表静音或接近静音。
   * 这个值在计算上通常对应于 0.0 的线性振幅。
   * 设置为 -200.0 dB。
   */
  static constexpr double MINIMUM = -200.0;

  /**
   * @brief 将线性振幅值转换为分贝 (dB) 值。
   * 计算公式为: $dB = 20 \cdot \log_{10}(linear)$。
   * @param linear 线性振幅值 (通常范围 0.0 到 1.0，其中 1.0 代表最大音量，0.0 代表静音)。
   * @return double 对应的分贝值。如果计算结果为负无穷大 (例如 linear 为 0)，
   * 并且未定义 ALLOW_RETURNING_INFINITY，则返回 MINIMUM (-200.0 dB)。
   */
  static double fromLinear(double linear) {
    double v = double(20.0) * std::log10(linear);
#ifndef ALLOW_RETURNING_INFINITY
    if (std::isinf(v)) { // 检查是否为无穷大 (通常是负无穷)
      return MINIMUM;
    }
#endif
    return v;
  }

  /**
   * @brief 将分贝 (dB) 值转换为线性振幅值。
   * 计算公式为: $linear = 10^{(dB / 20)}$。
   * @param decibel 分贝值。
   * @return double 对应的线性振幅值。如果转换后的线性值非常接近0 (小于 0.000001)，
   * 则直接返回 0.0 以避免浮点精度问题。
   */
  static double toLinear(double decibel) {
    double to_linear = std::pow(double(10.0), decibel / double(20.0));

    // 定义一个阈值，低于此值即视为0，以避免极小的非零值
    if (to_linear < 0.000001) {
      return 0.0; // 返回标准0.0
    } else {
      return to_linear;
    }
  }

  /**
   * @brief 将对数值（通常用于UI滑块，范围0.0到1.0）转换为分贝 (dB) 值。
   * 这种转换通常用于使滑块在低音量区域有更高的精度。
   * @param logarithmic 对数值，范围通常是 [0.0, 1.0]。
   * @return double 对应的分贝值。
   * 如果 logarithmic 接近0 (小于0.001)，根据 ALLOW_RETURNING_INFINITY 宏返回无穷大或 MINIMUM。
   * 如果 logarithmic 接近1 (大于0.99)，返回 0 dB。
   */
  static double fromLogarithmic(double logarithmic) {
    if (logarithmic < 0.001) // 接近静音
#ifdef ALLOW_RETURNING_INFINITY
      return std::numeric_limits<double>::infinity(); // 或者可能是负无穷大，取决于期望
#else
      return MINIMUM;
#endif
    else if (logarithmic > 0.99) // 接近最大音量 (0dB)
      return 0.0; // 返回标准0.0
    else
      // 具体转换公式，可能基于特定的对数映射曲线
      return 20.0 * std::log10(-std::log(1.0 - logarithmic) / LOG100);
  }

  /**
   * @brief 将分贝 (dB) 值转换为对数值（通常用于UI滑块，范围0.0到1.0）。
   * @param decibel 分贝值。
   * @return double 对应的对数值。
   * 如果 decibel 接近0 (使用 qFuzzyIsNull 判断)，则返回 1.0 (表示最大音量)。
   */
  static double toLogarithmic(double decibel) {
    if (qFuzzyIsNull(decibel)) { // 检查是否接近0dB
      return 1.0; // 返回标准1.0
    } else {
      // 具体转换公式
      return 1.0 - std::exp(-std::pow(10.0, decibel / 20.0) * LOG100);
    }
  }

  /**
   * @brief 将线性振幅值直接转换为对数值。
   * @param linear 线性振幅值。
   * @return double 对应的对数值。
   */
  static double LinearToLogarithmic(double linear) {
    // 公式: 1 - e^(-linear * LOG100)
    // LOG100 大约是 ln(100)
    return 1.0 - std::exp(-linear * LOG100);
  }

  /**
   * @brief 将对数值直接转换为线性振幅值。
   * @param logarithmic 对数值。
   * @return double 对应的线性振幅值。
   * 如果 logarithmic 接近1 (大于0.99)，返回1.0。
   */
  static double LogarithmicToLinear(double logarithmic) {
    if (logarithmic > 0.99) { // 接近最大
      return 1.0; // 返回标准1.0
    } else {
      // 公式: -ln(1 - logarithmic) / LOG100
      return -std::log(1.0 - logarithmic) / LOG100;
    }
  }

 private:
  /**
   * @brief 一个预计算的常量，等于 ln(100)，约等于 4.60517018599。
   * 用于上述对数转换公式中。
   */
  static constexpr double LOG100 = 4.60517018599;
};

}  // namespace olive

#endif  // DECIBEL_H