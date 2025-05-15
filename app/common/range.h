#ifndef RANGE_H
#define RANGE_H

// 此文件通常不需要包含特定的 Olive 或 Qt 头文件，
// 因为 InRange 函数是通用的数学/逻辑比较函数，仅依赖于模板类型 T。
// 如果 T 是自定义类型，则需要确保 T 支持 >=, <=, +, - 操作符。

// namespace olive { // 如果此函数应属于特定命名空间

/**
 * @brief 检查值 a 是否在以值 b 为中心、范围为 range 的区间内。
 *
 * 具体来说，此函数判断 a 是否满足 $b - range \le a \le b + range$。
 *
 * @tparam T 值 a, b, 和 range 的类型。此类型应支持减法、加法、大于等于 (>=) 和小于等于 (<=) 比较操作。
 * @param a 要检查的值。
 * @param b 区间的中心值。
 * @param range 区间的半径（从中心值 b 到区间边界的距离）。此值应为非负数以获得有意义的区间。
 * @return bool 如果 a 在 $[b - range, b + range]$ 区间内（包含边界），则返回 true；否则返回 false。
 */
template <typename T>
bool InRange(T a, T b, T range) {
  return (a >= (b - range) && a <= (b + range));
}

// } // namespace olive (如果适用)

#endif  // RANGE_H