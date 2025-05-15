#ifndef LERP_H
#define LERP_H

// 此文件通常不需要包含特定的 Olive 或 Qt 头文件，
// 因为 lerp 函数是通用的数学函数，仅依赖于模板类型 T 和 double/float。

// namespace olive { // 如果这些函数应属于特定命名空间

/**
 * @brief 在值 a 和 b 之间使用参数 t 进行线性插值。
 *
 * 参数 t 应为一个介于 0.0 和 1.0 之间的数值。
 * 当 t 为 0.0 时，返回 a。
 * 当 t 为 1.0 时，返回 b。
 * 当 t 在 0.0 和 1.0 之间时，返回 a 和 b 之间对应比例的线性插值结果。
 * 计算公式为： $result = a \cdot (1.0 - t) + b \cdot t$。
 *
 * @tparam T 值 a 和 b 的类型，也是返回值的类型。此类型应支持与 double 类型的乘法以及相互之间的加法。
 * @param a 起始值。
 * @param b 结束值。
 * @param t 插值因子，应在 [0.0, 1.0] 范围内以获得标准的线性插值。
 * 如果 t 超出此范围，函数仍会按线性外插法计算结果。
 * @return T 在 a 和 b 之间的线性插值结果。
 */
template <typename T>
T lerp(T a, T b, double t) {
  return (a * (1.0 - t)) + (b * t);
}

/**
 * @brief 在值 a 和 b 之间使用参数 t (float 类型) 进行线性插值。
 *
 * 此函数是 lerp(T, T, double) 的一个重载版本，接受 float 类型的插值因子 t。
 * 参数 t 应为一个介于 0.0f 和 1.0f 之间的数值。
 * 计算公式为： $result = a \cdot (1.0f - t) + b \cdot t$。
 *
 * @tparam T 值 a 和 b 的类型，也是返回值的类型。此类型应支持与 float 类型的乘法以及相互之间的加法。
 * @param a 起始值。
 * @param b 结束值。
 * @param t 插值因子 (float 类型)，应在 [0.0f, 1.0f] 范围内以获得标准的线性插值。
 * @return T 在 a 和 b 之间的线性插值结果。
 */
template <typename T>
T lerp(T a, T b, float t) {
  return (a * (1.0f - t)) + (b * t);
}

// } // namespace olive (如果适用)

#endif  // LERP_H