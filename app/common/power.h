#ifndef POWER_H
#define POWER_H

#include <stdint.h> // 为了 uint32_t

#include "common/define.h" // 可能包含一些通用定义，虽然在此文件中未直接使用

namespace olive {

/**
 * @brief 将一个32位无符号整数向上取整到最接近的2的幂。
 *
 * 例如：
 * - ceil_to_power_of_2(0) -> 0 (特殊情况，按位操作结果为0)
 * - ceil_to_power_of_2(1) -> 1
 * - ceil_to_power_of_2(2) -> 2
 * - ceil_to_power_of_2(3) -> 4
 * - ceil_to_power_of_2(7) -> 8
 * - ceil_to_power_of_2(8) -> 8
 *
 * 此函数使用一系列位操作来实现高效计算。
 * 如果输入已经是2的幂，则返回其本身。
 * 如果输入为0，则返回0。
 *
 * @param v 要向上取整的32位无符号整数。
 * @return uint32_t 大于或等于 v 的最小的2的幂。
 */
inline uint32_t ceil_to_power_of_2(uint32_t v) {
  if (v == 0) return 0; // 处理输入为0的特殊情况
  v--; // 先减1，确保如果v本身是2的幂，结果不变（例如v=8, v-1=7, 最后结果还是8）
       // 如果v不是2的幂，例如v=7, v-1=6, 最后结果是8
  v |= v >> 1;  // 将最高位的1向右传播，填满其右边的所有位
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++; // 最后加1，得到大于等于原v的最小的2的幂

  return v;
}

/**
 * @brief 将一个32位无符号整数向下取整到最接近的2的幂。
 *
 * 例如：
 * - floor_to_power_of_2(0) -> 0
 * - floor_to_power_of_2(1) -> 1
 * - floor_to_power_of_2(2) -> 2
 * - floor_to_power_of_2(3) -> 2
 * - floor_to_power_of_2(7) -> 4
 * - floor_to_power_of_2(8) -> 8
 *
 * 此函数使用一系列位操作来实现高效计算。
 * 如果输入已经是2的幂，则返回其本身。
 * 如果输入为0，则返回0。
 *
 * @param x 要向下取整的32位无符号整数。
 * @return uint32_t 小于或等于 x 的最大的2的幂。
 */
inline uint32_t floor_to_power_of_2(uint32_t x) {
  if (x == 0) return 0; // 处理输入为0的特殊情况
  // 下面的位操作会将x中最高有效位（MSB）右边的所有位都设置为1
  x = x | (x >> 1);
  x = x | (x >> 2);
  x = x | (x >> 4);
  x = x | (x >> 8);
  x = x | (x >> 16);
  // 此时，x的形式是 0...011...1 (MSB位置为1，其右边全为1，或者x本身就是0...010...0的形式)
  // 通过 x - (x >> 1) 可以得到最高位的2的幂
  // 例如：x = 7 (0111) -> 操作后 x = 7 (0111) -> 7 - (7>>1) = 7 - 3 = 4
  //       x = 8 (1000) -> 操作后 x = 15 (1111) -> 15 - (15>>1) = 15 - 7 = 8
  //       x = 6 (0110) -> 操作后 x = 7 (0111)  -> 7 - 3 = 4
  return x - (x >> 1);
}

}  // namespace olive

#endif  // POWER_H